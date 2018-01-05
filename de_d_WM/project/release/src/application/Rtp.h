/*
 * =============================================================================
 *
 *       Filename:  Rtp.h
 *
 *    Description:  传输接口
 *
 *        Version:  1.0
 *        Created:  2016-03-01 14:28:41 
 *       Revision:  1.0
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * ============================================================================
 */
#ifndef _RTP_H
#define _RTP_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


	typedef struct _Rtp {
		
		int fpAudio;	//音频驱动打开句柄
		int bTransVideo;//是否传输视频
		int bTransAudio;//是否传输音频
		int silence;	//静音模式
		int frame_period;// 帧率
		int Flag;		//区分是客户定制还是标准


		int *(*getFpAudio)(struct _Rtp *this);
		void (*setPeerIp)(struct _Rtp *this,char *ip);
		void (*setSilence)(struct _Rtp *this,int value);
		int (*getSilence)(struct _Rtp *this);
		int (*init)(struct _Rtp *this,
				const char *dest_ip, char *local_ip, char *server_ip, int Port,int Flag);
		void (*buildConnect)(struct _Rtp *this,int frame_period); 
		void (*startAudio)(struct _Rtp *this);
		void (*close)(struct _Rtp *this);
		void (*Destroy)(struct _Rtp **this);
	} Rtp;

	typedef struct{
		Rtp * rtp;
		int Flag;
	}SpiThreadParam;
	
	Rtp * createRtp(int video_type,
			void (*abortCallBack)(void *),void *callBackData);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
