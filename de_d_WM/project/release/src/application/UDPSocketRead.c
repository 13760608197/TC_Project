/*
 * =====================================================================================
 *
 *       Filename:  UDPSocketRead.c
 *
 *    Description:  网口协议接收
 *
 *        Version:  1.0
 *        Created:  2015-11-20 14:08:55
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
/* ----------------------------------------------------------------*
 *                      include head files
 *-----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <time.h>
#include <sys/time.h>

#include <signal.h>

#include "externfunc.h"

/* ----------------------------------------------------------------*
 *                  extern variables declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                  internal functions declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                        macro define
 *-----------------------------------------------------------------*/
#if DBG_SOCKETREAD > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif


/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
/**
 * @brief UDPCallProc 呼叫协议处理
 *
 * @param ABinding
 * @param AData
 */
static void udpCallProc(SocketHandle *ABinding,SocketPacket *AData)
{
	video->udpCallHandle(video,ABinding,AData);
}
/**
 * @brief UDPLocalMsgGetProc 设备信息获取
 *
 * @param ABinding
 * @param AData
 */
static void udpLocalMsgGetProc(SocketHandle *ABinding,SocketPacket *AData)
{
	DBG_P("TP_DEVCHECK:LocalMsgGetProc()\n");
	if(AData->Size==sizeof(TGetDeviceInfo)) {
		DBG_P("MsgGetProc():TGetDeviceInfo\n");
		//返回本机设备信息
		TGetDeviceInfo *GetDevMsg = (TGetDeviceInfo*)AData->Data;
		if(GetDevMsg->Cmd==CMD_GETSTATUS &&
			(GetDevMsg->DevType==TYPE_DOOR || GetDevMsg->DevType==0xFFFFFFFF)) { //类型为中控主机或所有设备
			TRetDeviceInfo RetDevMsg;
			const char *pDestIP;
			memset(&RetDevMsg,0,sizeof(TRetDeviceInfo));
			RetDevMsg.ID = PacketID++;
			RetDevMsg.Size = sizeof(TRetDeviceInfo);
			RetDevMsg.Type = TP_DEVCHECK;
			RetDevMsg.Cmd = CMD_RETSTATUS;			//返回设备信息
			RetDevMsg.DevType = TYPE_DOOR;
			strcpy(RetDevMsg.Code,Public.cRoomID);		//虚拟编号
			strcpy(RetDevMsg.Name,DEVICE_MODE);	//房间号码
			strcpy(RetDevMsg.IP,Public.cLocalIP);	//IP
			strcpy(RetDevMsg.dType,"门口机");		//类型
			sprintf(RetDevMsg.IMEI,"%llX",Public.dwDevID);		//设备唯一编号
			//========================================================
			// xiehb 20120409 增加上报本机软件版本号
			RetDevMsg.Addition[0] = 'V';
			RetDevMsg.Addition[1] = Public.SoftVer.Major+0x30;
			RetDevMsg.Addition[2] = '.';
			RetDevMsg.Addition[3] = Public.SoftVer.Minor+0x30;
			RetDevMsg.Addition[4] = Public.SoftVer.Release+0x30;
			RetDevMsg.Addition[5] = ' ';
			RetDevMsg.Addition[6] = 'K';
			RetDevMsg.Addition[7] = Public.KernelVer.Major+0x30;
			RetDevMsg.Addition[8] = '.';
			RetDevMsg.Addition[9] = Public.KernelVer.Minor+0x30;
			RetDevMsg.Addition[10] = Public.KernelVer.Release+0x30;
			//=======================================================

			char tmp[20];
			strcpy(tmp, "[H]");
			memcpy(&tmp[3], RetDevMsg.Addition, 11);
			memcpy(RetDevMsg.Addition, tmp, 15);

			if(Public.BroadCastFilter(Public.cLocalIP,ABinding->IP)) {
				UdpServer->SendBuffer(UdpServer,ABinding->IP,ABinding->Port, 		//返回信息
				(char *)&RetDevMsg,sizeof(TRetDeviceInfo));
			} else {
				UdpServer->SendBuffer(UdpServer,"255.255.255.255",ABinding->Port, 		//返回信息
						(char *)&RetDevMsg,sizeof(TRetDeviceInfo));
			}
		}
	} else if(AData->Size==sizeof(TRetDeviceInfo)) {
		DBG_P("MsgGetProc():TRetDeviceInfo\n");
		//其它设备返回设备信息
		TRetDeviceInfo *pDevMsg = (TRetDeviceInfo*)AData->Data;
		if(pDevMsg->Cmd==CMD_RETSTATUS) {
			if(pDevMsg->DevType==TYPE_DEVID_SRV) {
				//获取设备唯一编号
				if(Public.bGetDevID) {
					Public.bGetDevID = FALSE;
					TResDeviceInf Packet;
					memset(&Packet,0,sizeof(TResDeviceInf));
					Packet.ID = PacketID++;
					Packet.Size = sizeof(TResDeviceInf);
					Packet.Type = TP_LOCALDEVID;
					Packet.Cmd = REQSERVERINF;
					Packet.ReqType = TYPE_DOOR;
					UdpServer->AddTask(UdpServer,ABinding->IP,ABinding->Port,&Packet,Packet.Size,5,0,NULL,NULL);
					printf("Get Local ID From %s\n",ABinding->IP);
				}
			}
		}
	}
}

