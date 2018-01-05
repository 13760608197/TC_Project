/*
 * =====================================================================================
 *
 *       Filename:  VideoTrans.h
 *
 *    Description:  呼叫对讲协议
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
	//正在拨号
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
		EVENT_SHAKEHANDS, 	//主动呼叫发送握手
		EVENT_SHAKEHANDS_ASW, 	//对方呼叫返回握手
		EVENT_TYPE_CENTER, 	//类型管理中心
		EVENT_TYPE_HM, 		//类型室内机
		EVENT_DIAL,        	//拨号
		EVENT_CALL,        	//呼叫
		EVENT_TALK,        	//通话
		EVENT_MONITOR,    	//监视
		EVENT_FAIL_COMM,    //失败:连接
		EVENT_FAIL_SHAKE,   //失败:握手
		EVENT_FAIL_BUSY,   	//失败:正忙
		EVENT_FAIL_ABORT,   //失败:通话中中断
		EVENT_HANDS_UP, 	//挂机

	}VideoEvent;

	typedef enum _VideoStatus {
		ST_IDLE,			//空闲
		ST_SHAKEHANDS,		//主动发送握手状态
		ST_SHAKEHANDS_ASW,	//对方呼叫已返回握手状态
		ST_JUDGE_TYPE,		//判断设备类型状态3000还是U9
		ST_DIAL,			//正在拨号
		ST_CALL,			//呼叫
		ST_TALK,			//对讲
		ST_MONITOR,			//远程监视
	}VideoStatus;

	typedef enum _VideoDo{
		DO_NO,   			//不做操作，只改变状态
		DO_SHAKEHANDS,   	//发送握手命令
		DO_SHAKEHANDS_ASW, 	//回复握手命令
		DO_JUDGE_TYPE, 		//监视时判断类型
		DO_DIAL,          	//发送拨号命令
		DO_CALL,    		//发送呼叫命令
		DO_TALK,        	//执行对讲流程
		DO_MONITOR,        	//执行监视流程
		DO_FAIL_COMM,    	//执行连接失败
		DO_FAIL_SHAKE,    	//执行握手失败
		DO_FAIL_BUSY,       //执行正忙失败
		DO_FAIL_ABORT,   	//执行通话中中断
		DO_RET_FAIL,       	//回复失败
		DO_HANDS_UP, 		//执行挂机命令
	}VideoDo;



	typedef struct _VideoTrans
	{
		struct _State_machine *ST_machine; 	// 状态机处理
		struct _Rtp * rtp; 					// rtp传输处理
		int  CallTimeout;					//对讲超时
		int dwPeerType;						//对讲方的设备类型
		int m_CodeType;						//视频编码类型
		int dwInCameraWidth;				//呼入Camera的大小
		int dwInCameraHeight;				//呼入Camera的大小
		char cPeerRoom[32];					//对方名称
		char cPeerID[16];					//对方ID
		char cPeerIP[16];					//对方IP
		int PeerPort;						//对方端口
		char cDate[20];						//呼叫时间
		BOOL bLeaveWord;					//是否在留言状态
		
		void (*setSilence)(struct _VideoTrans *this,int value);
		int (*getSilence)(struct _VideoTrans *this);
		int (*getStatus)(struct _VideoTrans *this);
		int (*getCallTime)(struct _VideoTrans *this);
		int *(*getFpAudio)(struct _VideoTrans *this);
		int (*handleCallTime)(struct _VideoTrans *this);
		int (*handleCallOutTime)(struct _VideoTrans *this);
		int (*handleShakeOutTime)(struct _VideoTrans *this);
		int (*shakeHandsRet)(struct _VideoTrans *this,const char *IP,int Port);	//呼叫IP前的握手命令返回
		void (*udpCallHandle)(struct _VideoTrans *this,
				struct _SocketHandle *ABinding,
				struct _SocketPacket *AData);	//呼叫对讲协议处理
		void (*Destroy)(struct _VideoTrans *this);				//销毁

		void (*uiShakeHands)(void);//界面显示
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
		unsigned	char	IP[16];			//对方IP
		unsigned	int 	Port;			//对方端口
		unsigned 	int 	IsTransVideo;	//开启视频传输
		unsigned	int 	VideoType;		//视频类型: 0 H264, 1 Mpeg4
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
				struct _SocketPacket *AData);	//呼叫对讲协议处理
	}UdpCmd;

	VideoTrans * videoTransCreate(void);
	extern VideoTrans *video;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
