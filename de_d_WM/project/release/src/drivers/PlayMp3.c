/*
 * =====================================================================================
 *
 *       Filename:  PlayMp3.c
 *
 *    Description:  创建mp3播放器接口
 *
 本文件实现mp3的解码。调用play_mp3()开始解码,调用play_mp3_stop结束解码,释放解码资源。
 解码出的数据存放在out_buf中。你需要调用getsoundbuf()从out_buf中获取wav声音,然后将
 该数据送入声卡。声音播放完后需要调用freesoundbuf()将该buf返回给解码模块。
 本模块可以解任意采样率和位数的mp3文件。输出的PCM数据位数为24bit，声道数和采样率与mp3文件相同
 目前我们应用中只有8k 8bit单声道。使用scale8将24bit转换成8bit。但没有处理波特率。
 因此输入的mp3文件采样率必须为8k
 *        Version:  1.0
 *        Created:  2015-12-22 10:37:09
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
#include <string.h>
#include <stdlib.h>

#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "Mixer.h"
#include "mad.h"
#include "PlayMp3.h"
#include "playwav.h"
#include "ShareMemory.h"
#include "predefine.h"

/* ----------------------------------------------------------------*
 *                  extern variables declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                  internal functions declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                        macro define
 *-----------------------------------------------------------------*/
#define SHAREMEMORYSIZE (10*1024)
#define SHAREMEMORYCNT	3
#define MP3_READ_BUF_SIZE (2*SHAREMEMORYSIZE)


typedef struct _mp3_buffer {
	char in_buf[MP3_READ_BUF_SIZE];		//mp3数据
	int inlen;							//in_buf中数据长度
	unsigned char out_buf[20484];		//pcm数据
	int outpos;							//输出buf数据偏移量
}mp3_buffer;

typedef struct _PrivatePlayMp3
{
	mp3_buffer buffer;
	struct mad_decoder decoder;		//
	PShareMemory Share;
	char Mp3FileName[256];		//Mp3文件名
	int hdev;					//打开音频的句柄
	unsigned int Sample;		//采样率
	int nChannel;				//通道
	int FileSize;				//文件大小
	int FileCurrPos;			//文件当前位置指针
	int PlayCurrPos;			//当前播放位置
	int FileSeekPos;			//文件需要移动的指针,-1为不需要移动
	int Status;					//播放状态
	int Model;					//模式 1循环
	int Stop;					//是否停止
	int Mp3PthreadRun;			//MP3线程是否运行
	int FilePthreadRun;			//文件读线程是否运行
	//--------------  xb 20150727
	TMixer *mixer;				//混音器
	//--------------
	time_t PlayTime_Ini;					///歌曲开始播放时间
	time_t PlayTime_Pau;					///歌曲开始播放时间
	time_t PlayTime_Cur;					////歌曲当前播放时间
} PrivatePlayMp3;
/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/


//------------------------------------------------------------------------
static enum mad_flow mp3_header(void *Param,struct mad_header const  *header)
{
	PlayMp3Struct * this =(PlayMp3Struct *)Param;
	if((this->priv->hdev <= 0) || this->priv->Sample!=header->samplerate ||
		this->priv->nChannel!=header->mode)
	{
		int  temp_nSamples;
		//按参数打开声卡
		this->priv->Sample=header->samplerate;
		// printf("[%s]sample:%d\n",__FUNCTION__,this->priv->Sample );
		this->priv->nChannel=header->mode;

		if((44100!=header->samplerate) ||(22050!=header->samplerate)) {
			if(header->samplerate < FMT8K+2000)
				temp_nSamples = FMT8K;
			else if(header->samplerate < 30000)
				temp_nSamples = FMT22K;
			else
				temp_nSamples = FMT44K;
		}
		else
			temp_nSamples = header->samplerate;

		if(this->priv->hdev > 0) {
			this->priv->mixer->Close(this->priv->mixer,&this->priv->hdev);
		}
		// 固定MP3采样通道为2
		this->priv->hdev = this->priv->mixer->Open(this->priv->mixer,temp_nSamples,2);
		if(this->priv->hdev <= 0) {
			printf("err: mp3 open sound,file name:%s\n",this->priv->Mp3FileName);
			return MAD_FLOW_IGNORE;
		}
		// this->priv->mixer->ChangeSample(this->priv->mixer,temp_nSamples); // xb20150727
//		printf("flag---------------->mp3.hdev :%d\n",this->priv->hdev);
	}
	return MAD_FLOW_CONTINUE;
}
//------------------------------------------------------------------------
/*
 * this is the input callback. The purpose of this callback is to (re)fill
 * the stream buffer which is to be decoded. In this example, an entire file
 * has been mapped into memory, so we just call mad_stream_buffer() with the
 * address and length of the mapping. When this callback is called a second
 * time, we are finished decoding.
 */