/**
 * @brief UDPUpdateProc 远程升级
 *
 * @param ABinding
 * @param AData
 *
 * @return
 */
static void udpUpdateProc(SocketHandle *ABinding,SocketPacket *AData)
{
	DBG_P("TP_RETUPDATEMSG:UpdateProc()\n");
	TUpdateRevertFile *cBuf;
	static BOOL bUpdateProc = FALSE;			//处理收到重复的升级通知包时
	char *Packet = (char *)&AData->Data[sizeof(COMMUNICATION)];

	if(bUpdateProc == FALSE) {
		bUpdateProc = TRUE;
		cBuf = (TUpdateRevertFile *)malloc(sizeof(TUpdateRevertFile));

		sprintf(cBuf->ip,"%s",ABinding->IP);
		sprintf(cBuf->file,"%s",Packet);
		form_main->createForm(FORM_UPDATE,(void*)cBuf);
		bUpdateProc = FALSE;
	}
	free(cBuf);  /*注:上面的线程还在跑，这里提前释放会出问题(有UI时已经新开辟内存)*/
}


/**
 * @brief UDPSetSysTime 设置系统时间 和管理中心同步
 *
 * @param ABinding
 * @param AData
 */
static void udpSetSysTime(SocketHandle *ABinding,SocketPacket *AData)
{
	DBG_P("TP_SETSYSTIME:SetSysTime()\n");
	CFGDEVICE *pData = (CFGDEVICE *)&AData->Data[0];
	if(pData->Cmd == 1) {
		SETSYSTIME *pTime = (SETSYSTIME *)pData->Data;
		adjustdate( pTime->year,pTime->mon,pTime->day,
					pTime->hour,pTime->min,pTime->sec);
	}
	Public.CenterOnlineTime = 61;
}

/**
 * @brief UDPDownIPproc 配置本地IP地址,mask,gateway等
 *
 * @param ABinding
 * @param AData
 */
