/*
 * =====================================================================================
 *
 *       Filename:  public.c
 *
 *    Description:  ��ʼ��ȫ�ֱ���
 *
 *        Version:  1.0
 *        Created:  2015-11-25 18:08:06
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
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <fcntl.h>

#include "externfunc.h"

/* ----------------------------------------------------------------*
 *                  extern variables declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                  internal functions declare
 *-----------------------------------------------------------------*/
static void publicSyncTime(void);

/* ----------------------------------------------------------------*
 *                        macro define
 *-----------------------------------------------------------------*/
#if DBG_PUBLIC > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

#if (defined TC3000_X6)
#define Ver_Major	1
#define Ver_Minor	0
#define Ver_Release	0
#define Ver_Reserve	0
#endif

#if (defined TC3000_DE)
#define Ver_Major	1
#define Ver_Minor	1
#define Ver_Release	0
#define Ver_Reserve	0
#endif

#if (defined TC3000_18S)
#define Ver_Major	1
#define Ver_Minor	0
#define Ver_Release	0
#define Ver_Reserve	0
#endif
/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
PUBLIC_DATA Public;
SCREEN_FORM Screen;
TUDPClient *udpclient;
static int center_cnt = 0;
static int _first_update = 0;

unsigned int PacketID;
static char center_ip[16];
static char imei[32];
static char language_type[32];

static EtcValueInt etc_value_int[]={
{"PUBLIC",	"LCDLight",			&Public.LCDLight,			90},
{"PUBLIC",	"Kerber_Major",		&Public.KernelVer.Major,	1},
{"PUBLIC",	"Kerber_Minor",		&Public.KernelVer.Minor,	0},
{"PUBLIC",	"Kerber_Release",	&Public.KernelVer.Release,	0},
{"PUBLIC",	"Kerber_Reserve",	&Public.KernelVer.Reserve,	0},
{"PUBLIC",	"MaxScrSaversTimer",&Public.MaxScrSaversTimer,	60},
{"PUBLIC",	"TalkVolume",		&Public.TalkVolume,			80},
{"PUBLIC",	"CurrentVolume",	&Public.CurrentVolume,		80},
{"PUBLIC",	"FramPeriod",		&Public.FramPeriod,			10},
{"PUBLIC",	"AlarmDoorTime",	&Public.AlarmDoorTime,		10},
{"PUBLIC",	"VideoType",		&Public.VideoType,			VEDIO_640x480},
{"PUBLIC",	"ElevatorAddr",		&Public.ElevatorAddr,		0},
{"PUBLIC",	"ElevatorFloorCode",&Public.ElevatorFloorCode,	0},
{"PUBLIC",	"ElevatorLobby",	&Public.ElevatorLobby,		0},
{"PUBLIC",	"IsDownLoadUseInfo",&Public.IsDownLoadUseInfo,	0},	
{"PUBLIC",	"Build_Num",		&Public.Build_Num,			1}, 
{"PUBLIC",	"Unit_Num",			&Public.Unit_Num,			0},
{"MODE",	"EnableElevator",	&Public.eElevator,			0},
{"MODE",	"EnableDoorPassword",&Public.EnableDoorPassword,0},
{"MODE",	"EnableAlarmDoor",	&Public.EnableAlarmDoor,	0},
{"MODE",	"SyncTime",			&Public.SyncTime,			0},
{"MODE",	"Broken",			&Public.bUnbrokenUse,		0},
{"MODE",	"MonitorUnlock",	&Public.IsAllowUnLock,		0},
{"MODE",	"UseCardDisArm",	&Public.IsAllowDisarm,		0},



};

static EtcValueChar etc_value_char[]={
{"PUBLIC",	"LocalID",			Public.cLocalID,		sizeof(Public.cLocalID),""},
{"PUBLIC",	"MasterDeviceIP",	Public.MasterDeviceIP,	sizeof(Public.MasterDeviceIP),""},
{"PUBLIC",	"ViceDeviceIP",		Public.ViceDeviceIP,	sizeof(Public.ViceDeviceIP),""},
{"PUBLIC",	"IMEI",				imei,					sizeof(imei),""},
{"PUBLIC",	"Language",			language_type,			sizeof(language_type),"����"},
{"System",	"RoomID",			Public.cRoomID,			sizeof(Public.cRoomID),"0000000"},
{"System",	"RoomName",			Public.cRoomName,		sizeof(Public.cRoomName),""},
{"SECURE",	"UnlockPassword",	Public.UnlockPassword,	sizeof(Public.UnlockPassword),""},
{"SECURE",	"SystemPassword",	Public.SystemPassword,	sizeof(Public.SystemPassword),""},
{"Network",	"Gateway",			Public.cGateWay,		sizeof(Public.cGateWay),"172.16.1.1"},
{"Network",	"CenterIP",			center_ip,				sizeof(center_ip),"172.16.100.1"},	
{"Network", "DefaultDmkjIp",	Public.DefaultDmkjIp,	sizeof(Public.DefaultDmkjIp),"172.16.254.100"},

};

