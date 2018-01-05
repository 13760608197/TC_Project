/* V4L.c
 *
 *
 * Copyright (c)2008 Nuvoton technology corporation
 * http://www.nuvoton.com
 *
 * video for linux function
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <errno.h>
#include <inttypes.h>
#include <linux/fb.h>
#include <signal.h>

#include "Misc.h"
#include "FB_IOCTL.h"
#include "V4L.h"



#define VIDEO_PALETTE_YUV420P_MACRO		50		/* YUV 420 Planar Macro */

#define VID_DEVICE "/dev/video0"

#define VIDIOCGCAPTIME	_IOR('v',30,struct v4l2_buffer)      /*Get Capture time */
#define VIDIOCSPREVIEW                  _IOR('v',43, int)
#define VIDIOC_S_MOTION_THRESHOLD		_IOW('v',50, int)	 /* Set motion detection threshold */

#define VIDEO_START	0
#define VIDEO_STOP	1


typedef struct {
    int32_t i32VidFD;
    int32_t i32FrameFmt; /* see VIDEO_PALETTE_xxx */
    int32_t i32UseMMap;
    int32_t i32FrameSize;
    struct video_window sVidWin;
    uint8_t *pu8VidBuf;
    struct video_mbuf sVidMBufs; // gb_buffers;
    struct video_mmap sVidMMap; //gb_buf;
    int32_t i32VidFrame; //gb_frame;

	int32_t i32DiffOffset;	 
	int32_t i32DiffSize;	
	int32_t i32Thresold;
    uint8_t *pu8MDDiffBuf;	
} S_V4L_VID_DATA;

static S_V4L_VID_DATA s_sVidData = {
	.i32VidFD = -1,
	.i32UseMMap = 0,
	.i32VidFrame = 0,
};

static uint64_t s_u64PrevTS = 0;
static int init_camera = 0;