static void udpDownIPproc(SocketHandle *ABinding,SocketPacket *AData)
{
	DBG_P("TP_IPDOWN:DownIPproc()\n");
	int i;
	const char *pDestIP;
	CFGDEVICE *Media= (CFGDEVICE *)AData->Data;
	if(Media->Cmd != 1) //信息指令
		return;
	CONFIGIP * cfg = (CONFIGIP *)Media->Data;
	//NetParamSet NetSet;
	char LocalIP[16];		// 本机IP地址
	char NetMask[16];		// 子网掩码
	char GateWay[16];       // 网关
	for(i=0;i<Media->Count;i++){
		//配置管理中心IP
		if(cfg->KeyCode==IP_MANAGECENTER && strlen(cfg->IP)>0) {
			DBG_P("CenterSet:Center IP:%s\n",cfg->IP);
			Public.UpdateCenterIP("UserInfo",cfg->IP,sizeof(cfg->IP)-1);
			cfg++;
			continue;
		}
		//配置设备IP
		if(cfg->KeyCode==IP_DEVICE && strlen(cfg->IP)>0) {
			DBG_P("CenterSet:Local IP:%s\n",cfg->IP);
			strncpy(LocalIP,cfg->IP,sizeof(cfg->IP)-1);
			cfg++;
			continue;

		}
		//子网掩码
		if(cfg->KeyCode==IP_SUBNETMASK && strlen(cfg->IP)>0) {
			DBG_P("CenterSet:Netmask:%s\n",cfg->IP);
			strncpy(NetMask,cfg->IP,sizeof(cfg->IP)-1);
			cfg++;
			continue;
		}
		//配置网关
		if(cfg->KeyCode==IP_GATEWAY && strlen(cfg->IP)>0) {
			DBG_P("CenterSet:GateWay:%s\n",cfg->IP);
			strncpy(GateWay,cfg->IP,sizeof(cfg->IP)-1);
			if (jugdeRecIP(LocalIP,GateWay,NetMask)) {
				strncpy(Public.cLocalIP,LocalIP,sizeof(cfg->IP)-1);
				strncpy(Public.cNetMask,NetMask,sizeof(cfg->IP)-1);
				strncpy(Public.cGateWay,GateWay,sizeof(cfg->IP)-1);
				SetValueToEtcFile("interface.ini","Network","GateWay",cfg->IP);
			} else {
				printf("set Gateway err:local:%s,des:%s,mask:%s\n", LocalIP,GateWay,NetMask);
				return;
			}
			cfg++;
			continue;
		}
		//配置网卡地址
		if(cfg->KeyCode==IP_NETCARMAC && strlen(cfg->IP)>0) {
			DBG_P("CenterSet:Mac:%s\n",cfg->IP);
			strncpy(Public.MAC,cfg->IP,sizeof(cfg->IP)-1);
			char imei_buf[14];
			sprintf(imei_buf,"92%s",Public.MAC);
			Public.dwDevID = (unsigned long long)strtoull(imei_buf,NULL,16);
			SetValueToEtcFile_Ext("interface.ini","PUBLIC","IMEI",imei_buf);
			cfg++;
			continue;
		}
    }
	SetNetwork(1,Public.cLocalIP,Public.cNetMask,Public.cGateWay,Public.MAC);
	//不管有没有数据，都需要回复信息
	//Public.SendUdpData(ABinding->IP,UDPSERVER_PORT,TP_IPDOWN,NULL,0);
	pDestIP = GetSendIP(Public.cLocalIP,ABinding->IP,Public.cNetMask);
	Public.SendUdpData(pDestIP,ABinding->Port,TP_IPDOWN,NULL,0);	//返回信息
}

/**
 * @brief UDPReadIPproc 用户管理中心读取 本机ip
 *
 * @param ABinding
 * @param AData
 */
static void udpReadIPproc(SocketHandle *ABinding,SocketPacket *AData)
{
	DBG_P("TP_READIP:ReadIPproc()\n");
	char buff[200]={0};
	CFGDEVICE *File = (CFGDEVICE *)buff;
	CONFIGIP *cfg ;
	File->ID = PacketID++;
	File->Size = sizeof(buff);
	File->Type = TP_READIP;
	File->Cmd =(int)AData->Data[sizeof(COMMUNICATION)];
	File->Count = 4;
	// File->Count = 5;
	cfg = (CONFIGIP *)File->Data;
	if(File->Cmd == 2) {
		cfg->KeyCode = IP_DEVICE;
		strncpy(cfg->IP,Public.cLocalIP,16);
		cfg++;
		cfg->KeyCode = IP_MANAGECENTER;
		strncpy(cfg->IP,Public.pCenterMsg->IP,16);
		cfg++;
		cfg->KeyCode = IP_SUBNETMASK;
		strncpy(cfg->IP,Public.cNetMask,16);
		cfg++;
		cfg->KeyCode = IP_GATEWAY;
		strncpy(cfg->IP,Public.cGateWay,16);
		UdpServer->AddTask(UdpServer,ABinding->IP,ABinding->Port,buff,File->Size,
				3,0,NULL,NULL);
	}
}

/**
 * @brief UDPDownComparelist 下载房号对照表
 *
 * @param ABinding
 * @param AData
 */
static void udpDownComparelist(SocketHandle *ABinding,SocketPacket *AData)
{
	DBG_P("TP_COMPARELIST:DownComparelist()\n");
	DOWNDATA *Down = (DOWNDATA *)&AData->Data[sizeof(COMMUNICATION)];
	strncpy(DownData->cIP,ABinding->IP,16);
	DownData->Count = Down->Count;
	DownData->Port = UDPSERVER_PORT;
	DownData->ProcData(DownData,Down,TYPE_COMPARELIST);
}

/**
 * @brief udpDownUsercard 下载用户卡号
 *
 * @param ABinding
 * @param AData
 */
