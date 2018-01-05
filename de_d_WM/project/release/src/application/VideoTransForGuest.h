/*
 * =====================================================================================
 *
 *       Filename:  VideoTransForGuest.h
 *
 *    Description:  Ϊ�ͻ����Ƶ���ƵAPI
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
		struct _Rtp * rtp; 					// rtp���䴦��
		int m_CodeType;						//��Ƶ��������
		int dwInCameraWidth;				//����Camera�Ĵ�С
		int dwInCameraHeight;				//����Camera�Ĵ�С
		char cPeerIP[16];					//�Է�IP
		int PeerPort;						//�Է��˿�
		
		void (*Destroy)(struct _VideoTrans_ForGuest *this);				//����

	}VideoTrans_ForGuest;

	typedef struct{
		unsigned	char	IP[16];			//�Է�IP
		unsigned	int 	CmdPort;		//ָ����ն˿�
		unsigned	int 	VideoPort;		//��Ƶ����˿�
		unsigned	int 	VideoType;		//��Ƶ����: 0 H264, 1 Mpeg4
	}VideoTransNeedData;

	VideoTrans_ForGuest * videoTransCreateForGuest(void);
	extern VideoTrans_ForGuest *videoForGuest;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