ERRCODE InitV4LDevice(unsigned char vWidth, unsigned char vHeight)
{
	int i32VidFD;
	ERRCODE err = ERR_V4L_SUCCESS;
	struct video_capability sVidCap;
	struct video_picture sPict;

	i32VidFD = open(VID_DEVICE, O_RDWR);
	if (i32VidFD < 0) {
		ERR_PRINT("Can not open video device %s\n", strerror(errno));
		err = ERR_V4L_OPEN_DEV;
		goto fail;
	}
	if (ioctl(i32VidFD, VIDIOCGCAP, &sVidCap) < 0) {
		DEBUG_PRINT("VIDIOCGCAP: %s\n", strerror(errno));
		err = ERR_V4L_VID_GET_CAP;
		goto fail;
	}

	if (!(sVidCap.type & VID_TYPE_CAPTURE)) {
		DEBUG_PRINT("Fatal: grab device does not handle capture\n");
		err = ERR_V4L_VID_GRAB_DEV;
		goto fail;
	}

	ioctl(i32VidFD, VIDIOCGPICT, &sPict);
	sPict.palette = VIDEO_PALETTE_YUV422;
	sPict.depth= 16;
	if(ioctl(i32VidFD, VIDIOCSPICT, &sPict) < 0) {
		DEBUG_PRINT("VIDIOCSPICT: %s\n", strerror(errno));
		err = ERR_V4L_VID_SET_PICT;
		goto fail;
	}

	//==========================================================================
	// xiehb add 20150325 ÔË¶¯¼ì²â
//	int threshold = 20;
//	if(ioctl(i32VidFD, VIDIOC_S_MOTION_THRESHOLD, &threshold) < 0) {
//		DEBUG_PRINT("VIDIOC_S_MOTION_THRESHOLD: %s\n", strerror(errno));
//		err = ERR_V4L_VID_MOTION_DET;
//		goto fail;
//	}
	//==========================================================================
	if (ioctl(i32VidFD, VIDIOCGMBUF, &s_sVidData.sVidMBufs) < 0) {
		/* try to use read based access */
		int val;

		s_sVidData.sVidWin.width = vWidth;
		s_sVidData.sVidWin.height = vHeight;
		s_sVidData.sVidWin.x = 0;
		s_sVidData.sVidWin.y = 0;
		s_sVidData.sVidWin.chromakey = -1;
		s_sVidData.sVidWin.flags = 0;

		if (ioctl(i32VidFD, VIDIOCSWIN, &s_sVidData.sVidWin) < 0) {
			DEBUG_PRINT("VIDIOCSWIN: %s\n", strerror(errno));
			err = ERR_V4L_VID_SET_WIN;
			goto fail;
		}

		s_sVidData.i32FrameFmt = VIDEO_PALETTE_YUV420P;

		val = 1;
		if (ioctl(i32VidFD, VIDIOCCAPTURE, &val) < 0) {
			DEBUG_PRINT("VIDIOCCAPTURE: %s\n", strerror(errno));
			err = ERR_AVDEV_VID_CAPTURE;
			goto fail;
		}

		s_sVidData.i32UseMMap = 0;
	}
	else {
		s_sVidData.pu8VidBuf = mmap(0, s_sVidData.sVidMBufs.size, PROT_READ|PROT_WRITE, MAP_SHARED, i32VidFD, 0);
		if ((unsigned char*)-1 == s_sVidData.pu8VidBuf) {
			s_sVidData.pu8VidBuf = mmap(0, s_sVidData.sVidMBufs.size, PROT_READ|PROT_WRITE, MAP_PRIVATE, i32VidFD, 0);
			if ((unsigned char*)-1 == s_sVidData.pu8VidBuf) {
				DEBUG_PRINT("mmap: %s\n", strerror(errno));
				err = ERR_V4L_MMAP;
				goto fail;
			}
		}
		s_sVidData.i32VidFrame = 0;

		/* start to grab the first frame */
		s_sVidData.sVidMMap.frame = s_sVidData.i32VidFrame % s_sVidData.sVidMBufs.frames;
		s_sVidData.sVidMMap.height = vHeight;
		s_sVidData.sVidMMap.width = vWidth;
		s_sVidData.sVidMMap.format = VIDEO_PALETTE_YUV420P;

		if (ioctl(i32VidFD, VIDIOCMCAPTURE, &s_sVidData.sVidMMap) < 0) {
			if (errno != EAGAIN) {
				DEBUG_PRINT("VIDIOCMCAPTURE: %s\n", strerror(errno));
			}
			else {
				DEBUG_PRINT("Fatal: grab device does not receive any video signal\n");
			}
			err = ERR_V4L_VID_MCAPTURE;
			goto fail;
		}

		ioctl(i32VidFD,VIDIOC_G_DIFF_OFFSET, &s_sVidData.i32DiffOffset);
		ioctl(i32VidFD,VIDIOC_G_DIFF_SIZE, &s_sVidData.i32DiffSize);
		s_sVidData.pu8MDDiffBuf = s_sVidData.pu8VidBuf + s_sVidData.i32DiffOffset;	

		s_sVidData.i32FrameFmt = s_sVidData.sVidMMap.format;
		s_sVidData.i32UseMMap = 1;
	}
	s_sVidData.i32FrameSize = s_sVidData.sVidMMap.width  * s_sVidData.sVidMMap.height* 2;
	s_sVidData.i32VidFD = i32VidFD;
	return err;
fail:
	if (i32VidFD >= 0)
		close(i32VidFD);
	return err;
}


