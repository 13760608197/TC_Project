/*
 * =============================================================================
 *
 *       Filename:  VideoTrans.c
 *
 *    Description:  ��Ƶ����Э�鼰����
 *
 *        Version:  1.0
 *        Created:  2015-11-28 16:33:50
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =============================================================================
 */
/* ---------------------------------------------------------------------------*
 *                      include head files
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <sys/timeb.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>

#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <errno.h>
#include <inttypes.h>

#include "externfunc.h"

/* ---------------------------------------------------------------------------*
 *                  extern variables declare
 *----------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*
 *                  internal functions declare
 *----------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*
 *                        macro define
 *----------------------------------------------------------------------------*/
#if DBG_VIDEO_TRANS > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

#define MSG_CALLRETCODE (MSG_USER+20)
#define MSG_SETCAPTION (MSG_USER+21)

#define TALK_TIME		180
#define RING_TIME		30

/* ---------------------------------------------------------------------------*
 *                      variables define
 *----------------------------------------------------------------------------*/
VideoTrans *video = NULL;
static STMData *data = NULL;
static STMData *center_data = NULL;
/* ---------------------------------------------------------------------------*/
/**
 * @brief ͨ��״̬��
 */
/* ---------------------------------------------------------------------------*/
static StateTable stm_video_state[] = {

#if (defined TC3000_BZ)
{EVENT_SHAKEHANDS,		ST_IDLE,		ST_SHAKEHANDS,		DO_SHAKEHANDS},

{EVENT_SHAKEHANDS_ASW,	ST_IDLE,		ST_SHAKEHANDS_ASW,	DO_SHAKEHANDS_ASW},
{EVENT_SHAKEHANDS_ASW,	ST_MONITOR,		ST_MONITOR,			DO_SHAKEHANDS_ASW},
{EVENT_SHAKEHANDS_ASW,	ST_DIAL,		ST_DIAL,			DO_SHAKEHANDS_ASW},
{EVENT_SHAKEHANDS_ASW,	ST_CALL,		ST_CALL,			DO_SHAKEHANDS_ASW},
{EVENT_SHAKEHANDS_ASW,	ST_TALK,		ST_TALK,			DO_SHAKEHANDS_ASW},

{EVENT_MONITOR,			ST_SHAKEHANDS_ASW,	ST_MONITOR,		DO_MONITOR},
{EVENT_DIAL,			ST_SHAKEHANDS,	ST_DIAL,			DO_DIAL},
{EVENT_TYPE_HM,			ST_JUDGE_TYPE,	ST_IDLE,			DO_NO},
#else
{EVENT_DIAL,			ST_IDLE,		ST_DIAL,			DO_DIAL},
{EVENT_TYPE_HM,			ST_JUDGE_TYPE,	ST_MONITOR,			DO_MONITOR},
#endif

{EVENT_TYPE_CENTER,		ST_JUDGE_TYPE,	ST_MONITOR,			DO_MONITOR},
{EVENT_MONITOR,			ST_IDLE,		ST_JUDGE_TYPE,		DO_JUDGE_TYPE},
{EVENT_MONITOR,			ST_DIAL,		ST_DIAL,			DO_RET_FAIL},
{EVENT_MONITOR,			ST_CALL,		ST_CALL,			DO_RET_FAIL},
{EVENT_MONITOR,			ST_TALK,		ST_TALK,			DO_RET_FAIL},
{EVENT_MONITOR,			ST_MONITOR,		ST_MONITOR,			DO_RET_FAIL},


{EVENT_CALL,			ST_DIAL,		ST_CALL,			DO_CALL},

{EVENT_TALK,			ST_CALL,		ST_TALK,			DO_TALK},
{EVENT_TALK,			ST_MONITOR,		ST_TALK,			DO_TALK},

{EVENT_FAIL_COMM,		ST_DIAL,		ST_IDLE,			DO_FAIL_COMM},
{EVENT_FAIL_COMM,		ST_SHAKEHANDS,	ST_IDLE,			DO_FAIL_COMM},

{EVENT_FAIL_BUSY,		ST_DIAL,		ST_IDLE,			DO_FAIL_BUSY},
{EVENT_FAIL_SHAKE,		ST_SHAKEHANDS,	ST_IDLE,			DO_FAIL_SHAKE},

{EVENT_FAIL_ABORT,		ST_CALL,		ST_IDLE,			DO_FAIL_ABORT},
{EVENT_FAIL_ABORT,		ST_TALK,		ST_IDLE,			DO_FAIL_ABORT},
{EVENT_FAIL_ABORT,		ST_MONITOR,		ST_IDLE,			DO_FAIL_ABORT},

{EVENT_HANDS_UP,		ST_SHAKEHANDS,	ST_IDLE,			DO_HANDS_UP},
{EVENT_HANDS_UP,		ST_SHAKEHANDS_ASW,	ST_IDLE,		DO_HANDS_UP},
{EVENT_HANDS_UP,		ST_MONITOR,		ST_IDLE,			DO_HANDS_UP},
{EVENT_HANDS_UP,		ST_DIAL,		ST_IDLE,			DO_HANDS_UP},
{EVENT_HANDS_UP,		ST_CALL,		ST_IDLE,			DO_HANDS_UP},
{EVENT_HANDS_UP,		ST_TALK,		ST_IDLE,			DO_HANDS_UP},
};