//mp3解码输入回调函数
static enum mad_flow mp3_input(void *Param,
		struct mad_stream *stream)
{
	int itmp;
	unsigned char *pData;
	PlayMp3Struct *this =(PlayMp3Struct *)Param;
	mp3_buffer *pbuffer = &this->priv->buffer;
	PShareMemory pShare = this->priv->Share;


	if(this->priv->Stop)
		return MAD_FLOW_STOP;

	if(stream->next_frame)
	{
		int unused;
		pData = pbuffer->in_buf;
		unused = (pData + pbuffer->inlen) - stream->next_frame;
		memmove(pbuffer->in_buf, stream->next_frame, unused);
		pbuffer->inlen = unused;
	}
	if(pbuffer->inlen<(MP3_READ_BUF_SIZE/2)) {
		pData = pShare->GetStart(pShare,&itmp);
		if(itmp <= 0) {
			pShare->GetEnd(pShare);
			this->priv->Stop = 1;
			return MAD_FLOW_STOP;
		}
		memcpy(pbuffer->in_buf + pbuffer->inlen,pData,itmp);
		pbuffer->inlen+=itmp;
		pShare->GetEnd(pShare);
	}
	mad_stream_buffer(stream, pbuffer->in_buf, pbuffer->inlen); //xiehb delete
	this->priv->PlayCurrPos += pbuffer->inlen;
	return MAD_FLOW_CONTINUE;
}
//------------------------------------------------------------------------
static signed int scale16(mad_fixed_t sample)
{
	/* round */
	sample += (1L << (MAD_F_FRACBITS - 16));

	/* clip */
	if (sample >= MAD_F_ONE)
		sample = MAD_F_ONE - 1;
	else if (sample < -MAD_F_ONE)
		sample = -MAD_F_ONE;

	/* quantize */
	return sample >> (MAD_F_FRACBITS + 1 - 16);
}
//------------------------------------------------------------------------
//mp3解码输出回调函数
//进行了输出位数和声道的处理，但没有对采样率处理
static enum mad_flow mp3_output(void *Param,struct mad_header const *header,struct mad_pcm *pcm)
{
	PlayMp3Struct *this =(PlayMp3Struct *)Param;
	unsigned int nchannels, nsamples;
	mad_fixed_t const *left_ch, *right_ch;
	mp3_buffer *pbuffer = &this->priv->buffer;
	unsigned char * outsam = pbuffer->out_buf;

	nchannels = pcm->channels;
	nsamples  = pcm->length;
	left_ch   = pcm->samples[0];
	right_ch  = pcm->samples[1];
	while (nsamples--) {
		signed int sample;
		sample = scale16(*left_ch++);
		outsam[pbuffer->outpos++] = (sample >> 0) & 0xff;
		outsam[pbuffer->outpos++] = (sample >> 8) & 0xff;

		if(nchannels == 2)
		{
			sample = scale16(*right_ch++);
			outsam[pbuffer->outpos++] = (sample >> 0) & 0xff;
			outsam[pbuffer->outpos++] = (sample >> 8) & 0xff;
		}
		if(nchannels == 1)
		{
			outsam[pbuffer->outpos++] = (sample >> 0) & 0xff;
			outsam[pbuffer->outpos++] = (sample >> 8) & 0xff;
		}
		if(pbuffer->outpos >= 20480)
		{
			this->priv->mixer->WriteBuffer(this->priv->mixer,this->priv->hdev,pbuffer->out_buf,pbuffer->outpos);
			pbuffer->outpos = 0;
		}
	}
	if(this->priv->Stop)
		return MAD_FLOW_STOP;

	return MAD_FLOW_CONTINUE;

