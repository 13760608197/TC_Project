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

//�ְ����ͷ�ʽ�������ݷ�ΪMAXMTU+PACKETHEAD���ȵİ����з���
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
		//ͨ��������ת��
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
			//�������ݳ���
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
		pbody->dead = 0;			//��ͨ��������ת��
		Pos = PACKETHEAD;
		for(i=0;i<Cnt;i++) {
			pbody->packet_idx = i;
			if(i) {
				memcpy(&pData[Pos-PACKETHEAD],pData,PACKETHEAD);
			}
			//�������ݳ���
			Len = ((size-Pos)>MAXMTU?MAXMTU:(size-Pos))+PACKETHEAD;
			This->udp->SendBuffer(This->udp,This->cPeerIP,PeerPort,
				&pData[Pos-PACKETHEAD],Len);

			if(Public.fTransmitToCenter && (strcmp(This->cPeerIP,Public.pCenterMsg->IP)!=0)) {//����������� 
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
				printf("������:%d ������ʱʱ��Ϊ:%d\n",This->LostFramePrecent,This->DelayTime);
			}
		}
	}
	return size;
}
//---------------------------------------------------------------------------
//�ְ����շ�ʽ,�ڱ������д���������зְ���ƴ���Ĺ���
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

	//���IP���ԣ������ð�
	for(i=0;i<MAX_TIMEOUT_CNT;i++) {
		unsigned int PeerIP;
		Pos = This->udp->RecvBuffer(This->udp,pBuf,size,TimeOut,sockaddr,&Size);
		if(Pos<=0) {
			//���մ�����������̴���
			break;
		}
		PeerIP = GetIPBySockAddr(sockaddr);
		if(PeerIP==This->dwPeerIP)
		{   //if((PeerIP==This->dwPeerIP) || (PeerIP==This->dwMasterDevIP)) {
			//�Ѿ����յ�ֱ���İ�
			//This->bRecvLocalPacket = TRUE;
			RecvIP = PeerIP;
			break;
		}
		if(PeerIP==This->dwWEBSrvIP)//if(!This->bRecvLocalPacket && PeerIP==This->dwWEBSrvIP)
		{
			//�������INTERNET�ĺ���
			RecvIP = PeerIP;
			break;
		}
	}
	if(i==MAX_TIMEOUT_CNT) {
		return 0;
	}
	//������հ��Ķ˿ں�
//	if(Pos>=(int)PACKETHEAD) {
//		if(pbody->dead==0)
//		{
//			//pbody->dead �Ƿ��ͷ��Ķ˿ں�
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
	//���մ�����
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
	//�װ���������Ϊ0
	if(pbody->packet_idx!=0) {
		return 0;
	}

	//����дӻ���ת����IP��
//	if(This->ViceDeviceIP[0]) {
//		This->udp->SendBuffer(This->udp,This->ViceDeviceIP,8800,pBuf,Pos);
//	}

	//�����ж��ٸ����ݰ�
	Cnt = pbody->packet_cnt;
	PackID = pbody->seq;
	PackSize = pbody->packet_size;
	if(Cnt==1) {
		if(This->bInternet)
		{
			//������
			This->LostFramePrecent = pbody->atype;
		}
		return Pos;
	}
	//����ʣ��ķְ�
	pbody = (rec_body*)cTmpBuf;
	for(i=1;i<Cnt;i++) {
		//���IP���ԣ������ð�
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

		if(pbody->seq!=PackID) {				//֡�Ŵ���
			return 0;
		}
		if(pbody->packet_idx>=Cnt) {			//�ְ���������
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
		//������
		This->LostFramePrecent = pbody->atype;
	}
	return Pos;
}
//---------------------------------------------------------------------------
//  ����һ��UDP�ͻ��ˣ�PortΪ0�򲻰󶨶˿�
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

	//����IP��ַ�ж��Ƿ����INTERNET�Ĵ��䣬����INTERNET�Ĵ����ٶȽ�������һЩ
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
