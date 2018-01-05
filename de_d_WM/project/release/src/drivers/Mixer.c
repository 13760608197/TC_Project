/*
 * =====================================================================================
 *
 *       Filename:  Mixer.c
 *
 *    Description:  混音器接口处理
 *
 *        Version:  1.0
 *        Created:  2015-12-21 15:28:18
 *       Revision:  1.0
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
/* ----------------------------------------------------------------*
 *                      include head files
 *-----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>

#include <sys/types.h>
#include <sys/poll.h>
#include <pthread.h>
#include <math.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

#include "externfunc.h"

/* ----------------------------------------------------------------*
 *                  extern variables declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                  internal functions declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                        macro define
 *-----------------------------------------------------------------*/
#if DBG_MYMIXER > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

#define MAXMIXER 3
typedef struct _TMixerPriv
{
	int Inited;					//是否初始化成功

	int audiofp;				//打开声卡驱动  放音
	int mixer_fd;				//混音器句柄    喇叭

	int audiofp1;				//打开声卡驱动  录音
	int mixer_fd1;				//混音器句柄	咪头

	int CurrSample;				//当前采样率
	int oss_format;				//standard 16bit little endian format, support this format only
	unsigned int MinVolume;		//最小音量
	unsigned int MaxVolume;		//最大音量
	unsigned int MicVolume;		//MIC音量
	unsigned int PlayVolume;	//播放音量
	int bSlience;				//是否静音 1静音 0非静音
}TMixerPriv;

/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
TMixer *Mixer = NULL;

/* ----------------------------------------------------------------*/
/**
 * @brief mixerChangeSample 修改放音采样率
 *
 * @param this
 * @param Sample 采样率
 *
 * @returns 修改后当前采样率
 */