	//PlayMp3Struct *this =(PlayMp3Struct *)Param;
	//this->priv->mixer->Write(this->priv->mixer,this->priv->hdev,pcm->samples,pcm->length);
	//if(this->priv->Stop)
	//	return MAD_FLOW_STOP;

	//return MAD_FLOW_CONTINUE;
}
//------------------------------------------------------------------------
//mp3解码错误回调函数
static enum mad_flow mp3_error(void *Param,struct mad_stream *stream,struct mad_frame *frame)
{
	PlayMp3Struct *this =(PlayMp3Struct *)Param;
	if(this->priv->Stop)
		return MAD_FLOW_STOP;
	return MAD_FLOW_CONTINUE;
}
//------------------------------------------------------------------------
//解码mp3线程
static void * thread_dec_mp3(void *Param)
{
	PlayMp3Struct *this =(PlayMp3Struct *)Param;
	mad_decoder_init(&this->priv->decoder, Param, // xiehb delete
		mp3_input, mp3_header /* header */, 0 /* filter */, mp3_output,
		mp3_error, 0 /* message */);
	this->priv->Mp3PthreadRun = 1;
	while(!this->priv->Stop) {
		mad_decoder_run(&this->priv->decoder, MAD_DECODER_MODE_SYNC); // xiehb delete
	}
	mad_decoder_finish(&this->priv->decoder); // xiehb delete

	this->priv->Stop = 1;
	//解除阻塞
	if(this->priv->Share)
		this->priv->Share->CloseMemory(this->priv->Share);
	//等待文件读线程退出
	while(this->priv->FilePthreadRun) {
		usleep(10000);		//10ms
	}
	//删除共享内存
	if(this->priv->Share) {
		this->priv->Share->Destroy(this->priv->Share);
		this->priv->Share = NULL;
	}
	//关闭音频句柄
	if(this->priv->hdev > 0) {
		this->priv->mixer->Close(this->priv->mixer,&this->priv->hdev);
	}
	if (this->CallBackProc != NULL) {
		this->CallBackProc();
	}
	this->priv->Mp3PthreadRun = 0;
	this->priv->Status = MP3_STOP;
	pthread_exit(NULL);
	return NULL;
}

