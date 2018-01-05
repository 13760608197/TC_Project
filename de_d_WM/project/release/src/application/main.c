/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Main Window in MiniGUI
 *
 *        Version:  1.0
 *        Created:  2015-10-30 10:34:18
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
/* ---------------------------------------------------------------------------*
 *                      include head files
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <time.h>
#include <sys/time.h>

#include <signal.h>

#include "externfunc.h"

/* ---------------------------------------------------------------------------*
 *                  extern variables declare
 *----------------------------------------------------------------------------*/
// ��ӡ֡��ʱʹ��
// extern int seq;
// extern int cnt_test;
/* ---------------------------------------------------------------------------*
 *                  internal functions declare
 *----------------------------------------------------------------------------*/
static void waveInTalkBack(void);
static void waveShakeHandsTimeOut(void);
static void createGpioHandleThread(void);
static void createCheckInputThread(void);

/* ---------------------------------------------------------------------------*
 *                        macro define
 *----------------------------------------------------------------------------*/
#if DBG_MAIN > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif
#define MAIN_TIMERVAL	90000	//��ȷ��ʱ��: ΢��

/* ---------------------------------------------------------------------------*
 *                      variables define
 *----------------------------------------------------------------------------*/
FormMain *form_main = NULL;
TSqlite *LocalQuery = NULL;
TUDPServer *UdpServer = NULL;
UartDeal *uart = NULL;
RfCard *Card = NULL;
PlayMp3Struct *PlayMp3 = NULL;
PlayMp3Struct *PlayUnlock = NULL;
DownLoadData *DownData = NULL;
MyMonitor * monitor = NULL;


static STMData *data = NULL;
static BOOL fCallObject = FALSE; // FALSE ��������  TRUE �����û�
static BOOL voice_call_fail = TRUE; // ����"����ʧ��"�Ƿ����ڲ��ţ����ظ����� TRUE Ϊ�������
static BOOL m_bKeyVoiceprompt = TRUE; //����������ʾ ��ʾ"�����Ѵ���"��ֻ��ʾһ�� =TRUE ����ʾ

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoUiCallShakeHands UI����������������Э��
 */