/* ----------------------------------------------------------------*/
static int mixerChangeSample(struct _TMixer *this,int Sample)
{

	DBG_P("Change sample :%d-->sample :%d\n",this->Priv->CurrSample,Sample);
	if(this->Priv->CurrSample == Sample) {
		return this->Priv->CurrSample;
	}

	if(Sample <= FMT8K) {
		this->Priv->CurrSample = FMT8K;
	} else if(Sample >= FMT44K) {
		this->Priv->CurrSample = FMT44K;
	} else {
		this->Priv->CurrSample = Sample;
	}

	if (ioctl(this->Priv->audiofp, SNDCTL_DSP_SPEED, &Sample) < 0) {
		fprintf(stdout,"Sample:%d Error: %s\n",Sample,strerror(errno));
		return 0;
	}

	return this->Priv->CurrSample;
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerOpen 打开放音声卡
 *
 * @param this
 * @param Sample 设置采样率
 * @param CH 设置通道
 *
 * @returns 0失败
 */
/* ----------------------------------------------------------------*/
static int mixerOpen(TMixer *this,int Sample,int CH)
{
	int Setting;
	static int err_times ;
	if (this->Priv->audiofp > 0) {
		return 0;
	}
	err_times = 0;
	//打开声卡放音句柄 确保视频文件停止播放
	while ((this->Priv->audiofp = open("/dev/dsp",O_WRONLY,0)) < 0) {
		usleep(50000);
			fprintf(stdout,"/dev/dsp %s \n",strerror(errno));
		// 打开50次仍失败，则返回
		if (++err_times > 50) {
			fprintf(stdout,"/dev/dsp 50 timse: %s \n",strerror(errno));
			return 0;
		}
	}

	Setting = (5<<16) + 10; // 2^12 = 4096;
	if (ioctl(this->Priv->audiofp, SNDCTL_DSP_SETFRAGMENT, &Setting) < 0) {
		perror("SetFragment");
		return 0;
	}
	if (ioctl(this->Priv->audiofp,SNDCTL_DSP_SETFMT, &this->Priv->oss_format) < 0) {
		perror("OOS_format");
		return 0;
	}
	//打开放音混音器句柄
	if ((this->Priv->mixer_fd = open("/dev/mixer", O_RDWR)) < 0) {
		perror("/dev/mixer");
		return 0;
	}
	if (ioctl(this->Priv->audiofp, SNDCTL_DSP_CHANNELS, &CH) < 0) {
		perror("Channel");
		return 0;
	}

	if (mixerChangeSample(this,Sample)) {
		this->Priv->Inited = 1;
		return this->Priv->audiofp;
	}
	return 0;
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerClose 关闭放音设备
 *
 * @param this
 * @param Handle 设备句柄
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
static int mixerClose(TMixer *this,int *Handle)
{
	if (this->Priv->audiofp > 0)
		close(this->Priv->audiofp);
	if (this->Priv->mixer_fd > 0)
		close(this->Priv->mixer_fd);

	this->Priv->CurrSample = 0;
	this->Priv->Inited = 0;
	*Handle = this->Priv->audiofp = -1;
	return 0;
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerRead 从声卡读声音，录音
 *
 * @param this
 * @param pBuffer 保存录音数据
 * @param Size 一次读取字节大小
 *
 * @returns 实际读取字节数据
 */
/* ----------------------------------------------------------------*/
static int mixerRead(TMixer *this,void *pBuffer,int Size)
{
	if (this->Priv->audiofp1 != -1) {
		int k = read(this->Priv->audiofp1,pBuffer,Size);
		return k;
	}

	return 0;
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerReadBuffer 从声卡读声音，录音
 *
 * @param this
 * @param AudioBuf 保存录音数据
 * @param NeedSize 一次读取字节大小
 *
 * @returns 实际读取字节数据
 */
/* ----------------------------------------------------------------*/
static int mixerReadBuffer(TMixer *this, void *AudioBuf, int NeedSize)
{
	int RealSize = 0;
	audio_buf_info info;
	if (this->Priv->audiofp1 == -1) {
		return RealSize;
	}
	ioctl(this->Priv->audiofp1 , SNDCTL_DSP_GETISPACE , &info);
	if(info.fragments > 0) {
		RealSize = this->Read(this,AudioBuf,NeedSize);
		//	printf("info:RealSize:%d,bytes=%d,fragments=%d\n",RealSize, info.bytes, info.fragments);
	}

	return RealSize;
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerWrite 写声音缓存
 *
 * @param this
 * @param Handle 通道
 * @param pBuffer 数据
 * @param Size 大小
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
static int mixerWrite(TMixer *this,int Handle,const void *pBuffer,int Size)
{
	DBG_P("[%s]\n",__FUNCTION__);
	if(this->Priv->Inited == 0 ) {
		return 0;
	}

	int NewSize;
	if(Handle<1 && Handle>MAXMIXER)
		return 0;
	if(Size<10)
		return Size;
	if(this->Priv->bSlience) {
		DelayMs(Size/30);
		return Size;
	}
	NewSize = write(this->Priv->audiofp,pBuffer,Size);
	return NewSize;
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerWriteBuffer 写声音缓存块
 *
 * @param this
 * @param Handle 通道
 * @param pBuffer 数据
 * @param Size 大小
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
static int mixerWriteBuffer(TMixer *this,int Handle,const void *pBuffer,int Size)
{
	DBG_P("[%s]\n",__FUNCTION__);
	const char *pBuf = (const char*)pBuffer;
	int LeaveSize = Size;

	while(LeaveSize) {
		int WriteSize = this->Write(this,Handle,pBuf,LeaveSize);
		if(WriteSize <= 0) {
			break;
		}
		pBuf += WriteSize;
		LeaveSize -= WriteSize;
	}
	return Size;
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerGetVolume 获得音量
 *
 * @param this
 * @param type 音量类型 0放音 1录音
 *
 * @returns 音量值
 */
/* ----------------------------------------------------------------*/
static int mixerGetVolume(struct _TMixer *this,int type)
{
	if (type) {
		return this->Priv->MicVolume;
	} else {
		return this->Priv->PlayVolume;
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerSetVolume 设置音量
 *
 * @param this
 * @param Volume 音量
 * @param type 混音器类型 0放音 1录音
 *
 * @returns 设置结果
 */
/* ----------------------------------------------------------------*/
static int mixerSetVolume(struct _TMixer *this,int type,int Volume)
{
	int Value,buf;

	if(Volume<0) {
		Volume = 0;
		DBG_P("Set volume value %d wrong,set zero\n",Volume);
	}
	if(Volume>100) {
		Volume = 100;
		DBG_P("Set volume value %d wrong,set 100\n",Volume);
	}
	Value = this->Priv->MinVolume + Volume*(this->Priv->MaxVolume-this->Priv->MinVolume)/100;
	DBG_P("Volume Value:%d\n",Value);

	buf = Value;
	Value <<= 8;
	Value |= buf;

	if (type) {
		if( -1 == ioctl(this->Priv->mixer_fd1 , MIXER_WRITE(SOUND_MIXER_PCM), &Value)) {
			fprintf(stdout,"Set Volume %d ,%s\n", Value,strerror(errno));
			return -2;
		}
		this->Priv->PlayVolume = Volume;
	} else {
		if( -1 == ioctl(this->Priv->mixer_fd , MIXER_WRITE(SOUND_MIXER_PCM), &Value)) {
			fprintf(stdout,"Set Volume %d ,%s\n", Value,strerror(errno));
			return -2;
		}
		this->Priv->MicVolume = Volume;
	}

	return 0;
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerSetVolumeEx 通过外部应用程序设置音量
 *
 * @param this
 * @param Volume 音量
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
static int mixerSetVolumeEx(struct _TMixer *this,int Volume)
{
	char buf[6];
	if (Volume > MAX_VOLUME_EX) {
		sprintf(buf, "%d%%",MAX_VOLUME_EX);
	} else {
		sprintf(buf, "%d%%",Volume);
	}
	ExcuteCmd(0,"./amixer","cset","numid=1",buf,buf,NULL);
	return 0;
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerInitVolume 初始化音量
 *
 * @param this
 * @param Volume 音量
 * @param bSlience 0非静音 1静音
 *
 */
/* ----------------------------------------------------------------*/
static void mixerInitVolume(struct _TMixer *this,int Volume,int bSlience)
{
	this->SetVolume(this,Volume,1);
	this->Priv->bSlience = bSlience;
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerSetSlience 设置静音
 *
 * @param this
 * @param bSlience 1静音，0非静音
 */
/* ----------------------------------------------------------------*/

static void mixerSetSlience(struct _TMixer *this,int bSlience)
{
	this->Priv->bSlience = bSlience;
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerGetSlience 返回静音状态
 *
 * @param this
 *
 * @returns  1静音，0非静音
 */
/* ----------------------------------------------------------------*/
static int mixerGetSlience(struct _TMixer *this)
{
	return this->Priv->bSlience;
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerClearRecBuffer 清除声音录音缓冲区
 *
 * @param this
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
static void mixerClearRecBuffer(TMixer *this)
{
	int i;
	int buffersize;
	char *pBuf;
	audio_buf_info info;
	if (this->Priv->audiofp1 == -1) {
		return;
	}
	ioctl(this->Priv->audiofp1, SNDCTL_DSP_GETBLKSIZE, &buffersize);
	ioctl(this->Priv->audiofp1 , SNDCTL_DSP_GETISPACE , &info);
	pBuf = (char *)malloc(buffersize );
	if(pBuf && buffersize > 0) {
		for(i=0; i<info.fragstotal; i++) {
			memset(pBuf,0,buffersize);
			//	printf("ClearRecBuffer[%d]Rec buffersize :%d\n", i,buffersize);
			this->Read(this,pBuf,buffersize);
		}
	}
	free(pBuf);

}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerClearPlayBuffer 清除声音放音缓冲区
 *
 * @param this
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
static void mixerClearPlayBuffer(TMixer *this)
{
	int buffersize,i;
	char *pBuf;
	audio_buf_info info;
	ioctl(this->Priv->audiofp, SNDCTL_DSP_GETBLKSIZE, &buffersize);
	ioctl(this->Priv->audiofp , SNDCTL_DSP_GETOSPACE , &info);
	pBuf = (char *)malloc(buffersize);

	if(pBuf && buffersize > 0) {
		for (i=0; i<info.fragstotal; i++) {
			memset(pBuf,0,buffersize);
			write(this->Priv->audiofp,pBuf,buffersize);
		}
	}
	free(pBuf);
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerInitPlayAndRec 初始化录音和放音
 *
 * @param this
 * @param handle 返回声卡句柄
 * @param type ENUM_MIXER_CLEAR_PLAY_BUFFER 清除放音句柄
 * 			   ENUM_MIXER_CLEAR_REC_BUFFER 清除录音句柄
 */
/* ----------------------------------------------------------------*/
static void mixerInitPlayAndRec(TMixer *this,int *handle)
{
	int fp;
	fp = this->Open(this,FMT8K,1);			//单声道
	if (fp <= 0) {
		return;
	}
	// this->ClearPlayBuffer(this);
	this->ClearRecBuffer(this);
	*handle = fp;
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerDeInitPlay 清除放音缓存，释放放音设备
 *
 * @param this
 * @param handle 放音设备句柄
 */
/* ----------------------------------------------------------------*/
static void mixerDeInitPlay(TMixer *this,int *handle)
{
	this->ClearPlayBuffer(this);
	this->Close(this, handle);
}

/* ----------------------------------------------------------------*/
/**
 * @brief mixerDestroy 销毁混音器，释放资源
 *
 * @param this
 */
/* ----------------------------------------------------------------*/
static void mixerDestroy(TMixer *this)
{
	if(this->Priv->audiofp!=-1)
		close(this->Priv->audiofp);
	if(this->Priv->mixer_fd!=-1)
		close(this->Priv->mixer_fd);
	if(this->Priv->audiofp1!=-1)
		close(this->Priv->audiofp1);
	if(this->Priv->mixer_fd1!=-1)
		close(this->Priv->mixer_fd1);
	free(this->Priv);
	free(this);
	this = NULL;
}


/* ----------------------------------------------------------------*/
/**
 * @brief mixerCreate 创建混音器接口
 *
 * @returns 混音器指针
 */
/* ----------------------------------------------------------------*/
TMixer* mixerCreate(void)
{
	int Setting;
	int channel = 1;
	int Sample = FMT8K;

	TMixer *this = (TMixer *)malloc(sizeof(TMixer));
	memset(this,0,sizeof(TMixer));
	this->Priv = (TMixerPriv *)malloc(sizeof(TMixerPriv));
	memset(this->Priv,0,sizeof(TMixerPriv));

	this->Priv->oss_format = AFMT_S16_LE;

	this->Priv->Inited = 0;
	this->Priv->audiofp = -1;
	this->Priv->audiofp1 = -1;

//打开声卡录音句柄
	if ((this->Priv->audiofp1 = open("/dev/dsp1",O_RDONLY | O_NONBLOCK,0)) == -1) {
		perror("/dev/dsp1");
		return NULL;
	}

	fcntl(this->Priv->audiofp1, F_SETFL, O_NONBLOCK);
	Setting = (2<<16) + 10; // 2^12 = 4096;
	if (ioctl(this->Priv->audiofp1, SNDCTL_DSP_SETFRAGMENT, &Setting) == -1) {
		perror("SNDCTL_DSP_SETFRAGMENT");
	}

	if (ioctl(this->Priv->audiofp1,SNDCTL_DSP_SETFMT, &this->Priv->oss_format) == -1) {
		perror("TMixerPriv->oos_format error");
		return NULL;
	}
	if (ioctl(this->Priv->audiofp1, SNDCTL_DSP_CHANNELS, &channel) < 0) {
		perror("set rec channel error");
		return NULL;
	}
	if (ioctl(this->Priv->audiofp1, SNDCTL_DSP_SPEED, &Sample) == -1) {
		perror("set rec sample error");
		return NULL;
	}

//打开录音混音器句柄
	if ((this->Priv->mixer_fd1 = open("/dev/mixer1", O_RDWR)) == -1) {
		free(this->Priv);
		free(this);
		perror("open /dev/mixer1 error");
		return NULL;
    }
	printf("Mixer init successful\n");

	this->Priv->MaxVolume = 99;
	this->Priv->MinVolume = 0;
	mixerSetVolume(this,1,85);

	this->Destroy = mixerDestroy;
	this->Open = mixerOpen;
	this->Close = mixerClose;
	this->Read = mixerRead;
	this->ReadBuf =  mixerReadBuffer;
	this->Write = mixerWrite;
	this->WriteBuffer = mixerWriteBuffer;
	this->InitVolume = mixerInitVolume;
	this->GetVolume  = mixerGetVolume;
	this->SetVolume  = mixerSetVolume;
	this->SetVolumeEx  = mixerSetVolumeEx;
	this->SetSlience = mixerSetSlience;
	this->GetSlience = mixerGetSlience;
	this->ClearRecBuffer = mixerClearRecBuffer;
	this->ClearPlayBuffer = mixerClearPlayBuffer;
	this->InitPlayAndRec = mixerInitPlayAndRec;
	this->DeInitPlay = mixerDeInitPlay;
	return this;
}