//------------------------------------------------------------------------
static void * Mp3ReadFile(void *ThreadData)
{
	PlayMp3Struct *this = (PlayMp3Struct *)ThreadData;
	FILE *fp = fopen(this->priv->Mp3FileName,"rb");
	PShareMemory pShare = this->priv->Share;
	if(fp == NULL) {
		printf("Can't open file %s to play\n",this->priv->Mp3FileName);
		pShare->SaveStart(pShare);
		pShare->SaveEnd(pShare,0);
		this->priv->Stop = 1;
		pthread_exit(NULL);
		return NULL;
	}
	fseek(fp,0,2);
	this->priv->FileSize = ftell(fp);
	fseek(fp,0,0);
	this->priv->FileCurrPos = 0;
	this->priv->FilePthreadRun = 1;
	while(!this->priv->Stop) {
		char *pBuf = pShare->SaveStart(pShare);
		int ReadSize;
		if(pBuf==NULL) {
			pShare->SaveEnd(pShare,0);
			break;
		}
		if(this->priv->FileSeekPos>=0) {
			this->priv->FileCurrPos = this->priv->FileSeekPos;
			if(this->priv->FileCurrPos>=this->priv->FileSize) {
				if(this->priv->Model) {
					//循环播放
					fseek(fp,0,0);
					this->priv->FileCurrPos = 0;
				} else {
					pShare->SaveStart(pShare);
					pShare->SaveEnd(pShare,0);
					break;
				}
			} else {
				fseek(fp,this->priv->FileSeekPos,0);
				this->priv->FileSeekPos = -1;
			}
		}
		ReadSize = fread(pBuf,1,SHAREMEMORYSIZE,fp);
		if(ReadSize<=0) {
			pShare->SaveEnd(pShare,0);
			break;
		} else
			pShare->SaveEnd(pShare,ReadSize);
		this->priv->FileCurrPos+=ReadSize;
		if(this->priv->FileCurrPos>=this->priv->FileSize) {
			//文件结束
			if(this->priv->Model) {
				//循环播放
				fseek(fp,0,0);
				this->priv->FileCurrPos = 0;
			} else {
				pShare->SaveStart(pShare);
				pShare->SaveEnd(pShare,0);
				break;
			}
		}
	}
	fclose(fp);
	this->priv->FilePthreadRun = 0;
	pthread_exit(NULL);
	return NULL;
}
//------------------------------------------------------------------------
//功能:
//		初始化mp3解码，并启动mp3解码线程
//输入:
//		pFileName 需要解码的mp3文件名
//		nchannel  输出声道数,必须为0 或1
//		nbit	  输出位数,必须为8,16或24
//		nSamples  输出采样率
//输出:
//		-10	表示初始化失败
//		-11 表示分配mad_decoder空间失败
//		-1表示创建mp3解码线程失败
//		=0表示成功
//备注:
//		在收到呼叫时,即video->Ring 和呼叫成功时,即video->ReCall(video,CALL_OK)两个地方调用
//		nchannel,如果pFileName指定的mp3文件的声道为2，而nchannel指定输出声道数为1,则只输出左声道
//		如果pFileName指定的mp3文件的声道为1，而nchannel指定输出声道数为2,则将输出的左右声道数据一样
//		模块没有实现上采样和下采样，因此现在nSamples指定输出采样率无效果，实际输出采样率与原mp3的
//		采样率一样
//		由于spi传输的约定，现在nSamples固定为8000,nchannel固定为1，nbit固定为8
//
static int Play(PlayMp3Struct *this)
{
	int result;
	pthread_t pthread;
	pthread_attr_t threadAttr1;				//线程属性
	struct sched_param SchedParam;	//优先级
	if(this->priv->Share == NULL) {
		//创建共享缓冲区
		this->priv->Share = CreateShareMemory(SHAREMEMORYSIZE,SHAREMEMORYCNT,0);
		if(this->priv->Share==NULL) {
			printf("Can't create ShareMemory\n");
			return -1;
		}
	} else {
		//清除共享缓冲区
		while(this->priv->Share->WriteCnt(this->priv->Share)) {
			int Size;
			this->priv->Share->GetStart(this->priv->Share,&Size);
			this->priv->Share->GetEnd(this->priv->Share);
		}
	}
	this->priv->Stop = 0;
	this->priv->Status = MP3_PLAY;
	this->priv->buffer.inlen = 0;
	this->priv->buffer.outpos = 0;
	this->priv->PlayTime_Cur = 0;
	time(&this->priv->PlayTime_Ini);
	SchedParam.sched_priority = 80;			//线程优先级为10,越小越高优先级,实时策略为(1~99);
	pthread_attr_init(&threadAttr1);		//附加参数
	pthread_attr_setdetachstate(&threadAttr1,PTHREAD_CREATE_DETACHED);	//设置线程为自动销毁

	result = pthread_create(&pthread,&threadAttr1,Mp3ReadFile,this);	//创建线程
	if(result) {
		this->priv->Stop = 1;
		//printf(__FUNCTION__" pthread failt,Error code:%d\n",result);
		printf("Play() 1 pthread failt,Error code:%d\n",result);
	}
	pthread_attr_setschedpolicy(&threadAttr1, SCHED_RR);			//设置线程为实时的轮询策略
	pthread_attr_setschedparam(&threadAttr1, &SchedParam);			//设置线程的优先级
	result = pthread_create(&pthread,&threadAttr1, thread_dec_mp3, this);	//启动mp3解码线程
	if(result) {
		this->priv->Stop = 1;
		this->priv->Share->CloseMemory(this->priv->Share);
		printf("Play() 2 pthread failt,Error code:%d\n",result);
	}
	pthread_attr_destroy(&threadAttr1);		//释放附加参数
	return 0;
}
//------------------------------------------------------------------------
static int playMp3Play(PlayMp3Struct *this,const char * pFileName,int Model,void (*CallbackProc)(void))
{
	if(pFileName==NULL || pFileName[0]==0)
		return -1;
	this->CallBackProc = CallbackProc;
	if(this->priv->Status==MP3_PLAY) {
		this->Stop(this);
	}
	playWavStop(CallbackProc);
	this->priv->Model = Model;
	strncpy(this->priv->Mp3FileName,pFileName,255);

	this->priv->PlayTime_Pau = 0;
	this->priv->PlayCurrPos = 0;
	return Play(this);
}
//------------------------------------------------------------------------
static void playMp3Stop(PlayMp3Struct *this)
{
	this->priv->Stop = 1;
	//解除阻塞
	if(this->priv->Share)
		this->priv->Share->CloseMemory(this->priv->Share);
	while(this->priv->Mp3PthreadRun) {
		usleep(10000);		//10ms
	}
	this->priv->Status = MP3_STOP;
}
//------------------------------------------------------------------------
static int  playMp3GetVolume(PlayMp3Struct *this)
{
	return this->priv->mixer->GetVolume(this->priv->mixer,0);
}
//------------------------------------------------------------------------
static int  playMp3SetVolume(PlayMp3Struct *this,int volume)
{
	if(0 ==this->priv->mixer->SetVolume(this->priv->mixer,0,volume))
		return 0;
	else
		return -1;
}
//------------------------------------------------------------------------
static int playMp3Pause(PlayMp3Struct *this)
{
	if(this->priv->Status==MP3_PLAY) {
		time(&this->priv->PlayTime_Cur);
		this->priv->PlayTime_Pau += this->priv->PlayTime_Cur - this->priv->PlayTime_Ini;
		this->Stop(this);
		this->priv->Status = MP3_PAUSE;
		return 0;
	}
	return -1;
}
//------------------------------------------------------------------------
static int playMp3Resume(PlayMp3Struct *this)
{
	if(this->priv->Status==MP3_PAUSE) {
		// this->priv->FileSeekPos = this->priv->PlayCurrPos;
		this->priv->PlayTime_Pau = 0;
		this->priv->PlayCurrPos = 0;
		return Play(this);
	} else
		return -1;
}
//------------------------------------------------------------------------
static void playMp3Seek(PlayMp3Struct *this,int Pos)
{
	this->priv->PlayCurrPos = this->priv->FileSize /100 * Pos;
	this->priv->FileSeekPos = this->priv->PlayCurrPos;
}
//------------------------------------------------------------------------
static void playMp3Destroy(PlayMp3Struct *this)
{
	this->Stop(this);
	free(this->priv);
	free(this);
	this = NULL;
}
//------------------------------------------------------------------------
static int playMp3GetStatus(PlayMp3Struct *this)
{
	return this->priv->Status;
}
//------------------------------------------------------------------------
static int playMp3GetPos(PlayMp3Struct *this)              //////the  Current_Statue is between 0 --100
{
	if(this->priv->Status==MP3_STOP)
		return -1;
	else if(this->priv->FileSize)
		return this->priv->PlayCurrPos/(this->priv->FileSize/1000);
	else
		return 0;
}
//------------------------------------------------------------------------
static int playMp3GetPlayTime(PlayMp3Struct *this)
{
	if(this->priv->Status==MP3_PLAY) {
		time(&this->priv->PlayTime_Cur);
		return this->priv->PlayTime_Cur-this->priv->PlayTime_Ini+this->priv->PlayTime_Pau;
	}
	return 0;
}
//------------------------------------------------------------------------
PlayMp3Struct* playMp3Create(void *Mixer)
{
	PlayMp3Struct *this = (PlayMp3Struct *)malloc(sizeof(PlayMp3Struct));
	memset(this,0,sizeof(PlayMp3Struct));
	this->priv = (PrivatePlayMp3*) malloc (sizeof(PrivatePlayMp3));
	memset(this->priv,0,sizeof(PrivatePlayMp3));
	this->priv->Status = MP3_STOP;
	this->priv->mixer = (TMixer *)Mixer;
	this->GetPTime =    playMp3GetPlayTime;
	this->GetPos =		playMp3GetPos;
	this->Seek  =		playMp3Seek;
	this->GetStatus =	playMp3GetStatus;
	this->Play =		playMp3Play;
	this->Stop =		playMp3Stop;
	this->Pause =		playMp3Pause;
	this->Resume =		playMp3Resume;
	this->GetPVolume  =	playMp3GetVolume;
	this->SetPVolume  =	playMp3SetVolume;
	this->Destroy =		playMp3Destroy;
	return this;
}
//------------------------------------------------------------------------






