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
 * @brief ReplyVideoDataRequest 回复对方视频数据请求应答
 *
 * @param IP 对方IP
 * @param Port 对方端口
 * @param Cmd 回应的命令
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
	pRevert->Cmd = Cmd;		//0x100 成功  0x101 失败
	pRevert->VideoWidth = h264_type[Public.VideoType].img_width;
	pRevert->VideoHeight = h264_type[Public.VideoType].img_height;

	UdpServer->AddTask(UdpServer,IP,Port,cBuf,pComm->Size,4,0,NULL,NULL);

	return 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransNewInterface 新增视频获取接口
 *
 *
 * @param this
 * @param IP 对方IP
 * @param Port 对方端口
 * @param pCall 相关参数
 *
 * @returns
// 如果成功，返回非0,失败返回0
 */
/* ---------------------------------------------------------------------------*/
int videoTransNewInterface(VideoTrans_ForGuest * this,VideoTransNeedData * NeedData)
{
//	printf("[%s]-------------------Begin  IP:[%s]&Port:[%d]--------------\n",__FUNCTION__,NeedData->IP,NeedData->VideoPort);
	const char *IP = NeedData->IP;
	int CmdPort= NeedData->CmdPort;
	int VideoPort = NeedData->VideoPort;
	this->m_CodeType = NeedData->VideoType;
	//设置对方IP，根据IP和端口号发送回复给对方
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
	//只传输视频数据,不传输音频数据
	ReplyVideoDataRequest(IP,CmdPort,CMD_REPLY_OK);
	this->rtp->bTransVideo = TRUE;
	this->rtp->Flag = 1;
	this->rtp->buildConnect(this->rtp,Public.FramPeriod);

//	printf("[%s]-------------------End----------------\n",__FUNCTION__);	
	return CALL_OK;
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransNewInterface_Close 新增停止视频获取接口
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
	ReplyVideoDataRequest(IP,CmdPort,CMD_REPLY_HUNGUP);		//向客户视频接口回复挂断
	printf("[%s]-------------------End----------------\n",__FUNCTION__);	
	
	return ;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief videoTransDestroy 销毁对象，退出程序
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
 * @brief videoTransCreate 创建呼叫对讲对象
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
VideoTrans_ForGuest * videoTransCreateForGuest(void) {
	VideoTrans_ForGuest * this = (VideoTrans_ForGuest *)malloc(sizeof(VideoTrans_ForGuest));
	if(this==NULL) {
		ErrorLog(0x7FFFFFF,"创建共享内存:分配内存失败!");
		return NULL;
	}
	memset(this,0,sizeof(VideoTrans_ForGuest));

	this->dwInCameraWidth = h264_type[Public.VideoType].img_width;
	this->dwInCameraHeight = h264_type[Public.VideoType].img_height;
	this->rtp = createRtp(Public.VideoType,NULL,this);

	this->Destroy = videoTransDestroyForGuest;
	return this;
}

