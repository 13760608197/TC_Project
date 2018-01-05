#ifndef TLINK_SIP

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "externfunc.h"
extern unsigned int PacketID;
#if (defined TC3000_BZ)
#define MAXMTU (1400)  //xb modify 20150821
#else
#define MAXMTU (512)
#endif
#define MAX_TIMEOUT_CNT 10
void ErrorLog(int ecode,const char *fmt,...);
BOOL IsSameNetMask(const char *IP1,const char *IP2);
unsigned int my_inet_addr(const char *IP);
unsigned int GetPortBySockAddr(const void *Data);
unsigned int GetIPBySockAddr(const void *Data);
//----------------------------------------------------------------------------
static int TRTPObject_SendPortNumber(TRTPObject *This,const char *IP,int Port,const char *SrvIP,unsigned int UserID)
{
	TRTPBURROW RtpBurrow;
	RtpBurrow.ID = PacketID++;
	RtpBurrow.Type = TP_RTPBURROW;
	RtpBurrow.Size = sizeof(TRTPBURROW);
	RtpBurrow.UserID = UserID;
	RtpBurrow.IP = my_inet_addr(IP);
	RtpBurrow.Port = Port;
	RtpBurrow.Times = 0;
	This->udp->TrusthSend(This->udp,SrvIP,9888,&RtpBurrow,RtpBurrow.Size);
	return 0;
}
//----------------------------------------------------------------------------
static void TRTPObject_SetPeerPort(TRTPObject *This,int PeerPort)
{
	This->PeerPort = PeerPort;
	//printf("Set 2 PeerPort=%d\n", PeerPort);
}
//---------------------------------------------------------------------------
static void TRTPObject_Destroy(TRTPObject *This)
{
	This->udp->Destroy(This->udp);
    free(This);
	This = NULL;  //add by Jack
}
//---------------------------------------------------------------------------
void DelayMs(int ms);
#define PACKETHEAD ((int)(sizeof(rec_body)-MAXENCODEPACKET))