static void udpDownUsercard(SocketHandle *ABinding,SocketPacket *AData)
{
	DBG_P("TP_USERCARD:DownUsercard()\n");
	DOWNDATA *IDCard = (DOWNDATA *)&AData->Data[sizeof(COMMUNICATION)];
	strncpy(DownData->cIP,ABinding->IP,16);
	DownData->Count = IDCard->Count;
	DownData->Port = UDPSERVER_PORT;
	DownData->ProcData(DownData,IDCard,TYPE_IDCARD);
}

/**
 * @brief UDPSetSysVolume 设置系统音量
 *
 * @param ABinding
 * @param AData
 */
static void udpSetSysVolume(SocketHandle *ABinding,SocketPacket *AData)
{
	DBG_P("TP_SETSYSTEMAUDIO:SetSysVolume()\n");
	unsigned short vol = 0;
	TSetVolumePack *pack = (TSetVolumePack *)AData->Data;
	unsigned char *data = (unsigned char *)pack->Data;
	DBG_P("CenterSetVolume: %d\n",data[0]);
	if(data[0] == 2) {
		vol = data[1] * 256 + data[2];
		char buf[32];
		Public.TalkVolume = vol  * 100 / 2000;
		Mixer->SetVolumeEx(Mixer,Public.TalkVolume);
		sprintf(buf, "%02d",Public.TalkVolume);
		SetValueToEtcFile_Ext("interface.ini","PUBLIC","TalkVolume",buf);
	}
}

/**
 * @brief UDPResetPro 退出|重启
 *
 * @param ABinding
 * @param AData
 */
static void udpResetPro(SocketHandle *ABinding,SocketPacket *AData)
{
	DBG_P("TP_DEVCTRLCOM:ResetPro():%s\n",ABinding->IP);
	TDevCtrlCommand *pDevCtrl = (TDevCtrlCommand*)&AData->Data;
	if(pDevCtrl->Command != DEVCTRL_RESET) {
		return ;
	}
	printf("ReStart.........\n");
	TDevCtrlCommand ReInif;

	ReInif.ID = PacketID++;
	ReInif.Type = TP_DEVCTRLCOM;
	ReInif.Size = sizeof(TDevCtrlCommand);
	ReInif.DevType = TYPE_DOOR; // 户门口机
	ReInif.Command = DEVCTRL_RESET;
	memset(ReInif.Addition, 0, sizeof(ReInif.Addition));
	UdpServer->SendBuffer(UdpServer,ABinding->IP,UDPSERVER_PORT,&ReInif,sizeof(TDevCtrlCommand));
	sleep(1);
	exit(0);
}

/* ----------------------------------------------------------------*/
/**
 * @brief UDPLocalDevIDProc 返回设备唯一ID值
 *
 * @param ABinding
 * @param AData
 */
