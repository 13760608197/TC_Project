/*
 * =============================================================================
 *
 *       Filename:  Rtp.h
 *
 *    Description:  ����ӿ�
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
		
		int fpAudio;	//��Ƶ�����򿪾��
		int bTransVideo;//�Ƿ�����Ƶ
		int bTransAudio;//�Ƿ�����Ƶ
		int silence;	//����ģʽ
		int frame_period;// ֡��
		int Flag;		//�����ǿͻ����ƻ��Ǳ�׼


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
