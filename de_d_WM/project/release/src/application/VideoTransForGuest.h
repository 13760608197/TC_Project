/*
 * =====================================================================================
 *
 *       Filename:  VideoTransForGuest.h
 *
 *    Description:  为客户定制的视频API
 *
 *        Version:  1.0
 *        Created:  2017-07-21
 *       Revision:  1.0
 *
 *         Author:  LEE
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _VIDEO_TRANS_ForGeust_H
#define _VIDEO_TRANS_ForGeust_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifndef BOOL
#define BOOL int
#define TRUE 1
#define FALSE 0
#endif


	typedef struct _VideoTrans_ForGuest
	{
		struct _Rtp * rtp; 					// rtp传输处理
		int m_CodeType;						//视频编码类型
		int dwInCameraWidth;				//呼入Camera的大小
		int dwInCameraHeight;				//呼入Camera的大小
		char cPeerIP[16];					//对方IP
		int PeerPort;						//对方端口
		
		void (*Destroy)(struct _VideoTrans_ForGuest *this);				//销毁

	}VideoTrans_ForGuest;

	typedef struct{
		unsigned	char	IP[16];			//对方IP
		unsigned	int 	CmdPort;		//指令接收端口
		unsigned	int 	VideoPort;		//视频输出端口
		unsigned	int 	VideoType;		//视频类型: 0 H264, 1 Mpeg4
	}VideoTransNeedData;

	VideoTrans_ForGuest * videoTransCreateForGuest(void);
	extern VideoTrans_ForGuest *videoForGuest;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