//--------------------------------------------------------------------------
static BOOL screenAddForm(HWND hWnd,const char *Class)
{
	PFormClass Form = (PFormClass)malloc(sizeof(FormClass));
	memset(Form,0,sizeof(FormClass));
	Form->hWnd = hWnd;
	strncpy(Form->Class,Class,15);
	if(Screen.head == NULL) {
		Screen.head = Screen.tail = Form;
	} else {
		Screen.tail->next = Form;
		Screen.tail = Form;
	}
	Screen.Count++;
//	DBG_P("Form(%s)Add\n",Class);
	if(Public.IsDrangTest){
		fprintf(Screen.DrangLogfp,"%s\n",Class);
		fflush(Screen.DrangLogfp);
	}
	return TRUE;
}
//--------------------------------------------------------------------------
static BOOL screenDelForm(HWND hWnd)
{
	PFormClass form,parentform;
	form = Screen.head;
	while(form)	{
		if(form->hWnd != hWnd) {
			parentform = form;
			form = form->next;
			continue;
		}
		// DBG_P("Form(%s)Del\n",form->Class);
		if(Screen.head == form) {
			Screen.head = form->next;
			if(Screen.tail == form) Screen.tail = NULL;
			if(Public.IsDrangTest){
				fprintf(Screen.DrangLogfp,"%s exit\n",form->Class);
				fflush(Screen.DrangLogfp);
			}
		} else {
			parentform->next = form->next;
			if(Screen.tail == form) Screen.tail = parentform;
			if(Public.IsDrangTest){
				fprintf(Screen.DrangLogfp,"%s exit\n",form->Class);
				fflush(Screen.DrangLogfp);
			}
		}
		free(form);
		Screen.Count--;
		return TRUE;
	}
	return FALSE;
}
//--------------------------------------------------------------------------
static void screenReturnMainForm(void)
{
	int FormCnt=0;
	HWND *Forms;
	PFormClass form;
	form = Screen.head;
	Forms = (HWND *)calloc(sizeof(HWND),Screen.Count);
	while(form && FormCnt<Screen.Count) {
		//���������ڡ����жԽ����ں�״̬����
		if(strcmp(form->Class,"TFrmMain"))
			Forms[FormCnt++] = form->hWnd;
		form = form->next;
	}
	while(FormCnt) {
		SendNotifyMessage(Forms[FormCnt-1],MSG_CLOSE,0,-1);
		FormCnt--;
	}
	free(Forms);
	//�ж�SQL SERVER������
//	Query->DisConnect(Query,TRUE);
//	TempQuery->DisConnect(TempQuery,TRUE);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief updateTimeCallBackFunc ͬ����������ʱ�䳬ʱ����
 *
 * @param Ret
 * @param CallBackData
 */
/* ---------------------------------------------------------------------------*/
static void updateTimeCallBackFunc(int Ret,void *CallBackData)
{
	if(Ret == MSG_SENDTIMEOUT) {
		if (center_cnt < (Public.CenterCnt - 1)) {
			center_cnt++;
			publicSyncTime();
		} else {
			center_cnt = 0;
		}
	} else if (Ret == MSG_SENDSUCCESS) {
		_first_update = 1;
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief publicSyncTime ����Ϣ����������ͬ��ʱ��
 *
 */
/* ---------------------------------------------------------------------------*/
static void publicSyncTime(void)
{
	char tmpbuf[64];

	CFGDEVICE *pDateTIme = (CFGDEVICE*)tmpbuf;
	memset(tmpbuf, 0, sizeof(tmpbuf));

	pDateTIme->ID = PacketID++;
	pDateTIme->Size = sizeof(CFGDEVICE);
	pDateTIme->Type = TP_SETSYSTIME;
	pDateTIme->Cmd = 0;
	pDateTIme->Count = 6;
	pDateTIme->Data[0] = 0;
	if(Public.pCenterMsg) {
		if(Public.pCenterMsg[center_cnt].IP[0] != 0) {
			UdpServer->AddTask(UdpServer,
					Public.pCenterMsg[center_cnt].IP, 7800,
					pDateTIme, pDateTIme->Size,
					1, 0,
					updateTimeCallBackFunc, NULL);
		}
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief publicSyncTimeHandle1s ��û��ͬ������������ʱ��
 * ÿ60sͬ��һ�ι�������
 */
/* ---------------------------------------------------------------------------*/
static void publicSyncTimeHandle60s(void)
{
	if (_first_update) {
		return ;
	}
	publicSyncTime();
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief publicReSyncTime ����ͬ����������ʱ��
 */
/* ---------------------------------------------------------------------------*/
static void publicReSyncTime(void)
{
	_first_update = 0;
} 

//--------------------------------------------------------------------------
static HWND screenFindForm(const char *Class)
{
	PFormClass form = Screen.head;
	if(Class==NULL)
		return 0;

	while(form) {
		if(strncmp(Class,form->Class,15)==0)
			return form->hWnd;
		form = form->next;
	}
	return 0;
}

/**
 * @brief unlockCallback �����꿪�����ֵĻص����������ڻָ�ͨ��
 */
static void unlockCallback(void)
{
	if (video->getStatus(video) == ST_TALK) {
		Mixer->InitPlayAndRec(Mixer,video->getFpAudio(video));
	} else {
		if (PlayMp3->GetStatus(PlayMp3) == MP3_PAUSE) {
			PlayMp3->Resume(PlayMp3);
		}
	}
	video->setSilence(video,FALSE);
}

/* ----------------------------------------------------------------*/
/**
 * @brief PlayVoiceWithBack ���ص������Ĳ�������
 *
 * @param text ������������
 * @param Proc ������ϻص�����
 */
/* ----------------------------------------------------------------*/
static void PlayVoiceWithBack(char *text,void (*Proc)(void))
{
	char file[64];
	// ������ʱ����������
	if (Public.language != CHINEASE || Public.language != TAIWAN) {
		if (Proc != NULL) {
			Proc();
		}
		return;
	}

	sprintf(file,"res/wav/%s.wav",text);
	// printf("[%s]%s\n", __FUNCTION__,file);
	playwavfile(file,Proc);
}

/* ----------------------------------------------------------------*/
/**
 * @brief PlayVoice ���ص������Ĳ�������
 *
 * @param text ������������
 */
/* ----------------------------------------------------------------*/
static void PlayVoice(char *text)
{
	PlayVoiceWithBack(text,NULL);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief UnlockTheDoor ���տ�������� 500ms����ʱ��
 *
 * @param mode ������ʽ  0ˢ��������
 * 					 	1�����ڻ�������
 * 						2�������Ŀ�����
 * 						3���뿪����
 */
/* ---------------------------------------------------------------------------*/
static void UnlockTheDoor(int mode)
{
	// printf("[%s]status:%d\n", __FUNCTION__,video->getSilence(video));
	// ��ֹ���������������Ȳ��Ž������ִ����һ��,����ˢ��������
	if ((video->getSilence(video) == TRUE)){
		return;
	}
	video->setSilence(video,TRUE);

	Public.UnlockMode = mode;
	if (*video->getFpAudio(video) != 0) {
		Mixer->DeInitPlay(Mixer,video->getFpAudio(video));
	}
	if (Public.language != CHINEASE || Public.language != TAIWAN) {
		if (PlayMp3->GetStatus(PlayMp3) == MP3_PLAY) {
			PlayMp3->Pause(PlayMp3);
		}
		PlayUnlock->Play(PlayUnlock,"res/wav/didong.mp3",0,unlockCallback);
	} else {
		PlayVoiceWithBack("UnlockedPleaseComeIn",unlockCallback);
	}
	Mygpio->SetValueNow(Mygpio,ENUM_GPIO_UNLOCK,IO_ACTIVE);
	form_main->timerStart(IDC_TIMER_UNLOCK);
}

/* ----------------------------------------------------------------*/
/**
 * @brief LrcCheck �������Э��У����
 *
 * @param pElevator
 */
/* ----------------------------------------------------------------*/
static void LrcCheck(TElevator *pElevator)
{
	unsigned char check = 0;
	check += pElevator->AddrH;					//��ַ
	check += pElevator->AddrL;						//��Ԫ��
	check += pElevator->Function;					//������
	check += pElevator->CellCode;					//��ʼ¥��
	check += pElevator->FloorCode;					//����¥��
	check += pElevator->Reserved;					//Ԥ��
	check += pElevator->RoomNumber;					//����
	pElevator->Lrc = ~check+1;
}

/* ----------------------------------------------------------------*/
/**
 * @brief FormatSendData ��ʽ�����͵���Э������
 *
 * @param buf  ������������
 * @param pElevator �������ݽṹ
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
static int FormatSendData(unsigned char *buf,TElevator *pElevator)
{
	unsigned char * p = (unsigned char *) pElevator;
	int i=0,len,tmp;
	unsigned char temp;
	len = sizeof(TElevator)-3;
	buf[i] = *p++;
	for(i=0;i<len;i++) {
		tmp = i*2;
		temp = *p++;
		buf[tmp+1]=	temp>>4;
		buf[tmp+2] = temp&0x0F;
		if(buf[tmp+1]>9)
			buf[tmp+1] += 0x37;
		else
			buf[tmp+1] += 0x30;
		if(buf[tmp+2]>9)
			buf[tmp+2] += 0x37;
		else
			buf[tmp+2] += 0x30;
	}
	tmp = i*2;
	buf[tmp+1] = *p++;
	buf[tmp+2] = *p++;
	return 0;
}

/* ----------------------------------------------------------------*/
/**
 * @brief ElevatorSndData ��Ƭ�����͵���Э��
 *
 * @param This
 * @param pElevator ���ݽṹ��
 */
/* ----------------------------------------------------------------*/
static void ElevatorSndData(TElevator *pElevator)
{
	pElevator->ID = PacketID++;
	pElevator->Size = sizeof(TElevator);
	pElevator->Type = TP_ELEVATOR_3000; //TP_ELEVATOR; 3000ϵͳ����һ�ֶΣ�����㲥����������ڻ��յ�����
	LrcCheck(pElevator);
	pElevator->head = 0x3A;
	pElevator->Cr   = 0x0D;
	pElevator->Lf   = 0x0A;
	UdpServer->AddTask(UdpServer,
			"255.255.255.255",
			UDPSERVER_PORT,
			pElevator,
			sizeof(TElevator),
			3,0,NULL,NULL);
	// printf("[%s]len:%d,[id]:0x%x [size]:%d [CMD]:0x%x [head]:0x%x \n\
			// [addrh]:0x%x [addrl]:0x%x [fun]:%d [cell]:%d [floor]:%d \n\
			// [Room]:%d, [lrc]:0x%x,[cr]:0x%x,[lf]:0x%x\n",
			// __FUNCTION__,
			// sizeof(TElevator),
			// pElevator->ID,
			// pElevator->Size,
			// pElevator->Type,
			// pElevator->head ,
			// pElevator->AddrH ,
			// pElevator->AddrL ,
			// pElevator->Function ,
			// pElevator->CellCode ,
			// pElevator->FloorCode ,
			// pElevator->RoomNumber ,
			// pElevator->Lrc ,
			// pElevator->Cr ,
			// pElevator->Lf
			// );
	// This->uart->Send(This->uart,This->SndData,Len);
}

/* ----------------------------------------------------------------*/
/**
 * @brief SendUdpData ����Э�飬���ط�
 *
 * @param IP
 * @param Port
 * @param Type
 * @param Data
 * @param DataSize
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
static int SendUdpData(const char *IP,
		int Port,
		int Type,
		char *Data,
		unsigned int DataSize)
{
	int Success;
	char *pBuf = (char*)malloc(sizeof(COMMUNICATION)+DataSize);
	COMMUNICATION * pComm = (COMMUNICATION *)pBuf;
	pComm->ID = PacketID++;
	pComm->Size = sizeof(COMMUNICATION)+DataSize;
	pComm->Type = Type;
	if(DataSize)
		memcpy(&pBuf[sizeof(COMMUNICATION)],Data,DataSize);
	Success = udpclient->SendBuffer(udpclient,IP,Port,pBuf,pComm->Size);
	free(pBuf);
	return Success;
}

/* ----------------------------------------------------------------*/
/**
 * @brief SendUdpTData ����Э�飬���ط�����
 *
 * @param IP
 * @param Port
 * @param Type
 * @param Data
 * @param DataSize
 * @param Func  �ص�����
 * @param CallBackData ����
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
static int SendUdpTData(const char *IP,
		int Port,
		int Type,
		char *Data,
		unsigned int DataSize,
		CallBackUDP Func,
		void *CallBackData)
{
	char *pBuf = (char*)calloc(1,sizeof(COMMUNICATION)+DataSize);
	COMMUNICATION * pComm = (COMMUNICATION *)pBuf;
	pComm->ID = PacketID++;
	pComm->Size = sizeof(COMMUNICATION)+DataSize;
	pComm->Type = Type;
	if(DataSize)
		memcpy(&pBuf[sizeof(COMMUNICATION)],Data,DataSize);
	UdpServer->AddTask(UdpServer,IP,Port,pBuf,pComm->Size,3,0,Func,CallBackData);
	free(pBuf);
	return TRUE;
}

/* ----------------------------------------------------------------*/
/**
 * @brief LoadHomeList x6�����õ�����ÿ��������ӦIP
 */
/* ----------------------------------------------------------------*/
static void LoadHomeList(void)
{
	if(LocalQuery == 0) {
		return;
	}
	// �����ڻ�
	memset(Public.HomeList,0,sizeof(Public.HomeList));
	Public.HomeListCnt = MAX_HOMELIST_NUM;
	LocalQueryOpen(LocalQuery,"select Code,IP From UserInfo ORDER BY Code");
	int num = LocalQuery->RecordCount(LocalQuery);
	if(num ){
		int i;
		for(i=0; i<Public.HomeListCnt; i++) {
			LocalQueryOfChar(LocalQuery,"IP",Public.HomeList[i].IP,sizeof(Public.HomeList[i].IP));
			LocalQuery->Next(LocalQuery);
			printf("[%d],IP:%s\n",i,Public.HomeList[i].IP);
		}
	} else {
		Public.CenterCnt = 0;
	}
	LocalQuery->Close(LocalQuery);
}

/* ----------------------------------------------------------------*/
/**
 * @brief SaveIMEIandSetMac �����豸�Ų�����IP��ַ��MAC��ַ
 */
/* ----------------------------------------------------------------*/
static void SaveIMEIandSetMac(void)
{
	char buf[64];
	unsigned char *p = (unsigned char*)&Public.dwDevID;
	sprintf(buf,"%llX",Public.dwDevID);
	SetValueToEtcFile("interface.ini","Network","Gateway",Public.cGateWay);
	SetValueToEtcFile_Ext("interface.ini","PUBLIC","IMEI",buf);
	SetNetMac(p,Public.MAC);
	SetNetwork(1,Public.cLocalIP,Public.cNetMask,Public.cGateWay,Public.MAC);
}

/* ----------------------------------------------------------------*/
/**
 * @brief GetDeviceIMEI ���ͻ�ȡ�豸���Э��
 */
/* ----------------------------------------------------------------*/
static void GetDeviceIMEI(void)
{
	Public.dwDevIDTemp = Public.dwDevID;
	Public.bGetDevID = TRUE;
	TGetDeviceInfo Packet;
	memset(&Packet,0,sizeof(TGetDeviceInfo));
	Packet.ID = PacketID++;
	Packet.Size = sizeof(TGetDeviceInfo);
	Packet.Type = TP_DEVCHECK;
	Packet.Cmd = CMD_GETSTATUS;
	Packet.DevType = TYPE_DEVID_SRV;
	UdpServer->AddTask(UdpServer,
		   	GetSendIP(Public.cLocalIP, "255.255.255.255", Public.cNetMask),
			7801, &Packet, Packet.Size, 5, 0,NULL,NULL);
}

/* ----------------------------------------------------------------*/
/**
 * @brief UpdateCenterIP �������ݿ⵽��������
 *
 * @param db ���ݿ��ļ�����
 * @param ip ��������IP
 * @param ip_len IP����
 */
/* ----------------------------------------------------------------*/
static void UpdateCenterIP(char *db,char *ip,int ip_len)
{
	char SqlStr[512];
	sprintf(SqlStr,"Update %s Set IP=\"%s\" Where Code=\"0000\"",db,ip);
	LocalQueryExec(LocalQuery,SqlStr);
	strncpy(Public.pCenterMsg->IP,ip,ip_len);
	SetValueToEtcFile_Ext("interface.ini","Network","CenterIP",ip);
	if (sqlCheck(&LocalQuery,"DataBase.db",1) == TRUE) {//���ݿ�
		backData("DataBase.db");
	}
}


/* ----------------------------------------------------------------*/
/**
 * @brief SendDoorOpenWarn ���Ϳ��ű���״̬����������
 *
 * @param WarnLevel =0������  =1����
 * @param LogType :LOG_DR_DOOR, LOG_UR_ALARM, LOG_DR_LOWPOW
 * @param STime ���ɹ�ʱ���ʹ���
 * @param Msg  ������Ϣ����
 * @param MsgSize ������Ϣ����
 */
/* ----------------------------------------------------------------*/
static void SendDoorOpenWarn(int WarnLevel, int LogType, int STime, char *Msg, int MsgSize)
{
	TLogMessage *pSendInfo = (TLogMessage *)malloc(sizeof(TLogMessage)) ;
	if(pSendInfo == 0) {
		return;
	}
	memset(pSendInfo, 0, sizeof(TLogMessage));
	pSendInfo->ID = PacketID++;
	pSendInfo->Size = sizeof(TLogMessage);
	pSendInfo->Type = TP_LOGMSG;
	pSendInfo->WarnLevel = WarnLevel;
	pSendInfo->DevType = TYPE_DOOR;
	pSendInfo->LogType = LogType;
	memcpy(pSendInfo->LogMessage, Msg, MsgSize);
	UdpServer->AddTask(UdpServer, Public.pCenterMsg->IP, UDPSERVER_PORT,
			(char*)pSendInfo, sizeof(TLogMessage),
			STime,
			0,
			NULL,NULL);
	free(pSendInfo);
	pSendInfo = NULL;
	//printf("DOOR OPEN! DOOR OPEN! DOOR OPEN!\n");
}

/**
 * @brief RegisteToMng ÿ���ӵǼ�һ��������Ϣ
 */
static void RegisteToMng(void)
{
	TRespDevInfo2 Packet;
	memset(&Packet,0,sizeof(TRespDevInfo2));
	Packet.ID = 0;
	Packet.Size = sizeof(TRespDevInfo2);
	Packet.Type = TP_WORKREG2;
	Packet.Proctol = 1;
	Packet.DevType = TYPE_DOOR;
	Packet.dwLoDevID = (DWORD)(Public.dwDevID);
	Packet.dwHiDevID = (DWORD)(Public.dwDevID>>32);
	strncpy(Packet.Name,Public.cRoomName,sizeof(Packet.Name)-1);
	strncpy(Packet.MAC,Public.MAC,sizeof(Packet.MAC)-1);
	strncpy(Packet.CenterIP[0],Public.pCenterMsg->IP,sizeof(Public.pCenterMsg->IP)-1);

	Packet.ActiveTick = MyGetTickCount();
	CopyVerison(Packet.KerVer,Public.KernelVer);
	sprintf(Packet.AppVer,"%d.%d.%d",Public.SoftVer.Major,Public.SoftVer.Minor,Public.SoftVer.Release);

	GetDate(Packet.CurrTime,sizeof(Packet.CurrTime));
	Packet.UserDiskSize = GetFlashFreeSpaceKB();
	UdpServer->SendBuffer(UdpServer,Packet.CenterIP[0],7800,&Packet,Packet.Size);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief SendUnlockRec ���Ϳ������
 *
 * @param status ������� 0ʧ�� 1�ɹ�
 */
/* ---------------------------------------------------------------------------*/
static void SendUnlockRec(int status)
{
	char buf[200];
	TUnlockRec *unlock_rec = (TUnlockRec *)buf;
	unlock_rec->DevType = TYPE_DOOR;
	unlock_rec->UnlockMode = Public.UnlockMode;
	unlock_rec->UnlockStatus = status;
	strncpy(unlock_rec->DevName,Public.cRoomName,sizeof(unlock_rec->DevName)-1);
	SendUdpTData(Public.pCenterMsg->IP, UDPSERVER_PORT,
			TP_UNLOCK_INFO,
			buf, sizeof(buf),
			NULL, NULL);
}
/* ----------------------------------------------------------------*/
/**
 * @brief BroadCastFilter �ж�IP�Ƿ���ͬһ����
 *
 * @param SrcIP ԴIP
 * @param DstIP Ŀ��IP
 *
 * @returns 1ͬ���� 0��ͬ����
 */
/* ----------------------------------------------------------------*/
static int BroadCastFilter(const char *SrcIP,const char *DstIP)
{
	unsigned int NetType = 0;
	unsigned int sIP = 0,dIP = 0;
	unsigned int iSrcIP,iDstIP;
	iSrcIP = inet_addr(SrcIP);
	iDstIP = inet_addr(DstIP);
	NetType = iSrcIP & 0x000000FF;
	if(NetType < 128)	{	//A��IP
		sIP = iSrcIP & 0x000000FF;
		dIP = iDstIP & 0x000000FF;
	} else if(NetType < 192) {	//B��IP
		sIP = iSrcIP & 0x0000FFFF;
		dIP = iDstIP & 0x0000FFFF;
	} else if(NetType < 224) {	//C��IP
		sIP = iSrcIP & 0x00FFFFFF;
		dIP = iDstIP & 0x00FFFFFF;
	}

	if(sIP == dIP) {
		return 1;
	} else {
		return 0;
	}
}

static void CallBackCenter (int Ret,void *CallBackData)
{
	if(Ret == MSG_SENDSUCCESS) {							
		Public.fTransmitToCenter = 1;		
	}
}

static void SendCallInfoToCenter(void)
{
	TDoorCallMoniter CallInfo;

	memset(&CallInfo, 0, sizeof(TDoorCallMoniter));
	CallInfo.ID = PacketID++;
	CallInfo.Size = sizeof(TDoorCallMoniter);
	CallInfo.Type = TP_CALLUSER;
#if (defined TC3000_BZ)
	CallInfo.Version = 3000;
#else 
	CallInfo.Version = 0;
#endif
	CallInfo.CallType = TYPE_DOOR;
	CallInfo.IsFDoor = FALSE;
	CallInfo.VideoType = 0;
	CallInfo.VideoWidth = h264_type[Public.VideoType].img_width;
	CallInfo.VideoHeight = h264_type[Public.VideoType].img_height;
	strncpy(CallInfo.CALLIP, video->cPeerIP, sizeof(CallInfo.CALLIP)-1);	
	UdpServer->AddTask(UdpServer,
			Public.pCenterMsg->IP, UDPSERVER_PORT,
			&CallInfo,
			CallInfo.Size,3,0,
			CallBackCenter,NULL);
}
/* ----------------------------------------------------------------*/
/**
 * @brief publicInit ȫ�����ݳ�ʼ��
 */
/* ----------------------------------------------------------------*/
void publicInit(void)
{
    int hFile;
	//��ȡʱ����Ϊ��ʼ�����ͱ��
	PacketID = (unsigned)time(NULL);

	Public.bEnableScrSavers = TRUE;
	Public.ScrSaversTimer = 0;
	Public.bUnbroken = FALSE;
	Public.bUnbrokenUse = FALSE;
	Public.WatchDog_fd = -2;
	Public.VideoOverDlay = 0;

	Public.CallRecordView = 0;
	Public.CenterOnline = 0;
	Public.CenterOnlineTime = 0;
	Public.Single_Mutex = FALSE;
	Public.MainWndMode = 0;

	Public.UnlockTheDoor = UnlockTheDoor;
	Public.SendUdpData = SendUdpData;
	Public.SendUdpTData = SendUdpTData;
	Public.LoadHomeList = LoadHomeList;
	Public.SaveIMEIandSetMac = SaveIMEIandSetMac;
	Public.GetDeviceIMEI = GetDeviceIMEI;
	Public.UpdateCenterIP = UpdateCenterIP;
	Public.SendDoorOpenWarn = SendDoorOpenWarn;
	Public.RegisteToMng = RegisteToMng;
	Public.PlayVoiceWithBack = PlayVoiceWithBack;
	Public.PlayVoice = PlayVoice;
	Public.ElevatorSndData = ElevatorSndData;
	Public.BroadCastFilter = BroadCastFilter;
	Public.SendUnlockRec = SendUnlockRec;
	Public.SendCallInfoToCenter = SendCallInfoToCenter;
	Public.reSyncTime = publicReSyncTime;
	Public.syncTime = publicSyncTime;
	Public.syncTimeHandle60s = publicSyncTimeHandle60s;

	memset(&Screen,0,sizeof(SCREEN_FORM));
	Screen.Add = screenAddForm;
	Screen.Del = screenDelForm;
	Screen.Find = screenFindForm;
	Screen.ReturnMain = screenReturnMainForm;

	hFile = LoadEtcFile("interface.ini");

	if(hFile != ETC_FILENOTFOUND) {
		unsigned long i;
		char buf[32];

		Public.SoftVer.Major = Ver_Major;
		Public.SoftVer.Minor = Ver_Minor;
		Public.SoftVer.Release = Ver_Release;
		Public.SoftVer.Reserve = Ver_Reserve;

		sprintf(Public.BuilderDate,"%s", __DATE__);

		for (i=0; i<NELEMENTS(etc_value_int); i++) {
				if(GetIntValueFromEtc(hFile,
							etc_value_int[i].section,
							etc_value_int[i].key,
							etc_value_int[i].value)!=ETC_OK) {
					printf("Err get int etc sec:%s,key:%s\n",
							etc_value_int[i].section,etc_value_int[i].key );
					*etc_value_int[i].value = etc_value_int[i].default_int;
				}
		}
		for (i=0; i<NELEMENTS(etc_value_char); i++) {
				if(GetValueFromEtc(hFile,
							etc_value_char[i].section,
							etc_value_char[i].key,
							etc_value_char[i].value,
							etc_value_char[i].leng)!=ETC_OK) {
					printf("Err get char etc sec:%s,key:%s\n",
							etc_value_char[i].section,etc_value_char[i].key );
					strcpy(etc_value_char[i].value,etc_value_char[i].default_char);
				}
		}

		//��������IP
		Public.pCenterMsg = (TServerMsg*)malloc(sizeof(TServerMsg));
		Public.CenterCnt = 1;
		strcpy(Public.pCenterMsg->IP,center_ip);

		//���ļ���ȡ�豸����
		Public.dwDevID = (unsigned long long)strtoull(imei,NULL,16);

		//ѡ������ xb20150401
		if (strcmp(language_type,"����") == 0)
			Public.language = CHINEASE;
		else if (strcmp(language_type,"English") == 0)
			Public.language = ENGLISH;
		else if (strcmp(language_type,"Turkish") == 0)
			Public.language = TURKISH;
		else if (strcmp(language_type,"Taiwan") == 0)
			Public.language = TAIWAN;
		else if (strcmp(language_type,"Russian") == 0)
			Public.language = RUSSIAN;
		else if (strcmp(language_type,"Ukrainian") == 0)
			Public.language = UKRAINIAN;
#if (defined TC3000_X6)
		Public.LoadHomeList();
#endif
		Mixer->SetVolumeEx(Mixer,Public.CurrentVolume);

		UnloadEtcFile(hFile);
	} else {
		strcpy(Public.cRoomID,"0010101");
	}

	//ȡ����IP
	GetLocalIP(Public.cLocalIP,Public.cNetMask,Public.MAC);
	printf("LocalIP=%s, cNetMask=%s, MAC=%s\n",Public.cLocalIP, Public.cNetMask, Public.MAC);

	//���� UDP�ͻ���
	udpclient = TUDPClient_Create(0);

	Screen.Width = SCR_WIDTH;
	Screen.Height = SCR_HEIGHT;
	char *protol_type;
#if (defined TC3000_BZ)
	protol_type = "BZ";
#endif

#if (defined TC3000_WM)
	protol_type = "WM";
#endif

#if (defined TC3000_FL)
	protol_type = "FL";
#endif
	printf("[%s,%s]Version :V%d.%d.%d.%d\n",
			protol_type,DEVICE_MODE,Ver_Major,Ver_Minor,Ver_Release,Ver_Reserve);
}
//--------------------------------------------------------------------------
void publicDestroy(void)
{
    if(udpclient)
		udpclient->Destroy(udpclient);

	if(Public.pCenterMsg)
		free(Public.pCenterMsg);
	Public.pCenterMsg = NULL;
	Public.CenterCnt = 0;
}