ERRCODE
ReadV4LPicture(
	uint8_t **ppu8PicPtr,
	uint64_t *pu64TimeStamp,
	uint32_t *pu32BufPhyAddr
)
{
	int32_t i32TryCnt = 0;
	unsigned char *pchVidInBuf;
	struct v4l2_buffer sV4L2Buff;

	*ppu8PicPtr = NULL;

	while (ioctl(s_sVidData.i32VidFD, VIDIOCSYNC, &s_sVidData.i32VidFrame) < 0 &&
		(errno == EAGAIN || errno == EINTR)){
		usleep(10000);
		i32TryCnt ++;
		if(i32TryCnt >= 100){
			ERR_PRINT(": V4L fail\n");
			return ERR_V4L_VID_SYNC;
		}
	}

	sV4L2Buff.index = s_sVidData.i32VidFrame;
	ioctl(s_sVidData.i32VidFD, VIDIOCGCAPTIME, &sV4L2Buff);
	*pu64TimeStamp = (uint64_t)sV4L2Buff.timestamp.tv_sec * 1000000 + (uint64_t)sV4L2Buff.timestamp.tv_usec;
	*pu32BufPhyAddr = sV4L2Buff.m.userptr;	/* encode physical address ---Jack bug?*/

	// if(s_u64PrevTS != 0){
		// if((*pu64TimeStamp - s_u64PrevTS) > 300000)
			// DEBUG_PRINT(": V4L get raw picture over %"PRId64" us\n", (*pu64TimeStamp - s_u64PrevTS));
	// }
	s_u64PrevTS = *pu64TimeStamp;

	pchVidInBuf = s_sVidData.pu8VidBuf + s_sVidData.sVidMBufs.offsets[s_sVidData.i32VidFrame];

	*ppu8PicPtr = pchVidInBuf;


#if defined (STATISTIC)

	if(s_u32VidInFrames == 0){
		s_u32VidInFirstTS = s_u64PrevTS;
	}

	s_u32VidInFrames ++;

	if(s_u32VidInFrames == 300){
		DEBUG_PRINT(": Vin frame rate %"PRId64" fps \n", s_u32VidInFrames/((s_u64PrevTS - s_u32VidInFirstTS)/1000000));
		s_u32VidInFrames = 0;
	}
#endif

	return ERR_V4L_SUCCESS;
}


ERRCODE
TriggerV4LNextFrame(void)
{
	int32_t i32TryCnt = 0;

	while (ioctl(s_sVidData.i32VidFD, VIDIOCSYNC, &s_sVidData.i32VidFrame) < 0 &&
		(errno == EAGAIN || errno == EINTR)){
		usleep(10000);
		i32TryCnt ++;
		if(i32TryCnt >= 100){
			ERR_PRINT("TriggerV4LNextFrame: V4L fail\n");
			return ERR_V4L_VID_SYNC;
		}
	}

	s_sVidData.sVidMMap.frame = s_sVidData.i32VidFrame;
	if (ioctl(s_sVidData.i32VidFD, VIDIOCMCAPTURE, &s_sVidData.sVidMMap) < 0) {
		if (errno == EAGAIN)
			DEBUG_PRINT(": Cannot Sync\n");
		else
			DEBUG_PRINT(": VIDIOCMCAPTURE %s\n", strerror(errno));
		return ERR_V4L_VID_MCAPTURE;
	}

	/* This is now the grabbing frame */
	if(s_sVidData.sVidMBufs.frames > 0)  //add by Jack
		s_sVidData.i32VidFrame = (s_sVidData.i32VidFrame + 1) % s_sVidData.sVidMBufs.frames;

	return ERR_V4L_SUCCESS;
}

ERRCODE
StartV4LCapture(void)
{
	uint32_t u32Capture = VIDEO_START;
//	printf("User Start Capture\n");
	if (ioctl(s_sVidData.i32VidFD, VIDIOCCAPTURE, &u32Capture) < 0) {
		return ERR_V4L_VID_CAPTURE;
	}
	s_sVidData.i32VidFrame = 0;
	return ERR_V4L_SUCCESS;
}

ERRCODE
StopV4LCapture(void)
{
	uint32_t u32Capture = VIDEO_STOP;
	if (ioctl(s_sVidData.i32VidFD, VIDIOCCAPTURE, &u32Capture) < 0) {
		return ERR_V4L_VID_CAPTURE;
	}
	return ERR_V4L_SUCCESS;
}

