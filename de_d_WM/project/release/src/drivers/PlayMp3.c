/*
 * =====================================================================================
 *
 *       Filename:  PlayMp3.c
 *
 *    Description:  ����mp3�������ӿ�
 *
 ���ļ�ʵ��mp3�Ľ��롣����play_mp3()��ʼ����,����play_mp3_stop��������,�ͷŽ�����Դ��
 ����������ݴ����out_buf�С�����Ҫ����getsoundbuf()��out_buf�л�ȡwav����,Ȼ��
 �����������������������������Ҫ����freesoundbuf()����buf���ظ�����ģ�顣
 ��ģ����Խ���������ʺ�λ����mp3�ļ��������PCM����λ��Ϊ24bit���������Ͳ�������mp3�ļ���ͬ
 Ŀǰ����Ӧ����ֻ��8k 8bit��������ʹ��scale8��24bitת����8bit����û�д������ʡ�
 ��������mp3�ļ������ʱ���Ϊ8k
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
	char in_buf[MP3_READ_BUF_SIZE];		//mp3����
	int inlen;							//in_buf�����ݳ���
	unsigned char out_buf[20484];		//pcm����
	int outpos;							//���buf����ƫ����
}mp3_buffer;

typedef struct _PrivatePlayMp3
{
	mp3_buffer buffer;
	struct mad_decoder decoder;		//
	PShareMemory Share;
	char Mp3FileName[256];		//Mp3�ļ���
	int hdev;					//����Ƶ�ľ��
	unsigned int Sample;		//������
	int nChannel;				//ͨ��
	int FileSize;				//�ļ���С
	int FileCurrPos;			//�ļ���ǰλ��ָ��
	int PlayCurrPos;			//��ǰ����λ��
	int FileSeekPos;			//�ļ���Ҫ�ƶ���ָ��,-1Ϊ����Ҫ�ƶ�
	int Status;					//����״̬
	int Model;					//ģʽ 1ѭ��
	int Stop;					//�Ƿ�ֹͣ
	int Mp3PthreadRun;			//MP3�߳��Ƿ�����
	int FilePthreadRun;			//�ļ����߳��Ƿ�����
	//--------------  xb 20150727
	TMixer *mixer;				//������
	//--------------
	time_t PlayTime_Ini;					///������ʼ����ʱ��
	time_t PlayTime_Pau;					///������ʼ����ʱ��
	time_t PlayTime_Cur;					////������ǰ����ʱ��
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
		//������������
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
		// �̶�MP3����ͨ��Ϊ2
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
//mp3��������ص�����
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
//mp3��������ص�����
//���������λ���������Ĵ�����û�жԲ����ʴ���
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
//mp3�������ص�����
static enum mad_flow mp3_error(void *Param,struct mad_stream *stream,struct mad_frame *frame)
{
	PlayMp3Struct *this =(PlayMp3Struct *)Param;
	if(this->priv->Stop)
		return MAD_FLOW_STOP;
	return MAD_FLOW_CONTINUE;
}
//------------------------------------------------------------------------
//����mp3�߳�
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
	//�������
	if(this->priv->Share)
		this->priv->Share->CloseMemory(this->priv->Share);
	//�ȴ��ļ����߳��˳�
	while(this->priv->FilePthreadRun) {
		usleep(10000);		//10ms
	}
	//ɾ�������ڴ�
	if(this->priv->Share) {
		this->priv->Share->Destroy(this->priv->Share);
		this->priv->Share = NULL;
	}
	//�ر���Ƶ���
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
					//ѭ������
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
			//�ļ�����
			if(this->priv->Model) {
				//ѭ������
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
//����:
//		��ʼ��mp3���룬������mp3�����߳�
//����:
//		pFileName ��Ҫ�����mp3�ļ���
//		nchannel  ���������,����Ϊ0 ��1
//		nbit	  ���λ��,����Ϊ8,16��24
//		nSamples  ���������
//���:
//		-10	��ʾ��ʼ��ʧ��
//		-11 ��ʾ����mad_decoder�ռ�ʧ��
//		-1��ʾ����mp3�����߳�ʧ��
//		=0��ʾ�ɹ�
//��ע:
//		���յ�����ʱ,��video->Ring �ͺ��гɹ�ʱ,��video->ReCall(video,CALL_OK)�����ط�����
//		nchannel,���pFileNameָ����mp3�ļ�������Ϊ2����nchannelָ�����������Ϊ1,��ֻ���������
//		���pFileNameָ����mp3�ļ�������Ϊ1����nchannelָ�����������Ϊ2,�������������������һ��
//		ģ��û��ʵ���ϲ������²������������nSamplesָ�������������Ч����ʵ�������������ԭmp3��
//		������һ��
//		����spi�����Լ��������nSamples�̶�Ϊ8000,nchannel�̶�Ϊ1��nbit�̶�Ϊ8
//
static int Play(PlayMp3Struct *this)
{
	int result;
	pthread_t pthread;
	pthread_attr_t threadAttr1;				//�߳�����
	struct sched_param SchedParam;	//���ȼ�
	if(this->priv->Share == NULL) {
		//������������
		this->priv->Share = CreateShareMemory(SHAREMEMORYSIZE,SHAREMEMORYCNT,0);
		if(this->priv->Share==NULL) {
			printf("Can't create ShareMemory\n");
			return -1;
		}
	} else {
		//�����������
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
	SchedParam.sched_priority = 80;			//�߳����ȼ�Ϊ10,ԽСԽ�����ȼ�,ʵʱ����Ϊ(1~99);
	pthread_attr_init(&threadAttr1);		//���Ӳ���
	pthread_attr_setdetachstate(&threadAttr1,PTHREAD_CREATE_DETACHED);	//�����߳�Ϊ�Զ�����

	result = pthread_create(&pthread,&threadAttr1,Mp3ReadFile,this);	//�����߳�
	if(result) {
		this->priv->Stop = 1;
		//printf(__FUNCTION__" pthread failt,Error code:%d\n",result);
		printf("Play() 1 pthread failt,Error code:%d\n",result);
	}
	pthread_attr_setschedpolicy(&threadAttr1, SCHED_RR);			//�����߳�Ϊʵʱ����ѯ����
	pthread_attr_setschedparam(&threadAttr1, &SchedParam);			//�����̵߳����ȼ�
	result = pthread_create(&pthread,&threadAttr1, thread_dec_mp3, this);	//����mp3�����߳�
	if(result) {
		this->priv->Stop = 1;
		this->priv->Share->CloseMemory(this->priv->Share);
		printf("Play() 2 pthread failt,Error code:%d\n",result);
	}
	pthread_attr_destroy(&threadAttr1);		//�ͷŸ��Ӳ���
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
	//�������
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