/* ---------------------------------------------------------------------------*/
/**
 * @brief callbackVideo ���лص����������г�ʱʱ����
 *
 * @param Ret MSG_SENDTIMEOUT ���г�ʱ��Ϣ
 * @param CallBackData
 */
/* ---------------------------------------------------------------------------*/
static void callbackVideo(int Ret,void *CallBackData)
{
	if(Ret != MSG_SENDTIMEOUT) {
		return ;
	}
	VideoTrans * this = (VideoTrans *)CallBackData;
	data = (STMData *)this->ST_machine->initPara(this->ST_machine,
			sizeof(STMData));
	data->flag = 0;
	this->ST_machine->MsgPost(this->ST_machine,EVENT_FAIL_COMM,data);
	printf("Call Time out, Failed!\n");
	
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief callbackRtp ������Ƶʱ�жϳ�ʱ�ص�����
 */
/* ---------------------------------------------------------------------------*/
static void callbackRtp(void *CallBackData)
{
	VideoTrans * this = (VideoTrans *)CallBackData;
	data = (STMData *)this->ST_machine->initPara(this->ST_machine,
			sizeof(STMData));
	data->flag = 3;
	this->ST_machine->MsgPost(this->ST_machine,EVENT_FAIL_ABORT,data);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransSendCallCmd ����������������
 *
 * @param IP �Է�IP
 * @param Port �Է��˿�
 * @param Cmd ����ָ��
 * @param bCallBack ���н���Ƿ��лص�����
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int videoTransSendCallCmd(VideoTrans * this,
		const char *IP, int Port,
		int Cmd,
		BOOL bCallBack)
{
	char cBuf[sizeof(COMMUNICATION)+sizeof(COMMUNICATION_CALL)];
	COMMUNICATION * pComm = (COMMUNICATION *)cBuf;
	COMMUNICATION_CALL *pCall = (COMMUNICATION_CALL *)&cBuf[sizeof(COMMUNICATION)];
	pComm->ID = PacketID++;
	pComm->Size = sizeof(COMMUNICATION)+sizeof(COMMUNICATION_CALL);
	pComm->Type = TP_CALL;
	pCall->Cmd = Cmd;
	strncpy(pCall->CallID,Public.cLocalID,sizeof(pCall->CallID)-1);
	strncpy(pCall->CallName,Public.cRoomName,sizeof(pCall->CallName)-1);
	pCall->CallType = TYPE_DOOR;//TYPE_FDOOR;
	pCall->VideoType = 0;
	pCall->VideoWidth = h264_type[Public.VideoType].img_width;
	pCall->VideoHeight = h264_type[Public.VideoType].img_height;
	if(bCallBack)
		UdpServer->AddTask(UdpServer,
				IP,Port,cBuf,pComm->Size,4,0,callbackVideo,this);
	else
		UdpServer->AddTask(UdpServer,
				IP,Port,cBuf,pComm->Size,4,0,NULL,NULL);
	return 1;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransSendCallRevert ���ͶԷ���������Ļ�Ӧ
 *
 * @param IP �Է�IP
 * @param Port �Է��˿�
 * @param Cmd ��Ӧ������
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int videoTransSendCallRevert(const char *IP,int Port,int Cmd)
{
	char cBuf[sizeof(COMMUNICATION)+sizeof(COMMUNICATION_CALL)+4];
	COMMUNICATION * pComm = (COMMUNICATION *)cBuf;
	COMMUNICATION_CALL *pRevert = (COMMUNICATION_CALL *)&cBuf[sizeof(COMMUNICATION)];
	memset(cBuf,0,sizeof(COMMUNICATION)+sizeof(COMMUNICATION_CALL)+4);
	pComm->ID = PacketID++;
	pComm->Size = sizeof(COMMUNICATION)+sizeof(COMMUNICATION_CALL);
	pComm->Type = TP_CALL;
	pRevert->Cmd = Cmd;
	pRevert->CallType = TYPE_DOOR;
	pRevert->VideoType = 0;
	pRevert->VideoWidth = h264_type[Public.VideoType].img_width;
	pRevert->VideoHeight = h264_type[Public.VideoType].img_height;
	strncpy(pRevert->CallID,Public.cRoomID,sizeof(pRevert->CallID)-1);
	strncpy(pRevert->CallName,Public.cRoomName,sizeof(pRevert->CallName)-1);

	UdpServer->AddTask(UdpServer,IP,Port,cBuf,pComm->Size,4,0,NULL,NULL);

	return 0;
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransSendUnLockResult ���ͶԷ����������Ӧ��
 *
 * @param IP �Է�IP
 * @param Port �Է��˿�
 * @param Cmd ��Ӧ������
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int videoTransSendUnLockResult(const char *IP,int Port,int UnLockRet)
{
	char cBuf[sizeof(COMMUNICATION)+sizeof(COMMUNICATION_CALL)+4];
	COMMUNICATION * pComm = (COMMUNICATION *)cBuf;
	COMMUNICATION_CALL *pRevert = (COMMUNICATION_CALL *)&cBuf[sizeof(COMMUNICATION)];
	memset(cBuf,0,sizeof(COMMUNICATION)+sizeof(COMMUNICATION_CALL)+4);
	pComm->ID = PacketID++;
	pComm->Size = sizeof(COMMUNICATION)+sizeof(COMMUNICATION_CALL);
	pComm->Type = TP_CALL;
	pRevert->Cmd = CMD_UNLOCK;
	pRevert->CallType = UnLockRet;
	pRevert->VideoType = 0;
	pRevert->VideoWidth = h264_type[Public.VideoType].img_width;
	pRevert->VideoHeight = h264_type[Public.VideoType].img_height;
	strncpy(pRevert->CallID,Public.cRoomID,sizeof(pRevert->CallID)-1);
	strncpy(pRevert->CallName,Public.cRoomName,sizeof(pRevert->CallName)-1);

	UdpServer->AddTask(UdpServer,IP,Port,cBuf,pComm->Size,4,0,NULL,NULL);

	return 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransIsCenterIP �����Ƿ��������,���ع�������������
 *
 * @param IP ���ж�IP
 *
 * @returns -1 �ǹ�������IP
 */
/* ---------------------------------------------------------------------------*/
static int videoTransIsCenterIP(const char *IP)
{
	int i;
	for(i=0;i<Public.CenterCnt;i++) {
		if(strcmp(IP,Public.pCenterMsg[i].IP)==0) {
			return i;
		}
	}
	return -1;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransSetPower ��ʼ��ֹͣ��Ƶʱ�򿪻�ر�����ͷ��Դ
 *
 * @param start 1������0�ر�
 */
/* ---------------------------------------------------------------------------*/
static void videoTransSetPower(BOOL start)
{
	// printf("CCD POWER %d\n",Start);
	if(start) {
		talkLedControl(1);
	} else {
		talkLedControl(0);
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransDoNothing �����κ��£�ֻ�ı�״̬
 *
 * @param this
 * @param st_data
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int videoTransDoNothing(VideoTrans * this,STMData *st_data)
{
	return CALL_OK;
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransJudgeType �жϺ�����Դ���ͣ��ſڻ������ڻ�
 *
 * @param this
 * @param st_data
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int videoTransJudgeType(VideoTrans * this,STMData *st_data)
{
	const char *IP = st_data->abd.IP;
	int Port= st_data->abd.Port;
	center_data = (STMData *)this->ST_machine->initPara(this->ST_machine,
			sizeof(STMData));
	memcpy(center_data,st_data,sizeof(STMData));
	if (videoTransIsCenterIP(IP) >= 0 ) {
#if (defined TC3000_BZ)
		videoTransSendCallRevert(IP,Port,CMD_SHAKEHANDS);
#endif
		this->ST_machine->MsgPost(this->ST_machine,
				EVENT_TYPE_CENTER,center_data);
	}
	this->ST_machine->MsgPost(this->ST_machine,EVENT_TYPE_HM,center_data);
	return CALL_OK;
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransCallIP ֱ�Ӻ���IP
 * ����ɹ�����������Ҫת����Ƶ���Է�������������Ƶ
 * @param this
 * @param IP �Է�IP
 * @param Port �Է��˿�
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int videoTransCallIP(VideoTrans * this,STMData *st_data)
{
	this->PeerPort = 7800;
	strcpy(this->cPeerIP,Public.mCallIP);
	videoTransSendCallCmd(this,Public.mCallIP,7800,CMD_CALL,TRUE);
	this->uiCallIP();
	memset(Public.mCallIP,0,sizeof(Public.mCallIP));
	return CALL_OK;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransShakeHands ���жԽ�ǰ����������
 * ��ʹ��3000Э��Խ�ʱ��Ҫ��������Э�飬������U9Э�飬2�߻�����ͨ��
 * �ں��й�������ʱ����Ҫ�ȷ����֣�����Ȼ�Ӧ���ٷ�����
 *
 * @param this
 * @param IP �Է�IP
 * @param Port �Է��˿�
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int videoTransShakeHands(VideoTrans * this,STMData *st_data)
{
	strcpy(this->cPeerIP,Public.mCallIP);
	videoTransSendCallCmd(this,Public.mCallIP,7800,CMD_SHAKEHANDS,TRUE);
	this->uiShakeHands();
	return CALL_OK;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransShakeHandsRet ���жԽ�ǰ�����������
 * ��ʹ��3000Э��Խ�ʱ��Ҫ��������Э�飬������U9Э�飬2�߻�����ͨ��
 *
 * @param this
 * @param IP �Է�IP
 * @param Port �Է��˿�
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int videoTransShakeHandsRet(VideoTrans * this,STMData *st_data)
{
	const char *IP = st_data->abd.IP;
	int Port= st_data->abd.Port;
	videoTransSendCallRevert(IP,Port,CMD_SHAKEHANDS_ASW);
	return CALL_OK;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransRetCall ���غ��еĽ��
 *
 * @param this
 * @param RetCode ASW_OK ���гɹ�����ʼ����RTP���Ӵ�����Ƶ����������
 * ��Ƶ
 *				  ASW_FAIL ����ʧ�ܣ�˵���Է����ߣ���������æ״̬
 */
/* ---------------------------------------------------------------------------*/
static int videoTransRetCall(VideoTrans *this,STMData *st_data)
{
	if (videoTransIsCenterIP(this->cPeerIP) == -1 ) {
		Public.SendCallInfoToCenter();
	}
	videoTransSetPower(1);
	if(this->rtp->init(this->rtp,
				this->cPeerIP,
				Public.cLocalIP,
				Public.cInternetIP,
				8800,0) != CALL_OK) {
		return CALL_FAIL;
	}
	this->rtp->Flag = 0;
	this->rtp->buildConnect(this->rtp,Public.FramPeriod);
	this->CallTimeout = RING_TIME;
	this->uiRetCall();
	return CALL_OK;
}
#if 0
/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransNewInterface ������Ƶ��ȡ�ӿ�
 *
 *
 * @param this
 * @param IP �Է�IP
 * @param Port �Է��˿�
 * @param pCall ��ز���
 *
 * @returns
// ����ɹ������ط�0,ʧ�ܷ���0
 */
/* ---------------------------------------------------------------------------*/
int videoTransNewInterface(VideoTrans * this,VideoTransNeedData * NeedData)
{
	printf("[%s]-------------------Begin  IP:[%s]&Port:[%d]--------------\n",__FUNCTION__,NeedData->IP,NeedData->Port);
	const char *IP = NeedData->IP;
	int Port= NeedData->Port;
	int IsTransVideo = NeedData->IsTransVideo;
	this->m_CodeType = NeedData->VideoType;
	//���öԷ�IP������IP�Ͷ˿ںŷ��ͻظ����Է�
	strcpy(this->cPeerIP,IP);
	this->PeerPort = Port;
	printf("[%s]-------------------ret init----------------\n",__FUNCTION__);
	if(this->rtp->init(this->rtp,
				this->cPeerIP,
				Public.cLocalIP,
				Public.cInternetIP,
				this->PeerPort,1) != CALL_OK) {
	//	ReplyVideoDataRequest(IP,Port,ASW_FAIL);
		return CALL_FAIL;
	}
	//ReplyVideoDataRequest(IP,Port,ASW_OK);
	printf("[%s]-------------------ret buildConnect----------------\n",__FUNCTION__);
	//ֻ������Ƶ����,��������Ƶ����

	this->rtp->bTransVideo = IsTransVideo;
	this->rtp->Flag = 1;
	this->rtp->buildConnect(this->rtp,Public.FramPeriod);

	printf("[%s]-------------------End----------------\n",__FUNCTION__);	
	return CALL_OK;
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransNewInterface_Close ����ֹͣ��Ƶ��ȡ�ӿ�
 *
 *
 * @param this
 *
 */
/* ---------------------------------------------------------------------------*/
void videoTransNewInterface_Close(VideoTrans * this)
{
	printf("[%s]-------------------Begin Close VideoTrans --------------\n",__FUNCTION__);
	this->rtp->Flag = 1;
	this->rtp->close(this->rtp);
	printf("[%s]-------------------End----------------\n",__FUNCTION__);	
	return ;
}
#endif

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransMonitor �Է���������,���壬����ʾIP��ַ������Է�
 * ������Ƶ������ʾ��
 * ��Ϊ3000ϵͳʱ����������ڻ����У���Ҫ�Ƚ��ܹ�����Э�飬����Ӧ��
 * ����ǹ������ģ��Ȼ�����Э�飬����Ӧ
 *
 *
 * @param this
 * @param IP �Է�IP
 * @param Port �Է��˿�
 * @param pCall ��ز���
 *
 * @returns
// ����ɹ������ط�0,ʧ�ܷ���0
 */
/* ---------------------------------------------------------------------------*/
static int videoTransMonitor(VideoTrans * this,STMData *st_data)
{
	const char *IP = st_data->abd.IP;
	int Port= st_data->abd.Port;
	COMMUNICATION_CALL *pCall = &st_data->p_call;

	this->m_CodeType = pCall->VideoType;
	this->dwPeerType = pCall->CallType;
	//���öԷ�IP������IP�Ͷ˿ںŷ��ͻظ����Է�
	strcpy(this->cPeerIP,IP);
	strncpy(this->cPeerID,pCall->CallID,sizeof(this->cPeerID)-1);
	this->PeerPort = UDPSERVER_PORT;

	videoTransSetPower(1);
	if(this->rtp->init(this->rtp,
				this->cPeerIP,
				Public.cLocalIP,
				Public.cInternetIP,
				8800,0) != CALL_OK) {
		videoTransSendCallRevert(IP,Port,ASW_FAIL);
		return CALL_FAIL;
	}
	videoTransSendCallRevert(IP,Port,ASW_OK);
				
	this->rtp->Flag = 0;
	this->rtp->buildConnect(this->rtp,Public.FramPeriod);
	strncpy(this->cPeerRoom,pCall->CallName,sizeof(this->cPeerRoom)-1);
	GetDate(this->cDate,sizeof(this->cDate));
	this->CallTimeout = RING_TIME;
	this->uiMonitor();
	return CALL_OK;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransAnswer �Է�����������ͨ��(������Ƶ������Ƶͬʱ����)
 *
 * @param this
 * @param IP �Է�IP
 * @param Port �Է��˿�
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static BOOL videoTransAnswer(VideoTrans * this,STMData *st_data)
{

	const char *IP = st_data->abd.IP;

	PlayMp3->Stop(PlayMp3);     //stop the calling sound

	strncpy(this->cPeerIP,IP,15);
	this->PeerPort = UDPSERVER_PORT;
	this->rtp->setPeerIp(this->rtp,IP);
	this->rtp->startAudio(this->rtp);
	if(this->bLeaveWord) {
		this->CallTimeout = LEAVE_WORD_TIME;
	} else {
		this->CallTimeout = TALK_TIME;
	}

	this->uiAnswer();
	return TRUE;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransRetFail ͨ�����б����ӣ�����ʧ��
 *
 * @param this
 * @param st_data
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int videoTransRetFail(VideoTrans * this,STMData *st_data)
{
	const char *IP = st_data->abd.IP;
	int Port = st_data->abd.Port;

	videoTransSendCallRevert(IP,Port,ASW_FAIL);
	return CALL_OK;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransOver �����Խ�
 *
 * @param this
 * @param ByMyself 1��������������0Զ�̷����������
 */
/* ---------------------------------------------------------------------------*/
static int videoTransOver(VideoTrans * this,STMData *st_data)
{
	int ByMyself = st_data->flag;
	this->bLeaveWord = FALSE;
	this->CallTimeout = 0;
	this->rtp->Flag = 0;
	this->rtp->close(this->rtp);
	if(ByMyself) {
		videoTransSendCallCmd(this,this->cPeerIP,this->PeerPort,CMD_OVER,FALSE);
	}
	if (Public.fTransmitToCenter) {
		Public.fTransmitToCenter = 0;
		videoTransSendCallCmd(this,Public.pCenterMsg->IP,this->PeerPort,CMD_OVER,FALSE);
	}
	
	videoTransSetPower(0);
	this->cPeerIP[0] = 0;
	this->PeerPort = 0;
	this->uiOver();
	printf("videoTransOver\n");
	return CALL_OK;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransFail ����ʧ��
 *
 * @param this
 * @param st_data
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int videoTransFail(VideoTrans * this,STMData *st_data)
{
	int type = st_data->flag;
	if (type == 0) {
		this->uiFailComm();
	} else if (type == 1) {
		this->uiFailShakeHands();
	} else if (type == 2) {
		this->uiFailBusy();
	} else if (type == 3) {
		st_data->flag = 1;
		videoTransOver(this,st_data);
		this->uiFailAbort();
	}

	return CALL_OK;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransGetStatus ��õ�ǰͨ��״̬
 *
 * @param this
 *
 * @returns ����״̬����ǰ״̬
 */
/* ---------------------------------------------------------------------------*/
static int videoTransGetStatus(VideoTrans * this)
{
	return this->ST_machine->getCurState(this->ST_machine);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransGetCallTime ��õ�ǰͨ������ʱʱ�䣬������ʾ��
 * ��ʱ
 *
 * @param this
 *
 * @returns ͨ��ʱ��
 */
/* ---------------------------------------------------------------------------*/
static int videoTransGetCallTime(VideoTrans *this)
{
	return this->CallTimeout;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransHandleCallTime ��1s��ʱ������ִ�У�ͨ��ʱ-1
 *
 * @param this
 *
 * @returns 1 ͨ��ʱ����� 0 δ����
 */
/* ---------------------------------------------------------------------------*/
static int videoTransHandleCallTime(VideoTrans *this)
{
	if(this->CallTimeout > 0) {
		this->CallTimeout--;
		if (this->CallTimeout == 10) {
			return 2;
		}
		if(this->CallTimeout == 0) {
			data = (STMData *)this->ST_machine->initPara(this->ST_machine,
					sizeof(STMData));
			data->flag = 1;
			this->ST_machine->MsgPost(this->ST_machine,EVENT_HANDS_UP,data);
			return 1;
		}
	}
	return 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransHandleCallOutTime ���г�ʱ
 *
 * @param this
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int videoTransHandleCallOutTime(VideoTrans *this)
{
	data = (STMData *)this->ST_machine->initPara(this->ST_machine,
			sizeof(STMData));
	data->flag = 0;
	this->ST_machine->MsgPost(this->ST_machine,DO_FAIL_COMM,data);
	return 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransHandleShakeOutTime ���ֳ�ʱ
 *
 * @param this
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int videoTransHandleShakeOutTime(VideoTrans *this)
{
	data = (STMData *)this->ST_machine->initPara(this->ST_machine,
			sizeof(STMData));
	data->flag = 1;
	this->ST_machine->MsgPost(this->ST_machine,EVENT_FAIL_SHAKE,data);
	return 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransGetFpAudio ���ͨ��ʱ���������ַ
 *
 * @param this
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int *videoTransGetFpAudio(VideoTrans *this)
{
	return this->rtp->getFpAudio(this->rtp);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransSetSilence ����ͨ��ʱ�����Ƿ���
 *
 * @param this
 * @param value 1������0�Ǿ���
 */
/* ---------------------------------------------------------------------------*/
static void videoTransSetSilence(VideoTrans *this,BOOL value)
{
	this->rtp->setSilence(this->rtp,value);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransGetSilence ���ͨ������״̬
 *
 * @param this
 *
 * @returns 1������0�Ǿ���
 */
/* ---------------------------------------------------------------------------*/
static int videoTransGetSilence(VideoTrans *this)
{
	return this->rtp->getSilence(this->rtp);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransDestroy ���ٶ����˳�����
 *
 * @param this
 */
/* ---------------------------------------------------------------------------*/
static void videoTransDestroy(VideoTrans * this)
{
	this->ST_machine->Destroy(&this->ST_machine);
	this->rtp->Destroy(&this->rtp);
	free(this);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief udpCmdCall ͨ��Э��:������
 *
 * @param this
 * @param ABinding
 * @param AData
 */
/* ---------------------------------------------------------------------------*/
static void udpCmdCall(VideoTrans * this,
		SocketHandle *ABinding,
		SocketPacket *AData)
{
	DBG_P("CMD_CALL:Ring()\n");
	COMMUNICATION_CALL *pCall = (COMMUNICATION_CALL *)&AData->Data[sizeof(COMMUNICATION)];
	data = (STMData *)this->ST_machine->initPara(this->ST_machine,
			sizeof(STMData));
	memcpy(&data->abd,ABinding,sizeof(SocketHandle));
	memcpy(&data->p_call,pCall,sizeof(COMMUNICATION_CALL));
	this->ST_machine->MsgPost(this->ST_machine,EVENT_MONITOR,data);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief udpCmdTalk ͨ��Э��:�Է�����
 *
 * @param this
 * @param ABinding
 * @param AData
 */
/* ---------------------------------------------------------------------------*/
static void udpCmdTalk(VideoTrans * this,
		SocketHandle *ABinding,
		SocketPacket *AData)
{
	DBG_P("CMD_TALK:Answer():%s\n",ABinding->IP);
	data = (STMData *)this->ST_machine->initPara(this->ST_machine,
			sizeof(STMData));
	memcpy(&data->abd,ABinding,sizeof(SocketHandle));
	this->ST_machine->MsgPost(this->ST_machine,EVENT_TALK,data);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief udpCmdOver ͨ��Э��:�Է��һ�
 *
 * @param this
 * @param ABinding
 * @param AData
 */
/* ---------------------------------------------------------------------------*/
static void udpCmdOver(VideoTrans * this,
		SocketHandle *ABinding,
		SocketPacket *AData)
{
	DBG_P("CMD_OVER:Over(),From IP: %s\n", ABinding->IP);
	
	if(strcmp(this->cPeerIP,ABinding->IP)!=0) {
		return;
	}
	
	data = (STMData *)this->ST_machine->initPara(this->ST_machine,
			sizeof(STMData));
	data->flag = 0;
	this->ST_machine->MsgPost(this->ST_machine,EVENT_HANDS_UP,data);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief udpCmdUnlock ͨ��Э��:����
 *
 * @param this
 * @param ABinding
 * @param AData
 */
/* ---------------------------------------------------------------------------*/
static void udpCmdUnlock(VideoTrans * this,
		SocketHandle *ABinding,
		SocketPacket *AData)
{
	DBG_P("CMD_UNLOCK:Unlock()\n");
	if (strcmp(ABinding->IP,Public.pCenterMsg->IP) == 0) {
		int UnLockRet = 0;
		if(Public.IsAllowUnLock == 1){
			UnLockRet = 0xF0;
			videoTransSendUnLockResult(ABinding->IP,ABinding->Port,UnLockRet);
			return;
		}
		UnLockRet = 0xF1;
		Public.UnlockTheDoor(2);
		videoTransSendUnLockResult(ABinding->IP,ABinding->Port,UnLockRet);
	} else {
		Public.UnlockTheDoor(1);
	}

	char SqlStr[512];
	char Code[12]={0};
	// xb add ���ò��ҿ��Ž��п�������
	sprintf(SqlStr,"Select Code,IP From UserInfo Where IP='%s'",ABinding->IP);
	LocalQueryOpen(LocalQuery,SqlStr);
	int count = LocalQuery->RecordCount(LocalQuery);
	if(Public.eElevator && count) {
		TElevator lift;
		LocalQueryOfChar(LocalQuery,"Code",Code,12);
		Public.EleVatorFloor = atoi(Code);
		lift.AddrH = Public.ElevatorAddr%0x100;
		lift.AddrL = Public.EleVatorFloor/10000;
		lift.Function = 0x02;
		lift.CellCode = (char)(Public.ElevatorFloorCode&0xFF);
		lift.FloorCode = (char)(((Public.EleVatorFloor/100)%100)&0xFF);
		lift.Reserved = (char)(Public.ElevatorLobby);
		lift.RoomNumber = (char)((Public.EleVatorFloor%100)&0xFF);;
		Public.ElevatorSndData(&lift);
	}
	LocalQuery->Close(LocalQuery);
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief udpCmdAswOk ͨ��Э��:���к�Է�Ӧ��OK
 *
 * @param this
 * @param ABinding
 * @param AData
 */
/* ---------------------------------------------------------------------------*/
static void udpCmdAswOk(VideoTrans * this,
		SocketHandle *ABinding,
		SocketPacket *AData)
{
	DBG_P("CMD_ASW_OK:RetCall()IP:%s\n",ABinding->IP);
//	if (strcmp(ABinding->IP,this->cPeerIP) == 0){
		this->ST_machine->MsgPost(this->ST_machine,EVENT_CALL,NULL);
/*	} else {
		data = (STMData *)this->ST_machine->initPara(this->ST_machine,
				sizeof(STMData));
		data->flag = 0;
		this->ST_machine->MsgPost(this->ST_machine,EVENT_FAIL_COMM,data);
	}
*/	
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief udpCmdAswFail ͨ��Э��:���к�Է�Ӧ��ʧ��(��æ)
 *
 * @param this
 * @param ABinding
 * @param AData
 */
/* ---------------------------------------------------------------------------*/
static void udpCmdAswFail(VideoTrans * this,
		SocketHandle *ABinding,
		SocketPacket *AData)
{
	DBG_P("CMD_ASW_FAIL:RetFail()IP:%s\n",ABinding->IP);
	data = (STMData *)this->ST_machine->initPara(this->ST_machine,
			sizeof(STMData));
	data->flag = 2;
	this->ST_machine->MsgPost(this->ST_machine,EVENT_FAIL_BUSY,data);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief udpCmdShakeHands ͨ��Э��:�Է���������Э��
 *
 * @param this
 * @param ABinding
 * @param AData
 */
/* ---------------------------------------------------------------------------*/
static void udpCmdShakeHands(VideoTrans * this,
		SocketHandle *ABinding,
		SocketPacket *AData)
{
	DBG_P("CMD_SHAKEHANDS :shakeHandsRet()\n");
	data = (STMData *)this->ST_machine->initPara(this->ST_machine,
			sizeof(STMData));
	memcpy(&data->abd,ABinding,sizeof(SocketHandle));
	this->ST_machine->MsgPost(this->ST_machine,EVENT_SHAKEHANDS_ASW,data);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief udpCmdShakeHandsAsw ͨ��Э��:�Է�Ӧ������Э��
 *
 * @param this
 * @param ABinding
 * @param AData
 */
/* ---------------------------------------------------------------------------*/
static void udpCmdShakeHandsAsw(VideoTrans * this,
		SocketHandle *ABinding,
		SocketPacket *AData)
{
	DBG_P("CMD_SHAKEHANDS_ASW :CallIP()\n");
	this->ST_machine->MsgPost(this->ST_machine,EVENT_DIAL,NULL);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief ͨ��״̬��ִ�к�������ö��˳��һ��
 */
/* ---------------------------------------------------------------------------*/
static STMDo stm_video_state_do[] = {
	{DO_NO,				videoTransDoNothing},
	{DO_SHAKEHANDS,		videoTransShakeHands},
	{DO_SHAKEHANDS_ASW,	videoTransShakeHandsRet},
	{DO_JUDGE_TYPE,		videoTransJudgeType},
	{DO_DIAL,			videoTransCallIP},
	{DO_CALL,			videoTransRetCall},
	{DO_TALK,			videoTransAnswer},
	{DO_MONITOR,		videoTransMonitor},
	{DO_FAIL_COMM,		videoTransFail},
	{DO_FAIL_SHAKE,		videoTransFail},
	{DO_FAIL_BUSY,		videoTransFail},
	{DO_FAIL_ABORT,		videoTransFail},
	{DO_RET_FAIL,		videoTransRetFail},
	{DO_HANDS_UP,		videoTransOver},
};

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransSTMHanldeVideo ��������״̬��ִ����
 *
 * @param this
 * @param data �������
 */
/* ---------------------------------------------------------------------------*/
static void videoTransSTMHanldeVideo(State_machine *this,void *data)
{
	if ((this->TCB.status & TSK_BUSY) != TSK_BUSY) {
		return;
	}
	this->TCB.status &= ~TSK_BUSY;
	STMData *st_data = (STMData *)data;
	stm_video_state_do[this->G_run].proc(video,st_data);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief ͨ��Э�鴦���
 */
/* ---------------------------------------------------------------------------*/
static UdpCmd udp_cmd_handle[] = {
	{CMD_CALL,			udpCmdCall},
	{CMD_TALK,			udpCmdTalk},
	{CMD_OVER,			udpCmdOver},
	{CMD_UNLOCK,		udpCmdUnlock},
	{ASW_OK,			udpCmdAswOk},
	{ASW_FAIL,			udpCmdAswFail},
	{CMD_SHAKEHANDS,	udpCmdShakeHands},
#if (defined TC3000_BZ)
	{CMD_SHAKEHANDS_ASW,udpCmdShakeHandsAsw},
#endif
};

/* ---------------------------------------------------------------------------*/
/**
 * @brief udpCallHandle ͨ��Э�鴦��
 *
 * @param this
 * @param ABinding
 * @param AData
 */
/* ---------------------------------------------------------------------------*/
static void udpCallHandle(VideoTrans *this,
	   	SocketHandle *ABinding,
	   	SocketPacket *AData)
{
	if(AData->Size != sizeof(COMMUNICATION) + sizeof(COMMUNICATION_CALL)) {
		return;
	}

	COMMUNICATION_CALL *pCall = (COMMUNICATION_CALL *)&AData->Data[sizeof(COMMUNICATION)];
	unsigned int i;
	for(i=0; i<NELEMENTS(udp_cmd_handle); i++) {
		if (udp_cmd_handle[i].cmd == pCall->Cmd) {
			udp_cmd_handle[i].proc(this,ABinding,AData);
			return;
		}
	}
	DBG_P("[%s] : call cmd = 0x%04x\n",__FUNCTION__,pCall->Cmd);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoSTMHandle ��������ź��߳�ִ�к���
 *
 * @param arg
 */
/* ---------------------------------------------------------------------------*/
static void *videoSTMHandle(void *arg)
{
	State_machine *ST_machine = (State_machine *)arg;
	while (1) {
		ST_machine->Run(ST_machine);
		usleep(1000);
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoSTMHandleThreadCreate ������������߳�
 */
/* ---------------------------------------------------------------------------*/
static void videoSTMHandleThreadCreate(State_machine *arg)
{
	int result;
	pthread_t m_pthread;					//�̺߳�
	pthread_attr_t threadAttr1;				//�߳�����

	pthread_attr_init(&threadAttr1);		//���Ӳ���
	//�����߳�Ϊ�Զ�����
	pthread_attr_setdetachstate(&threadAttr1,PTHREAD_CREATE_DETACHED);	
	result = pthread_create(&m_pthread,&threadAttr1,videoSTMHandle,arg);
	if(result) {
		printf("[%s] pthread failt,Error code:%d\n",__FUNCTION__,result);
	}
	pthread_attr_destroy(&threadAttr1);		//�ͷŸ��Ӳ���
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransCreate �������жԽ�����
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
VideoTrans * videoTransCreate(void) {
	VideoTrans * this = (VideoTrans *)malloc(sizeof(VideoTrans));
	if(this==NULL) {
		ErrorLog(0x7FFFFFF,"���������ڴ�:�����ڴ�ʧ��!");
		return NULL;
	}
	memset(this,0,sizeof(VideoTrans));

	this->dwInCameraWidth = h264_type[Public.VideoType].img_width;
	this->dwInCameraHeight = h264_type[Public.VideoType].img_height;
	this->rtp = createRtp(Public.VideoType,callbackRtp,this);
	this->ST_machine = stateMachineCreate(ST_IDLE,
			stm_video_state,
			NELEMENTS(stm_video_state),
			0,
			videoTransSTMHanldeVideo);
	this->bLeaveWord = FALSE;

	this->Destroy = videoTransDestroy;
	this->getStatus = videoTransGetStatus;
	this->getCallTime = videoTransGetCallTime;
	this->handleCallTime = videoTransHandleCallTime;
	this->handleCallOutTime = videoTransHandleCallOutTime;
	this->handleShakeOutTime = videoTransHandleShakeOutTime;
	this->getFpAudio = videoTransGetFpAudio;
	this->setSilence = videoTransSetSilence;
	this->getSilence = videoTransGetSilence;
	this->udpCallHandle = udpCallHandle;
	videoSTMHandleThreadCreate(this->ST_machine);
	return this;
}