//分包发送方式，将数据分为MAXMTU+PACKETHEAD长度的包进行发送
static int TRTPObject_SendBuffer(TRTPObject *This,void *pBuf,int size)
{
	int i,Cnt,Pos,Len;
	int PeerPort = This->PeerPort;
	char *pData = (char*)pBuf;

	Cnt = (size - PACKETHEAD + MAXMTU - 1) / MAXMTU;
	if(Cnt==0) {
		Cnt = 1;
	}

	if(This->bInternet && !This->bRecvLocalPacket && (++This->SendPacketIdx)>8)
	{
		//通过服务器转发
		char SendBuffer[MAXMTU+sizeof(TTransPacket)+PACKETHEAD];
		rec_body *pbody = (rec_body*)&SendBuffer[sizeof(TTransPacket)];
		TTransPacket *Head = (TTransPacket*)SendBuffer;

		Head->Size = MAXMTU+sizeof(TTransPacket)+PACKETHEAD;
		Head->Type = TP_TRANSRTP;
		Head->RecvIP = This->dwPeerIP;
		Head->RecvPort = This->PeerPort;

		memcpy(pbody,pBuf,PACKETHEAD);
		pbody->packet_cnt = Cnt;
		pbody->packet_size = MAXMTU;
		Pos = PACKETHEAD;
		for(i=0;i<Cnt;i++) {
			Head->ID = PacketID++;
			pbody->packet_idx = i;
			Len = (size-Pos)>MAXMTU?MAXMTU:(size-Pos);
			if(Len)
				memcpy(&SendBuffer[PACKETHEAD+sizeof(TTransPacket)],&pData[Pos],Len);
			//发送数据长度
			This->udp->SendBuffer(This->udp,This->cServerIP,TC_DNS_PORT,
				SendBuffer,Len+PACKETHEAD+sizeof(TTransPacket));
			if(This->bInternet) {
				DelayMs(This->DelayTime);
			}
			Pos+=MAXMTU;
		}
	}
	else
	{
		rec_body *pbody = (rec_body*)pBuf;
		pbody->packet_cnt = Cnt;
		pbody->packet_size = MAXMTU;
		pbody->dead = 0;			//不通过服务器转发
		Pos = PACKETHEAD;
		for(i=0;i<Cnt;i++) {
			pbody->packet_idx = i;
			if(i) {
				memcpy(&pData[Pos-PACKETHEAD],pData,PACKETHEAD);
			}
			//发送数据长度
			Len = ((size-Pos)>MAXMTU?MAXMTU:(size-Pos))+PACKETHEAD;
			This->udp->SendBuffer(This->udp,This->cPeerIP,PeerPort,
				&pData[Pos-PACKETHEAD],Len);

			if(Public.fTransmitToCenter && (strcmp(This->cPeerIP,Public.pCenterMsg->IP)!=0)) {//发送随监数据 
				This->udp->SendBuffer(This->udp, Public.pCenterMsg->IP, PeerPort,
						&pData[Pos-RECHEADSIZE],Len);				
			}

			Pos+=MAXMTU;
			//printf("cPeerIP=%s, PeerPort=%d\n", This->cPeerIP,PeerPort);
			if(This->bInternet) {
				DelayMs(This->DelayTime);
			}
		}
	}
	if(This->bInternet)
	{
		if(++This->SendPacketCnt>30)
		{
			int DelayTime = This->DelayTime;
			This->SendPacketCnt = 0;
			if(This->LostFramePrecent>10) {
				DelayTime = This->DelayTime*2;
				if(DelayTime>50) {
					DelayTime = 50;
				}
			}
			else if(This->LostFramePrecent && This->LostFramePrecent<7) {
				DelayTime = This->DelayTime/2;
				if(DelayTime<1) {
					DelayTime = 1;
				}
			}
			if(DelayTime!=This->DelayTime)
			{
				This->DelayTime = DelayTime;
				printf("丢包率:%d 更改延时时间为:%d\n",This->LostFramePrecent,This->DelayTime);
			}
		}
	}
	return size;
}
//---------------------------------------------------------------------------
//分包接收方式,在本函数中处理接收所有分包及拼包的过程
//---------------------------------------------------------------------------
static int TRTPObject_RecvBuffer(TRTPObject *This,void *pBuf,int size,int TimeOut)
{
	int i,j,Cnt,Pos,Len;
    unsigned int PackID;
	char cTmpBuf[MAXMTU+PACKETHEAD],*pData;
	rec_body *pbody = (rec_body*)pBuf;
	char sockaddr[64];
	unsigned int RecvIP;
	int Size = sizeof(sockaddr);
	pData = (char*)pBuf;
	int PackSize = 0;

	//如果IP不对，丢弃该包
	for(i=0;i<MAX_TIMEOUT_CNT;i++) {
		unsigned int PeerIP;
		Pos = This->udp->RecvBuffer(This->udp,pBuf,size,TimeOut,sockaddr,&Size);
		if(Pos<=0) {
			//接收错误，由下面过程处理
			break;
		}
		PeerIP = GetIPBySockAddr(sockaddr);
		if(PeerIP==This->dwPeerIP)
		{   //if((PeerIP==This->dwPeerIP) || (PeerIP==This->dwMasterDevIP)) {
			//已经接收到直传的包
			//This->bRecvLocalPacket = TRUE;
			RecvIP = PeerIP;
			break;
		}
		if(PeerIP==This->dwWEBSrvIP)//if(!This->bRecvLocalPacket && PeerIP==This->dwWEBSrvIP)
		{
			//如果基于INTERNET的呼叫
			RecvIP = PeerIP;
			break;
		}
	}
	if(i==MAX_TIMEOUT_CNT) {
		return 0;
	}
	//保存接收包的端口号
//	if(Pos>=(int)PACKETHEAD) {
//		if(pbody->dead==0)
//		{
//			//pbody->dead 是发送方的端口号
//			int Port = GetPortBySockAddr(sockaddr);
//			if(Port!=This->PeerPort) {
//				This->PeerPort = Port;
//				printf("Remote RTP Port is %d\n",This->PeerPort);
//			}
//		}
//		else
//		{
//			if(pbody->dead!=This->PeerPort) {
//				This->PeerPort = pbody->dead;
//				printf("Remote RTP Port is %d\n",This->PeerPort);
//			}
//		}
//	}
	//接收错误处理
	if(Pos<PACKETHEAD) {
		if(Pos<=0) {
			This->RecvTimeOut++;
			if (This->RecvTimeOut > 1) {
				printf("[1]Rtp recv data timeout %d\n",This->RecvTimeOut);
			}
			if(This->RecvTimeOut<MAX_TIMEOUT_CNT)
				return 0;
			return -1;
		} else {
			return 0;
		}
	}
	This->RecvTimeOut=0;
	//首包索引必须为0
	if(pbody->packet_idx!=0) {
		return 0;
	}

	//如果有从机，转发该IP包
//	if(This->ViceDeviceIP[0]) {
//		This->udp->SendBuffer(This->udp,This->ViceDeviceIP,8800,pBuf,Pos);
//	}

	//保存有多少个数据包
	Cnt = pbody->packet_cnt;
	PackID = pbody->seq;
	PackSize = pbody->packet_size;
	if(Cnt==1) {
		if(This->bInternet)
		{
			//丢包率
			This->LostFramePrecent = pbody->atype;
		}
		return Pos;
	}
	//接收剩余的分包
	pbody = (rec_body*)cTmpBuf;
	for(i=1;i<Cnt;i++) {
		//如果IP不对，丢弃该包
		for(j=0;j<MAX_TIMEOUT_CNT;j++) {
			Len = This->udp->RecvBuffer(This->udp,cTmpBuf,MAXMTU+PACKETHEAD,TimeOut,sockaddr,&Size);
			if(Len<=0) {
				break;
			}
			if(GetIPBySockAddr(sockaddr)==RecvIP) {
				break;
			}
		}
		if(j==MAX_TIMEOUT_CNT)
			Len = 0;

		if(Len<=0) {
			This->RecvTimeOut++;
			printf("[2]Rtp recv data timeout %d\n",This->RecvTimeOut);
			if(This->RecvTimeOut<MAX_TIMEOUT_CNT)
				return 0;
			return -1;
		}

		if(pbody->seq!=PackID) {				//帧号错误
			return 0;
		}
		if(pbody->packet_idx>=Cnt) {			//分包索引错误
			return 0;
		}

//		if(This->ViceDeviceIP[0]) {
//			This->udp->SendBuffer(This->udp,This->ViceDeviceIP,8800,cTmpBuf,Len);
//		}

		if(Len>PACKETHEAD)
			memcpy(&pData[PACKETHEAD+pbody->packet_idx*MAXMTU],&cTmpBuf[PACKETHEAD],Len-PACKETHEAD);
		Pos+=Len-PACKETHEAD;
	}
	if(This->bInternet)
	{
		//丢包率
		This->LostFramePrecent = pbody->atype;
	}
	return Pos;
}
//---------------------------------------------------------------------------
//  创建一个UDP客户端，Port为0则不绑定端口
//---------------------------------------------------------------------------
TRTPObject* TRTPObject_Create(const char *PeerIP,char *LocalIP,const char *MasterDevIP,
		const char *ViceDeviceIP,int PeerPort,const char *ServerIP)
{
	TRTPObject* This;

	This = (TRTPObject *)malloc(sizeof(TRTPObject));
	if(This==NULL) {
		//        ErrorLog(0,"alloc RTPObject memory failt!\n");
		//printf("Set Error 1 PeerPort=%d\n", PeerPort);
		return NULL;
	}
	memset(This,0,sizeof(TRTPObject));

	strcpy(This->cPeerIP,PeerIP);
	This->udp = TUDPClient_Create(PeerPort);
	if(This->udp == NULL) {
		free(This);
		//        ErrorLog(0,"Create rtp socket is error!\n");
		//printf("Set Error 2 PeerPort=%d\n", PeerPort);
		return NULL;
	}

	//根据IP地址判断是否基于INTERNET的传输，基于INTERNET的传输速度将调整慢一些
	This->bInternet = 0; //!IsSameNetMask(LocalIP,PeerIP);

	This->dwPeerIP = my_inet_addr(PeerIP);
	//	strncpy(This->cMasterDevIP,MasterDevIP,16);
	//	strncpy(This->ViceDeviceIP,ViceDeviceIP,16);
	//	if(MasterDevIP[0]) {
	//		This->dwMasterDevIP = my_inet_addr(MasterDevIP);
	//	} else {
	//		This->dwMasterDevIP = 0;
	//	}
	strncpy(This->cServerIP,ServerIP,15);
	This->dwWEBSrvIP = my_inet_addr(ServerIP);
	This->DelayTime = 10;
	//printf("Set 1 PeerPort=%d\n", PeerPort);
	This->PeerPort = PeerPort;
	This->Destroy = TRTPObject_Destroy;
	This->RecvBuffer = TRTPObject_RecvBuffer;
	This->SendBuffer = TRTPObject_SendBuffer;
	This->SetPeerPort = TRTPObject_SetPeerPort;
	This->SendPortNumber = TRTPObject_SendPortNumber;
	//	SendPortNumber(This,IP,Port);
	return This;

}
//---------------------------------------------------------------------------
#endif