static ERRCODE StartPreview(void)
{
	if (ioctl(s_sVidData.i32VidFD, VIDIOCSPREVIEW, 1) < 0) {
		return ERR_V4L_VID_CAPTURE;
	}
	return ERR_V4L_SUCCESS;
}

static ERRCODE StopPreview(void)
{
	if (ioctl(s_sVidData.i32VidFD, VIDIOCSPREVIEW, 0) < 0) {
		return ERR_V4L_VID_CAPTURE;
	}
	return ERR_V4L_SUCCESS;
}



static ERRCODE SetV4LViewWindow(int lcmwidth, int lcmheight, int prewidth, int preheight)
{
	ERRCODE err = ERR_V4L_SUCCESS;
	s_sVidData.sVidWin.width = prewidth;
	s_sVidData.sVidWin.height = preheight;
	s_sVidData.sVidWin.x = (lcmwidth-prewidth)/2;		//Depend on the VPOST
	s_sVidData.sVidWin.y = (lcmheight-preheight)/2;
	s_sVidData.sVidWin.chromakey = -1;
	s_sVidData.sVidWin.flags = 0;

	printf("User Packet View Win Width , Height = %d, %d\n", s_sVidData.sVidWin.width, s_sVidData.sVidWin.height);
	printf("User Packet View Win Position X , Y = %d, %d\n", s_sVidData.sVidWin.x, s_sVidData.sVidWin.y);

	if (ioctl(s_sVidData.i32VidFD, VIDIOCSWIN, &s_sVidData.sVidWin) < 0) {
		DEBUG_PRINT("VIDIOCSWIN: %s\n", strerror(errno));
		printf("User VIDIOCSWIN error\n");
		err = ERR_V4L_VID_SET_WIN;
		goto fail;
	}
	return err;
fail:
	if (s_sVidData.i32VidFD >= 0)
		close(s_sVidData.i32VidFD);
	return err;
}

ERRCODE SetV4LEncode(int frame, int width, int height, int palette)
{
	ERRCODE err = ERR_V4L_SUCCESS;
	s_sVidData.sVidMMap.frame = frame; //s_sVidData.i32VidFrame;
	s_sVidData.sVidMMap.format = palette; //VIDEO_PALETTE_YUV422P or VIDEO_PALETTE_YUV420P
	s_sVidData.sVidMMap.height =  height;
	s_sVidData.sVidMMap.width = width;
	s_sVidData.i32FrameSize = s_sVidData.sVidMMap.width  * s_sVidData.sVidMMap.height* 2;
	if (ioctl(s_sVidData.i32VidFD, VIDIOCMCAPTURE, &s_sVidData.sVidMMap) < 0) {
		if (errno == EAGAIN)
			DEBUG_PRINT(": Cannot Sync\n");
		else
			DEBUG_PRINT(": VIDIOCMCAPTURE %s\n", strerror(errno));
		return ERR_V4L_VID_MCAPTURE;
	}
	return err;
}

void FinializeV4LDevice()
{
	if (s_sVidData.i32UseMMap)
		munmap(s_sVidData.pu8VidBuf, s_sVidData.sVidMBufs.size);
	s_sVidData.pu8VidBuf = NULL;
    if (s_sVidData.i32VidFD >= 0)
        close(s_sVidData.i32VidFD);
	s_sVidData.i32VidFD = -1;
}

//------------------------MonitorDet-------------------------------------
ERRCODE	MotionDetV4LSetThreshold(uint32_t u32Threshold)
{
	ERRCODE err = ERR_V4L_SUCCESS;

	if (0 != ioctl (s_sVidData.i32VidFD, VIDIOC_S_MOTION_THRESHOLD, &u32Threshold)) {//Get Diff buffer size
			printf("VIDIOC_S_MOTION_THRESHOLD fail\n");
			return -1; 
	}
	return err;
}

