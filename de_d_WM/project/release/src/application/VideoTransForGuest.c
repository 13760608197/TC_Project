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
 *                      variables define
 *----------------------------------------------------------------------------*/
VideoTrans_ForGuest *videoForGuest = NULL;
/* ---------------------------------------------------------------------------*/
/**
 * @brief ReplyVideoDataRequest �ظ��Է���Ƶ��������Ӧ��
 *
 * @param IP �Է�IP
 * @param Port �Է��˿�
 * @param Cmd ��Ӧ������
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int ReplyVideoDataRequest(const char *IP,int Port,int Cmd)
{
	char cBuf[sizeof(COMMUNICATION)+sizeof(REPLYVIDEODATAREQUEST)+4];
	COMMUNICATION * pComm = (COMMUNICATION *)cBuf;
	REPLYVIDEODATAREQUEST *pRevert = (REPLYVIDEODATAREQUEST *)&cBuf[sizeof(COMMUNICATION)];
	memset(cBuf,0,sizeof(COMMUNICATION)+sizeof(REPLYVIDEODATAREQUEST)+4);
	pComm->ID = PacketID++;
	pComm->Size = sizeof(COMMUNICATION)+sizeof(REPLYVIDEODATAREQUEST);
	pComm->Type = TP_REQUESTVIDEO_OPEN;
	strcpy(pRevert->IP,IP);
	pRevert->Port = Port;
	pRevert->Cmd = Cmd;		//0x100 �ɹ�  0x101 ʧ��
	pRevert->VideoWidth = h264_type[Public.VideoType].img_width;
	pRevert->VideoHeight = h264_type[Public.VideoType].img_height;

	UdpServer->AddTask(UdpServer,IP,Port,cBuf,pComm->Size,4,0,NULL,NULL);

	return 0;
}

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
int videoTransNewInterface(VideoTrans_ForGuest * this,VideoTransNeedData * NeedData)
{
//	printf("[%s]-------------------Begin  IP:[%s]&Port:[%d]--------------\n",__FUNCTION__,NeedData->IP,NeedData->VideoPort);
	const char *IP = NeedData->IP;
	int CmdPort= NeedData->CmdPort;
	int VideoPort = NeedData->VideoPort;
	this->m_CodeType = NeedData->VideoType;
	//���öԷ�IP������IP�Ͷ˿ںŷ��ͻظ����Է�
	strcpy(this->cPeerIP,IP);
	this->PeerPort = VideoPort;
//	printf("[%s]-------------------ret init----------------\n",__FUNCTION__);
	if(this->rtp->init(this->rtp,
				this->cPeerIP,
				Public.cLocalIP,
				Public.cInternetIP,
				this->PeerPort,1) != CALL_OK) {
		ReplyVideoDataRequest(IP,CmdPort,CMD_REPLY_FAIL);
		return CALL_FAIL;
	}
//	printf("[%s]-------------------ret buildConnect----------------\n",__FUNCTION__);
	//ֻ������Ƶ����,��������Ƶ����
	ReplyVideoDataRequest(IP,CmdPort,CMD_REPLY_OK);
	this->rtp->bTransVideo = TRUE;
	this->rtp->Flag = 1;
	this->rtp->buildConnect(this->rtp,Public.FramPeriod);

//	printf("[%s]-------------------End----------------\n",__FUNCTION__);	
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
void videoTransNewInterface_Close(VideoTrans_ForGuest * this,VideoTransNeedData * NeedData)
{
	printf("[%s]-------------------Begin Close VideoTrans --------------\n",__FUNCTION__);
	this->rtp->Flag = 1;
	this->rtp->close(this->rtp);
	const char * IP = NeedData->IP;
	int CmdPort = NeedData->CmdPort;
	printf("[%s]------------IP : [%s] , CmdPort : [%d]------------------\n",__FUNCTION__,IP,CmdPort);
	ReplyVideoDataRequest(IP,CmdPort,CMD_REPLY_HUNGUP);		//��ͻ���Ƶ�ӿڻظ��Ҷ�
	printf("[%s]-------------------End----------------\n",__FUNCTION__);	
	
	return ;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransDestroy ���ٶ����˳�����
 *
 * @param this
 */
/* ---------------------------------------------------------------------------*/
static void videoTransDestroyForGuest(VideoTrans_ForGuest * this)
{
	this->rtp->Destroy(&this->rtp);
	free(this);
}


/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransCreate �������жԽ�����
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
VideoTrans_ForGuest * videoTransCreateForGuest(void) {
	VideoTrans_ForGuest * this = (VideoTrans_ForGuest *)malloc(sizeof(VideoTrans_ForGuest));
	if(this==NULL) {
		ErrorLog(0x7FFFFFF,"���������ڴ�:�����ڴ�ʧ��!");
		return NULL;
	}
	memset(this,0,sizeof(VideoTrans_ForGuest));

	this->dwInCameraWidth = h264_type[Public.VideoType].img_width;
	this->dwInCameraHeight = h264_type[Public.VideoType].img_height;
	this->rtp = createRtp(Public.VideoType,NULL,this);

	this->Destroy = videoTransDestroyForGuest;
	return this;
}

