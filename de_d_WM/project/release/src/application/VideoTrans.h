/*
 * =====================================================================================
 *
 *       Filename:  VideoTrans.h
 *
 *    Description:  ���жԽ�Э��
 *
 *        Version:  1.0
 *        Created:  2016-02-26 10:30:43 
 *       Revision:  1.0
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _VIDEO_TRANS_H
#define _VIDEO_TRANS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define CALL_OK					0
#define CALL_NOHOST				-1
#define CALL_FAIL				-2
#define CALL_BUSY				-3
	//���ڲ���
#define CALL_WAIT				-4

#ifndef BOOL
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif

#define MSG_CALLMSG MSG_USER+850
#define MSG_MONITOR MSG_USER+323

#define LEAVE_WORD_TIME	30

	struct _COMMUNICATION_CALL;

	typedef enum _VideoEvent{
		EVENT_SHAKEHANDS, 	//�������з�������
		EVENT_SHAKEHANDS_ASW, 	//�Է����з�������
		EVENT_TYPE_CENTER, 	//���͹�������
		EVENT_TYPE_HM, 		//�������ڻ�
		EVENT_DIAL,        	//����
		EVENT_CALL,        	//����
		EVENT_TALK,        	//ͨ��
		EVENT_MONITOR,    	//����
		EVENT_FAIL_COMM,    //ʧ��:����
		EVENT_FAIL_SHAKE,   //ʧ��:����
		EVENT_FAIL_BUSY,   	//ʧ��:��æ
		EVENT_FAIL_ABORT,   //ʧ��:ͨ�����ж�
		EVENT_HANDS_UP, 	//�һ�

	}VideoEvent;

	typedef enum _VideoStatus {
		ST_IDLE,			//����
		ST_SHAKEHANDS,		//������������״̬
		ST_SHAKEHANDS_ASW,	//�Է������ѷ�������״̬
		ST_JUDGE_TYPE,		//�ж��豸����״̬3000����U9
		ST_DIAL,			//���ڲ���
		ST_CALL,			//����
		ST_TALK,			//�Խ�
		ST_MONITOR,			//Զ�̼���
	}VideoStatus;

	typedef enum _VideoDo{
		DO_NO,   			//����������ֻ�ı�״̬
		DO_SHAKEHANDS,   	//������������
		DO_SHAKEHANDS_ASW, 	//�ظ���������
		DO_JUDGE_TYPE, 		//����ʱ�ж�����
		DO_DIAL,          	//���Ͳ�������
		DO_CALL,    		//���ͺ�������
		DO_TALK,        	//ִ�жԽ�����
		DO_MONITOR,        	//ִ�м�������
		DO_FAIL_COMM,    	//ִ������ʧ��
		DO_FAIL_SHAKE,    	//ִ������ʧ��
		DO_FAIL_BUSY,       //ִ����æʧ��
		DO_FAIL_ABORT,   	//ִ��ͨ�����ж�
		DO_RET_FAIL,       	//�ظ�ʧ��
		DO_HANDS_UP, 		//ִ�йһ�����
	}VideoDo;



	typedef struct _VideoTrans
	{
		struct _State_machine *ST_machine; 	// ״̬������
		struct _Rtp * rtp; 					// rtp���䴦��
		int  CallTimeout;					//�Խ���ʱ
		int dwPeerType;						//�Խ������豸����
		int m_CodeType;						//��Ƶ��������
		int dwInCameraWidth;				//����Camera�Ĵ�С
		int dwInCameraHeight;				//����Camera�Ĵ�С
		char cPeerRoom[32];					//�Է�����
		char cPeerID[16];					//�Է�ID
		char cPeerIP[16];					//�Է�IP
		int PeerPort;						//�Է��˿�
		char cDate[20];						//����ʱ��
		BOOL bLeaveWord;					//�Ƿ�������״̬
		
		void (*setSilence)(struct _VideoTrans *this,int value);
		int (*getSilence)(struct _VideoTrans *this);
		int (*getStatus)(struct _VideoTrans *this);
		int (*getCallTime)(struct _VideoTrans *this);
		int *(*getFpAudio)(struct _VideoTrans *this);
		int (*handleCallTime)(struct _VideoTrans *this);
		int (*handleCallOutTime)(struct _VideoTrans *this);
		int (*handleShakeOutTime)(struct _VideoTrans *this);
		int (*shakeHandsRet)(struct _VideoTrans *this,const char *IP,int Port);	//����IPǰ�����������
		void (*udpCallHandle)(struct _VideoTrans *this,
				struct _SocketHandle *ABinding,
				struct _SocketPacket *AData);	//���жԽ�Э�鴦��
		void (*Destroy)(struct _VideoTrans *this);				//����

		void (*uiShakeHands)(void);//������ʾ
		void (*uiCallIP)(void);
		void (*uiRetCall)(void);
		void (*uiFailComm)(void);
		void (*uiFailShakeHands)(void);
		void (*uiFailBusy)(void);
		void (*uiFailAbort)(void);
		void (*uiMonitor)(void);
		void (*uiAnswer)(void);
		void (*uiOver)(void);

	}VideoTrans;

	typedef struct _STMData {
		int flag;
		struct _SocketHandle abd;
		struct _COMMUNICATION_CALL p_call;
		struct _VideoTrans *video;
	}STMData;
/*
	typedef struct{
		unsigned	char	IP[16];			//�Է�IP
		unsigned	int 	Port;			//�Է��˿�
		unsigned 	int 	IsTransVideo;	//������Ƶ����
		unsigned	int 	VideoType;		//��Ƶ����: 0 H264, 1 Mpeg4
	}VideoTransNeedData;
*/

	typedef struct _STMDo {
		int action;
		int (*proc)(struct _VideoTrans *video,STMData *data);
	}STMDo;

	typedef struct _UdpCmd {
		unsigned int cmd;
		void (*proc)(struct _VideoTrans *this,
				struct _SocketHandle *ABinding,
				struct _SocketPacket *AData);	//���жԽ�Э�鴦��
	}UdpCmd;

	VideoTrans * videoTransCreate(void);
	extern VideoTrans *video;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