ERRCODE	MotionDetV4LDumpDiffBuffer(void)
{
	
	FILE *pFile;
	uint32_t u32DiffOffset, u32DiffSize;
	int32_t i32TryCnt=0;
	char* pchVidInDiffBuf;

	pFile = fopen("Diff.bin", "wb"); 	
	if (0 != ioctl (s_sVidData.i32VidFD, VIDIOC_G_DIFF_OFFSET, &u32DiffOffset)) {//Get Diff buffer Offset
		printf("VIDIOC_G_DIFF_OFFSET fail\n");
		return -1;
	}	
	if (0 != ioctl (s_sVidData.i32VidFD, VIDIOC_G_DIFF_SIZE, &u32DiffSize)) {//Get Diff buffer size
			printf("VIDIOC_G_DIFF_SIZE fail\n");
			return -1;
	}
	printf("Diff buffer offset = 0x%x\n", u32DiffOffset);
	printf("Diff buffer size = 0x%x\n", u32DiffSize);
	pchVidInDiffBuf = (char *)s_sVidData.pu8VidBuf + u32DiffOffset;	/* Diff Buffer map address */

	while (ioctl(s_sVidData.i32VidFD, VIDIOCSYNC, &s_sVidData.i32VidFrame) < 0 &&
		(errno == EAGAIN || errno == EINTR)){
		usleep(10000);
		i32TryCnt ++;
		if(i32TryCnt >= 100){
			ERR_PRINT(": V4L fail\n");
			return ERR_V4L_VID_SYNC;
		}
	}
	fwrite(pchVidInDiffBuf, 1, u32DiffSize, pFile);
	fclose(pFile);

	return ERR_V4L_SUCCESS;
}

uint32_t MotionDetV4LJudge(void)
{
	int32_t i;
	uint8_t u8Temp;
	uint32_t u32DiffCnt = 0;	
	int32_t i32TryCnt=0;
	uint32_t u32DiffSize;
	struct v4l2_crop sVideoCrop;
	uint32_t u32DiffWidth;
	uint32_t u32DiffHeight;

	while (ioctl(s_sVidData.i32VidFD, VIDIOCSYNC, &s_sVidData.i32VidFrame) < 0 &&
		(errno == EAGAIN || errno == EINTR)){
		usleep(10000);
		i32TryCnt ++;
		if(i32TryCnt >= 100){
			ERR_PRINT(": V4L fail\n");
			return 0;
		}
	}

	ioctl(s_sVidData.i32VidFD, VIDIOC_G_DIFF_SIZE, &u32DiffSize);
	printf("u32DiffSize = %d\n", u32DiffSize);
	for(i = 0; i  < u32DiffSize; i ++){
		u8Temp = *(s_sVidData.pu8MDDiffBuf + i);
		if(u8Temp & 0x80){
			u32DiffCnt ++;
		}
	}
	return u32DiffCnt;
}


//----------------------------------------------------------------------------------------------------------

int camera_init(unsigned int vWidth, unsigned int vHeight)
{
	int i;
	int32_t i32Ret = 0;
	uint8_t *pu8PicPtr;
	uint64_t u64TS;
	uint32_t u32PicPhyAdr;

	if (init_camera == 1) {
		return 0;
	}
	//Init video in
	if(InitV4LDevice(vWidth, vHeight) != ERR_V4L_SUCCESS) {
		i32Ret = -1;
		printf("Init V4L device fail\n");
		goto exit_prog;
	}
	SetV4LEncode(0, vWidth, vHeight, VIDEO_PALETTE_YUV420P_MACRO);
	StartV4LCapture();
    for(i=0;i<2;i++) {   // 10->OK, 4-> xOK, 3-> Fail
    	if(ReadV4LPicture(&pu8PicPtr, &u64TS, &u32PicPhyAdr) == ERR_V4L_SUCCESS) {
            i32Ret = 0;
    		TriggerV4LNextFrame();
        } else {
            i32Ret = -1;
        }
    }
	if (i32Ret == 0)
		init_camera = 1;
exit_prog:
	return i32Ret;

}

//----------------------------------------------------------------------------------------------------------
void camera_close(void)
{
	StopV4LCapture();
	usleep(50000);
	FinializeV4LDevice();
}