/* ---------------------------------------------------------------------------*/
static void videoUiCallShakeHands(void)
{
	form_main->timerStart(IDC_TIMER_SHAKEHANS_OUT);
	form_main->setTextOpration(Word[WORD_CALLING].string);
	form_main->showCalling();
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoUiCallIP UI������:���ͺ�������
 */
/* ---------------------------------------------------------------------------*/
static void videoUiCallIP(void)
{
	form_main->timerStop(IDC_TIMER_SHAKEHANS_OUT);
	form_main->timerStart(IDC_TIMER_CALL_OUT);
	form_main->setTextOpration(Word[WORD_CALLING].string);
	form_main->showCalling();
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief videoUiRetCall UI������:���гɹ����ش���������ʱ����ʾ
 * ����ʱ
 */
/* ---------------------------------------------------------------------------*/
static void videoUiRetCall(void)
{
	Mixer->ClearRecBuffer(Mixer);
	form_main->timerStop(IDC_TIMER_CALL_OUT);
	form_main->timerStart(IDC_TIMER_CALL_DISP_200ms);
	PlayMp3->Play(PlayMp3,"res/wav/BackRing.mp3",1,NULL);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoUiMonitor UI������:������ʱ���ı���̵ĺ��м�Ϊ�һ���
 */
/* ---------------------------------------------------------------------------*/
static void videoUiMonitor(void)
{
	// form_main->setKeyboardHandsUp();
	form_main->showCalling();
	form_main->setKeyboardType(KEY_TYPE_STANDY);
	form_main->setTextDial(Word[WORD_PLEASE_INSERT_PHONE_NUM].string);
	form_main->timerStart(IDC_TIMER_CALL_DISP_200ms);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoUiAnswer UI������:�Է�����ʱ,��ʼ���������ã����
 * ���ԣ��򲥷�"������"��ͨ��"�Ѿ���������Խ�",ͬʱ�������öԽ�����
 */
/* ---------------------------------------------------------------------------*/
static void videoUiAnswer(void)
{
	if (*video->getFpAudio(video) != 0) {
//		SaveLog("\n");
		Mixer->DeInitPlay(Mixer,video->getFpAudio(video));
	}
	if (video->bLeaveWord) {
		Public.PlayVoice("PleaseLeaveMessage");
	} else {
		Public.PlayVoiceWithBack("PleaseTalk",waveInTalkBack);
	}
	Mixer->SetVolumeEx(Mixer,Public.TalkVolume);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoUiOver UI������:�һ�ʱ�����ص���ʾ��ʱ��,�ָ�����
 * ����,�ص�����ͨ����ʱ��
 */
/* ---------------------------------------------------------------------------*/
static void videoUiOver(void)
{
	Mixer->SetVolumeEx(Mixer,Public.CurrentVolume);
	form_main->timerStop(IDC_TIMER_CALL_DISP_200ms);
	form_main->clearDialNumber();
	form_main->setTextOpration("");
	form_main->setTextDial(Word[WORD_PLEASE_INSERT_PHONE_NUM].string);
	PlayMp3->Stop(PlayMp3);
	if (Public.language != CHINEASE || Public.language != TAIWAN){
		PlayMp3->Play(PlayMp3,"res/wav/dodo.mp3",0,NULL);
	} else {
		Public.PlayVoice("HandsUp");
	}
	form_main->timerStopAllCalling();
	form_main->setKeyboardType(KEY_TYPE_STANDY);
	form_main->resetKeyPressTime();
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoUiFailComm UI������:IP���������
 */
/* ---------------------------------------------------------------------------*/
static void videoUiFailComm(void)
{
	memset(Public.mCallIP,0,sizeof(Public.mCallIP));
	form_main->clearDialNumber();
	form_main->setKeyboardType(KEY_TYPE_STANDY);
	form_main->timerStop(IDC_TIMER_CALL_OUT);
	form_main->timerStop(IDC_TIMER_SHAKEHANS_OUT);
	if (Public.language != CHINEASE || Public.language != TAIWAN){
		PlayMp3->Play(PlayMp3,"res/wav/dodo.mp3",0,NULL);
	} else {
		if (voice_call_fail) {
			Public.PlayVoiceWithBack("CallFail",waveShakeHandsTimeOut);
			voice_call_fail = FALSE;
		}
	}
	form_main->setTextOpration(Word[WORD_CALLING_FAIL].string);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoUiFailShakeHands UI������:���ֳ�ʱ��Э�����Ͳ�ƥ��
 */
/* ---------------------------------------------------------------------------*/
static void videoUiFailShakeHands(void)
{
	videoUiFailComm();
	form_main->setTextOpration(Word[WORD_DEVICE_TYPE_WRONG].string);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoUiFailBusy UI������:���з�����æ
 */
/* ---------------------------------------------------------------------------*/
static void videoUiFailBusy(void)
{
	form_main->timerStop(IDC_TIMER_CALL_OUT);
	form_main->setKeyboardType(KEY_TYPE_STANDY);
	if (Public.language != CHINEASE || Public.language != TAIWAN){
		PlayMp3->Play(PlayMp3,"res/wav/dodo.mp3",0,NULL);
	} else {
		Public.PlayVoice("CallBuzy");
	}
	form_main->setTextOpration(Word[WORD_CALLING_BUZY].string);
	form_main->clearDialNumber();
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoUiFailAbort UI������:��Ƶͨ�������жϴ���
 */
/* ---------------------------------------------------------------------------*/
static void videoUiFailAbort(void)
{
	form_main->setTextOpration(Word[WORD_MONITOR_COMM_ERR].string);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoLoadUiFunc װ��UI������
 *
 * @param this
 */
/* ---------------------------------------------------------------------------*/
static void videoLoadUiFunc(VideoTrans *this)
{
	this->uiShakeHands = videoUiCallShakeHands;
	this->uiCallIP = videoUiCallIP;
	this->uiRetCall = videoUiRetCall;
	this->uiFailComm = videoUiFailComm;
	this->uiFailShakeHands = videoUiFailShakeHands;
	this->uiFailBusy = videoUiFailBusy;
	this->uiFailAbort = videoUiFailAbort;
	this->uiMonitor = videoUiMonitor;
	this->uiAnswer = videoUiAnswer;
	this->uiOver = videoUiOver;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief CountIPByRoomNum  ���ݷ���ż����ӦIP
 *
 * @return	result���
 */
/* ---------------------------------------------------------------------------*/
static int	CountIPByRoomNum(void){
	char DH[8],DYH[8],CH[8],FH[8];
	int dong,dy,ceng,fh;
	int i;
	int Ret = 0;
	char Getinbuf[32];
	form_main->listDeletAll();
	Ret = form_main->getTextNumber(Getinbuf);
	if(Ret == 0 || Ret != 4) {
		form_main->listRefresh();
		return Ret;
	}
	if(!Public.Unit_Num){
		for(i=0;i<Ret;i++){
			if(i == 0){
				sprintf(DYH,"%c",Getinbuf[i]);
			}else if(i == 2){
				sprintf(CH,"%c%c",Getinbuf[i-1],Getinbuf[i]);
			}else if(i == 3){
				sprintf(FH,"%c",Getinbuf[i]);
			}
		}
	}else{
		for(i=0;i<Ret;i++){
			if(i == 1){
				sprintf(CH,"%c%c",Getinbuf[i-1],Getinbuf[i]);
			}else if(i == 3){
				sprintf(FH,"%c%c",Getinbuf[i-1],Getinbuf[i]);
			}
		}			
	}	
	dong = Public.Build_Num;
	if(!Public.Unit_Num){
		dy = atoi(DYH);
	}else{
		dy = Public.Unit_Num;	
	}
	ceng = atoi(CH);
	fh = atoi(FH);

	printf("dong : [%d] ,dy : [%d],ceng : [%d],fh : [%d] \n",dong,dy,ceng,fh);
	
	c_RoomInfo * room = (c_RoomInfo *)calloc(1,sizeof(c_RoomInfo));
	room->c_DH = dong;
	room->c_DYH = dy;
	room->c_CH = ceng;
	room->c_FH = fh;
	Ret = CheckRoomIP(room);
	if(Ret){
		memcpy(Public.mCallIP,room->c_ip,sizeof(room->c_ip));
	}else{
		memset(Public.mCallIP,0,sizeof(Public.mCallIP));
	}
	
	free(room);
	return Ret;

} 

#if 0
/* ---------------------------------------------------------------------------*/
/**
 * @brief CountIPByRoomNum  ���ݷ���ż����ӦIP
 *
 * @return	result���
 */
/* ---------------------------------------------------------------------------*/
static int	CountIPByRoomNum(void){
	int Ret = 0;
	char Getinbuf[12];
	char TmpIP[16] = "172.16.1.1";
	unsigned long s_addr = 0;
	memset(Public.mCallIP,0,sizeof(Public.mCallIP));
	form_main->listDeletAll();
	Ret = form_main->getTextNumber(Getinbuf);
	if(Ret == 0) {
		form_main->listRefresh();
		return 0;
	}

	int Counts = atoi(Getinbuf);

	s_addr = inet_addr(TmpIP);
	s_addr = ntohl(s_addr);
	s_addr += Counts;
	s_addr = htonl(s_addr);

	struct in_addr Re_addr;
	Re_addr.s_addr = s_addr;
	char * ReIP = inet_ntoa(Re_addr);
	
	strncpy(Public.mCallIP,ReIP,16);

	return 1;

} 
#endif


/* ---------------------------------------------------------------------------*/
/**
 * @brief getRoomNum ���ҷ���
 *
 * @returns ���ҽ������
 */
/* ---------------------------------------------------------------------------*/
#define LISTDISP_NUM	5 // �б������ʾ��¼��
static int getRoomNum(void)
{
	char SqlStr[512];
	IPADDRESS2 CodeIp[LISTDISP_NUM];
	int Ret = 0;
	char tmp[64], Getinbuf[12],Getinbuf_next[12];
	BOOL bFirstIndex = FALSE;

	memset(Public.mCallIP,0,sizeof(Public.mCallIP));
	form_main->listDeletAll();
	if(form_main->getTextNumber(Getinbuf) == 0) {
		form_main->listRefresh();
		return 0;
	}
		 memcpy(Getinbuf_next,Getinbuf,sizeof(Getinbuf));
		 int index = strlen(Getinbuf);
		 Getinbuf_next[index - 1] += 1;
		 
		 sprintf(SqlStr,
			//"Select Code, IP,Roomname From UserInfo where Code like '%s%%' limit 0,4",
			"Select Code, IP,Roomname From UserInfo where Code >= '%s' and Code <= '%s' limit 0,4",
			Getinbuf,Getinbuf_next);
	LocalQueryOpen(LocalQuery,SqlStr);	//��ѯ

	int num = LocalQuery->RecordCount(LocalQuery);
	memset(CodeIp, 0, sizeof(CodeIp));
	while(num--) {
		LocalQueryOfChar(LocalQuery,"Code",tmp,12);
		if(strcmp(tmp,Getinbuf) == 0) {
			strncpy(CodeIp[0].Code,tmp,12);
			LocalQueryOfChar(LocalQuery,"IP",CodeIp[0].IP,16);
			LocalQueryOfChar(LocalQuery,"Roomname",CodeIp[0].Roomname,32);
			bFirstIndex = TRUE;
			if(Ret >= LISTDISP_NUM-1) {
				break;
			}
		} else if(Ret < LISTDISP_NUM-1) {
			strncpy(CodeIp[Ret+1].Code,tmp,12);
			LocalQueryOfChar(LocalQuery,"IP",CodeIp[Ret+1].IP,16);
			LocalQueryOfChar(LocalQuery,"Roomname",CodeIp[Ret+1].Roomname,32);
			Ret++;
		}
		LocalQuery->Next(LocalQuery);
	}
	LocalQuery->Close(LocalQuery);
	if(Ret>0 || bFirstIndex) {
		IPADDRESS2 *pCodeIp;
		int i;

		if(bFirstIndex) {
			pCodeIp = CodeIp;
			Ret++;
		} else {
			pCodeIp = &CodeIp[1];
		}
		memcpy(tmp,pCodeIp->Code,16);
		strncpy(Public.mCallIP,pCodeIp->IP,16);
		if(strcmp(Public.pCenterMsg->IP,Public.mCallIP)==0) {
			sprintf(tmp,"%s %s",tmp,Word[WORD_HELP_CALL_USER].string);
		} else {
			sprintf(tmp,"%s %s %s",tmp,pCodeIp->Roomname,Word[WORD_HELP_CALL_CENTER].string);
		}
		form_main->listAddItem((WPARAM)tmp);
		pCodeIp++;
		for(i=1; i<Ret; i++) {
			sprintf(tmp,"%s %s",pCodeIp->Code,pCodeIp->Roomname);
			form_main->listAddItem((WPARAM)tmp);
			pCodeIp++;
		}
	}
	form_main->listRefresh();
	if(Ret == 0) {
		if(m_bKeyVoiceprompt && (strncmp(Getinbuf,"0000",4) != 0))	 {
			Public.PlayVoice("RoomNumIsNotExist");
			m_bKeyVoiceprompt = FALSE;
		}
		memset(Public.mCallIP,0,sizeof(Public.mCallIP)); // 20110315 xiehb ���벻��ȷʱ����Ҫ�������
		return FALSE;
	} else {
		m_bKeyVoiceprompt = TRUE;
		return TRUE;
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief callbackLeaveWord �������Իص�����
 *
 * @param Ret MSG_SENDSUCCESS�ɹ�  MSG_SENDTIMEOUT ʧ��
 * @param CallBackData
 */
/* ---------------------------------------------------------------------------*/
static void callbackLeaveWord (int Ret,void *CallBackData)
{
	if(Ret == MSG_SENDSUCCESS) {
		video->bLeaveWord = TRUE;
	} else {
		video->bLeaveWord = FALSE;
		Public.PlayVoice("LeaveMessageFail"); //����ʧ��
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief leaveWordForUser �����Լ�����
 */
/* ---------------------------------------------------------------------------*/
static void leaveWordForUser(void)
{
	if (video->getCallTime(video) > 27) { // ���к��������Ի��������ڻ�����
		return ;
	}
	int status = video->getStatus(video);
	if( status == ST_CALL){
		if(strcmp(video->cPeerIP,Public.pCenterMsg->IP)!=0) {
			char tmp[4]={0};
			sprintf(tmp,"%d",CMD_LEAVEWORD);
			Public.SendUdpTData(video->cPeerIP, UDPSERVER_PORT,
					TP_CALLUSER,
					tmp, 4,
					callbackLeaveWord, NULL);
		} else {
			Public.PlayVoice("CanNotLeaveMessageForCenter"); // ���ܶԹ�����������
		}
	} else {
		if (	   (status != ST_TALK)
				&& (status != ST_MONITOR)
				&& (status != ST_IDLE)	){
			Public.PlayVoice("LeaveMessageAfterCall"); //���к��������
		}
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief keyCalling ��ȷ��������
 */
/* ---------------------------------------------------------------------------*/
static void keyCalling(void)
{
	int len = strlen(Public.mCallIP);
	printf("Check Calling IP : [%s] \n",Public.mCallIP);
	if(len) {
		if(strcmp(Public.mCallIP,Public.cLocalIP)==0) { // �����Լ������Լ�
			return ;
		}
		if(strcmp(Public.pCenterMsg->IP,Public.mCallIP)==0) {	//��������
			Public.PlayVoice("CallCenter");
			fCallObject = FALSE;
		} else {
			Public.PlayVoice("CallUser");
			fCallObject = TRUE;
		}
#if (defined TC3000_BZ)
		video->ST_machine->MsgPost(video->ST_machine,EVENT_SHAKEHANDS,NULL);
		if(fCallObject == FALSE)
#endif
		{
			video->ST_machine->MsgPost(video->ST_machine,EVENT_DIAL,NULL);
		}
		form_main->setKeyboardType(KEY_TYPE_CALLING);
	} else {
		char buf[16];
		if (form_main->getTextNumber(buf)) {
			Public.PlayVoice("RoomNumIsNotExist");
			form_main->setTextDial("");
			form_main->setTextOpration(Word[WORD_PHONE_NUM_DOT_EXIST].string);
		}
		form_main->clearDialNumber();
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief keyHandsUp �һ�
 */
/* ---------------------------------------------------------------------------*/
static void keyHandsUp(void)
{
	if(video->getStatus(video) == ST_IDLE) {
		form_main->clearDialNumber();
	} else {
		data = (STMData *)video->ST_machine->initPara(video->ST_machine,
				sizeof(STMData));
		data->flag = 1;
		video->ST_machine->MsgPost(video->ST_machine,EVENT_HANDS_UP,data);
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief keyboardHandle С���̰����ص�����
 *
 * @param key_value ��ֵ 0-12
 * @param ret ��ֵΪ11ʱ�ķ���ֵ��0ɾ��ʧ�ܣ�1ɾ���ɹ�
 */
/* ---------------------------------------------------------------------------*/
static void keyboardHandle(unsigned int key_value,int ret)
{
#if (defined TC3000_X6)
	if(video->getStatus(video) != ST_IDLE) {
		keyHandsUp();
		return;
	}
	if ((key_value >= 1) && (key_value <= 12)) {
		// ����˳�� �ϵ���
		memcpy(Public.mCallIP,
				Public.HomeList[key_value].IP,
				sizeof(Public.HomeList[key_value].IP));
		// ����˳�� �µ���
		// if (key_value % 2)
			// memcpy(Public.mCallIP,
					// Public.HomeList[12 - key_value].IP,
					// sizeof(Public.HomeList[12 - key_value].IP));
		// else
			// memcpy(Public.mCallIP,
					// Public.HomeList[14 - key_value].IP,
					// sizeof(Public.HomeList[14 - key_value].IP));
		keyCalling();
	} else {
		printf("Err unknow keyval\n");
	}
#else
	static unsigned int enter_unlock_key = 0;
	switch (key_value) {
		case KEYLEAVEWORD :
			{
				enter_unlock_key = 0;
				leaveWordForUser();
			} break;

		case KEYBACK :
			{
				if (ret == 0) {
					enter_unlock_key = 1;
				} else if(ret == 1){
					enter_unlock_key = 0;
					if(Public.IsDownLoadUseInfo){
						getRoomNum();
					}
					
				} else if(ret == 3){
					form_main->createForm(FORM_USERSEARCH,NULL);
					return;
				}
			} break;

		case KEYOK :
			{
				char text_num[16];
				if (form_main->getTextNumber(text_num) == 8) {
					if (strncmp(text_num, "00000000",8) == 0) {
						form_main->clearDialNumber();
						form_main->createForm(FORM_SETTING,NULL);
						return;
					}
				}
				
				if (enter_unlock_key) {
					enter_unlock_key = 0;
					form_main->clearDialNumber();
					if (Public.EnableDoorPassword) {
						form_main->createForm(FORM_UNLOCK,NULL);
					}
					return;
				}
				if(video->getStatus(video) == ST_IDLE) {
					if(!Public.IsDownLoadUseInfo){
						CountIPByRoomNum();
					}
					keyCalling();
				} else {
					keyHandsUp();
				}
			} break;
			
		default :
			enter_unlock_key = 0;
			if(Public.IsDownLoadUseInfo){
				getRoomNum();
			}
			break;

	}
#endif
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief waveInTalkBack ͨ���в���WAV�Ļص�����
 */
/* ---------------------------------------------------------------------------*/
static void waveInTalkBack(void)
{
	if (video->getStatus(video) == ST_TALK) {
		Mixer->InitPlayAndRec(Mixer,video->getFpAudio(video));
	}
	video->setSilence(video,FALSE);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief waveShakeHandsTimeOut ����Э�鳬ʱ�ص�����
 */
/* ---------------------------------------------------------------------------*/
static void waveShakeHandsTimeOut(void)
{
	video->setSilence(video,FALSE);
	voice_call_fail = TRUE;
}


/* ---------------------------------------------------------------------------*/
/**
 * @brief irqTimerInfo 100ms��ȷ��ʱ���������жϷ�ʽ������������
 * ��delay����������������,�������ɳ�����������
 *
 * @param signo
 */
/* ---------------------------------------------------------------------------*/
static void irqTimerInfo(int signo)
{
	static unsigned int TimerCnt=0;
	static struct itimerval timerval;	// ��ʱ����ʱֵ
	// ReSet timer
	timerval.it_value.tv_sec = 0;
	timerval.it_value.tv_usec = MAIN_TIMERVAL;
	timerval.it_interval = timerval.it_value;
	setitimer(ITIMER_REAL, &timerval, NULL);

	if((TimerCnt % 30) == 0) {// 3��
		WatchDogFeed();
	}
	TimerCnt++;
}

static void irqTimerSigaction(void)
{
	sigset_t set;
    struct sigaction action;

    action.sa_handler = irqTimerInfo;
    action.sa_flags   = 0;
    sigemptyset(&set); 	// Get a clean set
	sigaddset(&set, SIGALRM);	// Add SIGALARM
    sigaction(SIGALRM, &action, NULL);
}

static void irqTimerInit(int sec)
{
	static struct itimerval timerval;	// ��ʱ����ʱֵ

	timerval.it_value.tv_sec = 0;
	timerval.it_value.tv_usec = MAIN_TIMERVAL;
	timerval.it_interval = timerval.it_value;
	setitimer(ITIMER_REAL, &timerval, NULL);
}


/* ---------------------------------------------------------------------------*/
/**
 * @brief timer1s ��ʱ��1s
 */
/* ---------------------------------------------------------------------------*/
static void timer1s(void)
{
	static int net_ok = 0; // ���������Ƿ����� 0δ���� 1������
	// �����������״̬
	if (net_detect("eth0") < 0) {
		if (net_ok == 1) {
			net_ok = 0;
			form_main->setNetWorkState(0);
		}
	} else {
		if (net_ok == 0) {
			net_ok = 1;
			form_main->setNetWorkState(1);
		}
	}

	// ������������ʱ��
	if (Public.CenterOnlineTime) {
		if (--Public.CenterOnlineTime == 0) {
			Public.CenterOnline = FALSE;
			form_main->setCenterOnlineState(0);
		} else if (Public.CenterOnline == FALSE) {
			Public.CenterOnline = TRUE;
			form_main->setCenterOnlineState(1);
		}
	}

	// ͨ������ʱ
	int status = video->getStatus(video);
	if(status != ST_IDLE) {
		int ret = video->handleCallTime(video);
		if (ret == 1) {
			// if (status == ST_CALL) {
				// Public.PlayVoice("HandsUpNoAnswer");
			// } else if (status == ST_TALK) {
				// Public.PlayVoice("HandsUpTalkTimeOut");
			// }
		} else if (ret == 2) {
			if ((status == ST_CALL) && fCallObject) {
				Public.PlayVoice("Press0ToLeaveMessage");	
			}
		}
	}

	if (form_main->timerProc1s()) {
		Public.PlayVoice("KeyPressTimeOut");
	}
    // ��ӡ֡��
	// if (seq > 0) {
		// cnt_test++;
		// printf("frame=%d,seq:%d,cnt:%d\n",seq/cnt_test,seq,cnt_test);
	// }
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief timerUpdateTime200ms
 */
/* ---------------------------------------------------------------------------*/
static void timerUpdateTime200ms(void)
{
	//��������ʱ��
	if (form_main->updateClock()) {
		if (Public.SyncTime) {
			Public.syncTime();
		}
	}
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief timer60s ��ʱ��60s ���������ע��
 */
/* ---------------------------------------------------------------------------*/
static void timer60s(void)
{
	Public.RegisteToMng();
	Public.syncTimeHandle60s();
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief timerUnlock500ms ��ʱ��500ms��������ʱ
 */
/* ---------------------------------------------------------------------------*/
static void timerUnlock500ms(void)
{
	// printf("[%s]Unlock the door! end!!!\n",__FUNCTION__);
	form_main->timerStop(IDC_TIMER_UNLOCK);
	Mygpio->SetValueNow(Mygpio,ENUM_GPIO_UNLOCK,IO_INACTIVE);
	form_main->timerStart(IDC_TIMER_UNLOCK_FAIL_10S);
}


/* ---------------------------------------------------------------------------*/
/**
 * @brief timerCallDisp200ms ͨ����ʾ��ʱ��
 *
 * @param hWnd
 */
/* ---------------------------------------------------------------------------*/
static void timerCallDisp200ms(void)
{
	static int PreTime = 0;
	if(video->getCallTime(video) == PreTime) {
		return;
	}
	PreTime = video->getCallTime(video);

	char *title = NULL;
	int status = video->getStatus(video);
	switch (status)
	{
		case ST_CALL:
			{
				title = Word[WORD_WATING].string;
			} break;

		case ST_TALK:
			{
				if (video->bLeaveWord) {
					title = Word[WORD_LEAVE_MESSAGE_ING].string;
				} else {
					title = Word[WORD_LEFT_TIME].string;
				}
			} break;

		case ST_MONITOR:
			{
				title = Word[WORD_MONITOR_LOCAL].string;
			} break;

		default :
			break;
	}

	if (title) {
		char cMsg[64];
		sprintf(cMsg,"%s: %d %s\n",
				title,PreTime,Word[WORD_SECOND].string);
		form_main->setTextOpration(cMsg);
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief timerCallOut2s ��ʱ��2s ������ʱ
 */
/* ---------------------------------------------------------------------------*/
static void timerCallOut2s(void)
{
	form_main->timerStop(IDC_TIMER_CALL_OUT);
	video->handleCallTime(video);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief timerShakeHandsOut10s ��ʱ��10s ������ʱ
 */
/* ---------------------------------------------------------------------------*/
static void timerShakeHandsOut10s(void)
{
	form_main->timerStop(IDC_TIMER_SHAKEHANS_OUT);
	video->handleShakeOutTime(video);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief timerRecover2s ��ʱ��2s ��ʱ�����ָ����ݴ���
 */
/* ---------------------------------------------------------------------------*/
static void timerRecover2s(void)
{
	form_main->createForm(FORM_RECOVER,NULL);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief timerUnlockFail10s ��ʱ��10s ����10sδ��⵽�ŴŴ򿪱���
 */
/* ---------------------------------------------------------------------------*/
static void timerUnlockFail10s(void)
{
	form_main->timerStop(IDC_TIMER_UNLOCK_FAIL_10S);
	// �ϴ��������Ŀ���ʧ��
	Public.SendUnlockRec(0);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief ��ʱ��ִ�к�����˳����Ҫ��ö��һ��
 */
/* ---------------------------------------------------------------------------*/
static FormMainTimers timers_table[] = {
	{timer1s,				10},
	{timer60s,				600},
	{timerCallDisp200ms,	2},
	{timerUnlock500ms,		5},
	{timerRecover2s,		20},
	{timerCallOut2s,		20},
	{timerShakeHandsOut10s,	100},
	{timerUnlockFail10s,	100},
	{timerUpdateTime200ms,	2},
};
/* ---------------------------------------------------------------------------*/
/**
 * @brief mainAppProc �����ڻص�����
 *
 * @param hWnd
 * @param message
 * @param wParam
 * @param lParam
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int mainAppProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_CREATE:
			{
				sqlLoad(&LocalQuery,"DataBase.db");

				LocalQueryExec(LocalQuery,
						"CREATE INDEX index_card on IDCard (CardCode, IP)");
				LocalQueryExec(LocalQuery,
						"CREATE INDEX index_user on UserInfo (Code, IP,Roomname)");
				// ������Ƶ�ӿ�
				Mixer = mixerCreate();
				// ��ʼ����������
				publicInit();
				// ����UDP���
				UdpServer = TUDPServer_Create(hWnd,UDPLOCAL_PORT);
				if(UdpServer == NULL) {
					printf("Create UpdServer fail\n");
				}
				
				// �������жԽ�����
				video = videoTransCreate();
				videoLoadUiFunc(video);
				
				// MP3���Žӿ� ��ͨ�����Ϳ�����ʾ
				PlayMp3 = playMp3Create(Mixer);
				PlayUnlock = playMp3Create(Mixer);
				// ���ڶ���
				uart = uartDealCreate(hWnd);
				uart->ToSingleChip(uart,UART_VERSION);
				if (Public.EnableAlarmDoor)
					uart->ToSingleChip(uart,UART_ALARMDOOR,Public.AlarmDoorTime);
				else 
					uart->ToSingleChip(uart,UART_ALARMDOOR,0);

				if (Public.bUnbrokenUse) 
					uart->ToSingleChip(uart,UART_ENALARM);
				else 
					uart->ToSingleChip(uart,UART_DISALARM);
			
				// ���ض���
				DownData = downloadDataCreate(LocalQuery);
				// ˢ���������
				Card = rfCardCreate(hWnd,LocalQuery);
				// GPIO��������
				Mygpio = myGpioPrivCreate(gpiotbl);
				Mygpio->Init(Mygpio);
				createGpioHandleThread();
				// ��ʼ��������оƬ
				echo_iic_load();
				// ���Ź���ʱ��
				irqTimerSigaction();
				irqTimerInit(MAIN_TIMERVAL);
				WatchDogOpen();
				PlayMp3->Play(PlayMp3,"res/wav/SystemStart.mp3",0,NULL);

				// �����
				/*
				monitor = myMonitorCreate();
				monitor->MonitorStart(Public.VideoType);
				*/
			} return 1;

		case MSG_SOCKETREAD:
			{
				udpSocketRead((SocketHandle*)wParam,(SocketPacket*)lParam);
				free((SocketHandle*)wParam);
				free((SocketPacket*)lParam);
			} return 0;

		case MSG_SERIALPORT:
			{
				uart->FromSingleChip(uart);
			} return 0;

		default:
			return 1;
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief MiniGUIMain ���������
 *
 * @param argc
 * @param argv[]
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
int MiniGUIMain(int argc, const char* argv[])
{
	form_main = formMainCreate(mainAppProc,
			keyboardHandle,
			timers_table);
	// ����Э�鵽��Ƭ����ÿ����һ�����ټ��100ms
	// form_main->timerStart(IDC_TIMER_SEND_CMD_100ms);
	// ������ݿ�������
	etcFileCheck("interface.ini");
	// createCheckInputThread();
	form_main->loop();
	UdpServer->Destroy(UdpServer);
	PlayMp3->Destroy(PlayMp3);
	PlayUnlock->Destroy(PlayUnlock);
	Mygpio->Destroy(Mygpio);
	uart->Destroy(uart);
	Card->Destroy(Card);
	Mixer->Destroy(Mixer);
	LocalQuery->Destroy(LocalQuery);
    publicDestroy();
	WatchDogClose();
    return 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief inputReset ��⸴λ�ź�����
 *
 * @returns 1���� 0δ����
 */
/* ---------------------------------------------------------------------------*/
static int inputReset(void)
{
	static int reset_flag,reset_delay_time;
	reset_flag = Mygpio->IsActive(Mygpio,ENUM_GPIO_RESET);
	if (reset_flag != IO_ACTIVE) {
		reset_delay_time = 0;
		return 0;
	}
	if (reset_delay_time < 5) {
		if (++reset_delay_time == 5) {
			return 1;
		}
	}

	return 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief checkInputHandle ��������ź��߳�ִ�к���
 *
 * @param arg
 */
/* ---------------------------------------------------------------------------*/
static void *checkInputHandle(void *arg)
{
	while (1) {
		if (inputReset()) {
			// uart->ToSingleChip(uart,UART_CARD,1);
			// ExcuteCmd(1,"./reset_config.sh",NULL);
		}
		sleep(1);
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief createCheckInputThread ������������߳�
 */
/* ---------------------------------------------------------------------------*/
static void createCheckInputThread(void)
{
	int result;
	pthread_t m_pthread;					//�̺߳�
	pthread_attr_t threadAttr1;				//�߳�����

	pthread_attr_init(&threadAttr1);		//���Ӳ���
	//�����߳�Ϊ�Զ�����
	pthread_attr_setdetachstate(&threadAttr1,PTHREAD_CREATE_DETACHED);
	//�����̣߳��޴��ݲ���
	result = pthread_create(&m_pthread,&threadAttr1,checkInputHandle,NULL);
	if(result) {
		printf("[%s] pthread failt,Error code:%d\n",__FUNCTION__,result);
	}
	pthread_attr_destroy(&threadAttr1);		//�ͷŸ��Ӳ���
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief gpioHandle Gpio��������50ms����һ��
 *
 * @param arg
 */
/* ---------------------------------------------------------------------------*/
static void * gpioHandle(void *arg)
{
	while (Mygpio != NULL) {
		Mygpio->Handle(Mygpio);
		usleep(50000);// 50ms
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief createGpioHandleThread ����Gpio�����߳�
 */
/* ---------------------------------------------------------------------------*/
static void createGpioHandleThread(void)
{
	int result;
	pthread_t m_pthread;					//�̺߳�
	pthread_attr_t threadAttr1;				//�߳�����
	pthread_attr_init(&threadAttr1);		//���Ӳ���
	//�����߳�Ϊ�Զ�����
	pthread_attr_setdetachstate(&threadAttr1,PTHREAD_CREATE_DETACHED);	
	//�����̣߳��޴��ݲ���
	result = pthread_create(&m_pthread,&threadAttr1,gpioHandle,NULL);		
	if(result) {
		printf("[%s] pthread failt,Error code:%d\n",__FUNCTION__,result);
	}
	pthread_attr_destroy(&threadAttr1);		//�ͷŸ��Ӳ���
}