/* ----------------------------------------------------------------*/
static void udpLocalDevIDProc(SocketHandle *ABinding,SocketPacket *AData)
{
	DBG_P("TP_LOCALDEVID:LocalDevIDProc()\n");
	if(AData->Size != sizeof(TResDeviceInf)) {
		return;
	}
	TResDeviceInf *GetPacket = (TResDeviceInf*)AData->Data;
	if(GetPacket->Cmd == RESPONSESERVERINF && GetPacket->ReqType==TYPE_DEVID_SRV) {
		Public.dwDevIDTemp = GetPacket->DevID; //Public.dwDevID;
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief udpElevatorProc 电梯协议处理
 *
 * @param ABinding
 * @param AData
 */
/* ----------------------------------------------------------------*/
static void udpElevatorProc(SocketHandle *ABinding,SocketPacket *AData)
{
	DBG_P("TP_ELEVATOR:IP:%s\n",ABinding->IP);
	if(strcmp(Public.cLocalIP, ABinding->IP) == 0) {
		return;
	}

	if ((video->getStatus(video) != ST_IDLE) && (strcmp(video->cPeerIP, ABinding->IP) == 0)) {
		return;
	}
	char SqlStr[512];
	char Code[16]={0};
	// xb add 不用查找卡号进行开锁联动
	sprintf(SqlStr,"Select Code,IP From UserInfo Where IP='%s'",ABinding->IP);
	LocalQueryOpen(LocalQuery,SqlStr);
	int count = LocalQuery->RecordCount(LocalQuery);
	if(Public.eElevator && count) {
		TElevator lift;
		LocalQueryOfChar(LocalQuery,"Code",Code,12);
		Public.EleVatorFloor = atoi(Code);
		lift.AddrH = Public.ElevatorAddr%0x100;
		lift.AddrL = Public.EleVatorFloor/10000;
		lift.Function = 0x11;
		lift.CellCode = (char)(Public.ElevatorFloorCode&0xFF);
		lift.FloorCode = (char)(((Public.EleVatorFloor/100)%100)&0xFF);
		lift.Reserved = (char)(Public.ElevatorLobby);
		lift.RoomNumber = (char)((Public.EleVatorFloor%100)&0xFF);;
		Public.ElevatorSndData(&lift);
	}
	LocalQuery->Close(LocalQuery);
}

/* ----------------------------------------------------------------*/
/**
 * @brief udpOnline 检测管理中心在线协议
 *
 * @param ABinding
 * @param AData
 */
/* ----------------------------------------------------------------*/
static void udpOnline(SocketHandle *ABinding,SocketPacket *AData)
{
	// DBG_P("TP_ONLINE:%s\n",ABinding->IP);
	if(strcmp(Public.pCenterMsg->IP, ABinding->IP) == 0) {
		Public.CenterOnlineTime = 30;
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief udpRequestVideoData 视频数据请求协议
 *
 * @param ABinding
 * @param AData
 */
/* ----------------------------------------------------------------*/
static void udpRequestVideoData(SocketHandle *ABinding,SocketPacket *AData)
{
	VideoReqestData * Req = (VideoReqestData *)AData->Data;	
	printf("Now Device dwDevID : [%llu],Recv Cmd dwDevID : [%llu] \n",Public.dwDevID,Req->ReqDevID);
	if(Req->ReqDevID == Public.dwDevID){
		printf("Come to Trans Video Data  !!! \n");
		//确认当前请求为视频数据请求 !!!
		VideoTransNeedData NeedData;
		memset(&NeedData,0,sizeof(VideoTransNeedData));
		strcpy(NeedData.IP,Req->IP);
		NeedData.CmdPort = Req->CmdPort;
		NeedData.VideoPort = Req->VideoPort;
		NeedData.VideoType = 0;  // H264
		if(videoForGuest == NULL){
			//创建客户定制视频接口对象
			videoForGuest = videoTransCreateForGuest();
		}
		videoTransNewInterface(videoForGuest,&NeedData);		

	}
}
/* ----------------------------------------------------------------*/
/**
 * @brief udpRequestVideoData_Close 停止视频数据请求
 *
 * @param ABinding
 * @param AData
 */
/* ----------------------------------------------------------------*/
static void udpRequestVideoData_Close(SocketHandle *ABinding,SocketPacket *AData)
{
	VideoReqestData * Req = (VideoReqestData *)AData->Data;	
	printf("Now Device dwDevID : [%llu],Recv Cmd dwDevID : [%llu] \n",Public.dwDevID,Req->ReqDevID);
	if(Req->ReqDevID == Public.dwDevID){
		printf("Come to Close Video Data  !!! \n");
		//确认当前请求为停止视频数据请求 !!!
		VideoTransNeedData NeedData;
		strcpy(NeedData.IP,Req->IP);
		NeedData.CmdPort = Req->CmdPort;
		videoTransNewInterface_Close(videoForGuest,&NeedData);
	}
}
/* ----------------------------------------------------------------*/
/**
 * @brief udpSetDefenseAndLockByManagerCenter 通过管理中心设置门口机布防撤防开锁等功能
 *
 * @param ABinding
 * @param AData
 */
/* ----------------------------------------------------------------*/
static void udpSetDefenseAndLockByManagerCenter(SocketHandle *ABinding,SocketPacket *AData)
{
	TSetDefenseAndLock * Req = (TSetDefenseAndLock *)AData->Data;	
	if(Req->AckFlag == 0 && Req->ReadWrite == 0){	//收到管理中心的读出命令时返回数据
		printf("Get Defense and Lock State ...! \n");
		TSetDefenseAndLock ReplyInfo;
		ReplyInfo.ID = PacketID++;
		ReplyInfo.Type = TP_DOOROTHERPARA;
		ReplyInfo.Size = sizeof(TSetDefenseAndLock);
		ReplyInfo.AckFlag = 1;
		ReplyInfo.ReadWrite = 0;
		ReplyInfo.UseCardDisDef = Public.IsAllowDisarm;
		ReplyInfo.DisUnlockWhenMon =Public.IsAllowUnLock;
		memset(ReplyInfo.Reserve, 0 , sizeof(ReplyInfo.Reserve));
		
		//UDP发送数据
		UdpServer->AddTask(UdpServer,ABinding->IP,ABinding->Port,&ReplyInfo,sizeof(TSetDefenseAndLock),
				3,0,NULL,NULL);
		printf("Check Send Info : \n");
		printf("AckFlag--------------------> [%d] \n",ReplyInfo.AckFlag);
		printf("ReadWrite--------------------> [%d] \n",ReplyInfo.ReadWrite);
		printf("UseCardDisDef--------------------> [%d] \n",ReplyInfo.UseCardDisDef);
		printf("DisUnlockWhenMon--------------------> [%d] \n",ReplyInfo.DisUnlockWhenMon);
	}
	else if(Req->AckFlag == 0 && Req->ReadWrite == 1){	//收到管理中的写入命令时的操作
		printf("Check Recv Info : \n");
		printf("AckFlag--------------------> [%d] \n",Req->AckFlag);
		printf("ReadWrite--------------------> [%d] \n",Req->ReadWrite);
		printf("UseCardDisDef--------------------> [%d] \n",Req->UseCardDisDef);
		printf("DisUnlockWhenMon--------------------> [%d] \n",Req->DisUnlockWhenMon);
		printf("Change Defense and Lock State ...! \n");

		//向配置文件中保存监控开锁数据
		char buf[64] = {0};
		Public.IsAllowUnLock = Req->DisUnlockWhenMon;
		sprintf(buf,"%d",Req->DisUnlockWhenMon);
		SetValueToEtcFile_Ext("interface.ini","MODE","MonitorUnlock",buf);

		//向配置文件中保存刷卡撤防数据
		char buf2[64] = {0};
		Public.IsAllowDisarm = Req->UseCardDisDef;
		sprintf(buf2,"%d",Req->UseCardDisDef);
		SetValueToEtcFile_Ext("interface.ini","MODE","UseCardDisArm",buf2);
			
	}		
	
}

/* ----------------------------------------------------------------*/
/**
 * @brief 协议处理表
 */
/* ----------------------------------------------------------------*/
static UdpCmdRead udp_cmd_read_handle[] = {
	{TP_CALL,			udpCallProc},
	{TP_DEVCHECK,		udpLocalMsgGetProc},
	{TP_RETUPDATEMSG,	udpUpdateProc},
	{TP_LOCALDEVID,		udpLocalDevIDProc},
	{TP_SETSYSTIME,		udpSetSysTime},
	{TP_IPDOWN,			udpDownIPproc},
	{TP_READIP,			udpReadIPproc},
	{TP_COMPARELIST,	udpDownComparelist},
	{TP_USERCARD,		udpDownUsercard},
	{TP_SETSYSTEMAUDIO,	udpSetSysVolume},
	{TP_DEVCTRLCOM,		udpResetPro},
	{TP_ELEVATOR,		udpElevatorProc}, 
	{TP_ONLINE,			udpOnline},
	{TP_REQUESTVIDEO_OPEN,	udpRequestVideoData},
	{TP_REQUESTVIDEO_CLOSE,	udpRequestVideoData_Close},
	{TP_DOOROTHERPARA,udpSetDefenseAndLockByManagerCenter},
	
};

/**
 * @brief udpSocketRead UDP端口接收协议
 *
 * @param ABinding
 * @param AData
 */
void udpSocketRead(SocketHandle *ABinding,SocketPacket *AData)
{
	COMMUNICATION * head = (COMMUNICATION *)AData->Data;

	if(AData->Size < sizeof(COMMUNICATION))
		return;

	if(AData->Size != head->Size) {
		return;
	}

	// if(strcmp(Public.pCenterMsg->IP, ABinding->IP) == 0) {
		// DBG_P("TP_ONLINE:%s\n",ABinding->IP);
		// Public.CenterOnlineTime = 30;
	// }
	unsigned int i;
	for(i=0; i<NELEMENTS(udp_cmd_read_handle); i++) {
		if (udp_cmd_read_handle[i].cmd == head->Type) {
			udp_cmd_read_handle[i].udpCmdProc(ABinding,AData);
			return;
		}
	} 
	DBG_P("[%s]:IP:%s,Cmd=0x%04x\n",__FUNCTION__,ABinding->IP,head->Type);
}
