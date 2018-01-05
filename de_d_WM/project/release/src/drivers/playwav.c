/*
 * =====================================================================================
 *
 *       Filename:  playwav.c
 *
 *    Description:  .wav�ļ���������
 *
 *        Version:  1.0
 *        Created:  2015-11-24 14:22:41
 *       Revision:  none
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
#include <memory.h>
#include "externfunc.h"
#include "pthread.h"
#include "playwav.h"
#include "PlayMp3.h"
#include "predefine.h"
#include "Mixer.h"

/* ----------------------------------------------------------------*
 *                  extern variables declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                  internal functions declare
 *-----------------------------------------------------------------*/
/* ----------------------------------------------------------------*
 *                        macro define
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
static pthread_t thd_play_wav;
static int wav_stop,thread_exit=1;
static void (*CallbackProc)(void);
static int soundfp = -1;


/* ----------------------------------------------------------------*/
/**
 * @brief play_wav_stop ���Ž���������mp3���������
 * ��wav�ļ�δ������Ͳ�����һ���ļ�ʱ����ִ�д˴εĻص�����
 * CallbackProc = NULL;
 *
 * @param type 0�޻ص����� 1�лص�����
 */
/* ----------------------------------------------------------------*/
void playWavStop(void (*Proc)(void))
{
	wav_stop = 1;
	// ���������ĺ����лص�ʱ���ܹرջص�������Ҫ�ص�����ִ����
	if (Proc)
		CallbackProc = NULL;
	while(!thread_exit || (soundfp > 0)) {
		usleep(10000);
	}
	if (soundfp > 0)
		Mixer->Close(Mixer, &soundfp);

}

static void playWavThreadEnd(void)
{
	if(PlayMp3->GetStatus(PlayMp3) != MP3_STOP) {
		PlayMp3->Resume(PlayMp3);
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief thread_wav ����.wav�ļ������߳�
 *
 * @param lpfilename
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
static void * thread_wav(void *lpfilename)
{
	FILE *fp;
	int WavSize;
	int ReadSize;
	char cWavBuf[4096];
	WAVEFILESTRUCT WavMsg;
	fp = fopen(lpfilename,"rb");
	if(fp==NULL) {
		printf("can't open %s\n",(char*)lpfilename);
		goto done2;
	}
	//��ȡ��Ƶ��ʽ
	if(fread(&WavMsg,1,sizeof(WAVEFILESTRUCT),fp)!=sizeof(WAVEFILESTRUCT)) {
		printf("%s format error!\n",lpfilename);
		goto done1;
	}
	if(WavMsg.RIFFArea.RIFF!=0x46464952 && WavMsg.DataAreaSign!=0x61746164) {
		printf("%s format error or can't support!\n",lpfilename);
		goto done1;
	}
	if(WavMsg.WaveFormatSize != 16) {
		printf("Only support 16bit standard wave format!\n");
		goto done1;
	}
	if(WavMsg.WaveFormat.wFormatTag != WAVE_FORMAT_PCM) {
		printf("Only support PCM wave format!\n");
		goto done1;
	}
	WavSize = WavMsg.DataAreaSize;
	if(WavSize<=0) {
		printf("%s format error!\n",lpfilename);
		goto done1;
	}
	// printf("size:%d,sec:%d,channel:%d\n",WavSize,WavMsg.WaveFormat.nSamplesPerSec,WavMsg.WaveFormat.nChannels);
	soundfp = Mixer->Open(Mixer,WavMsg.WaveFormat.nSamplesPerSec,
		WavMsg.WaveFormat.nChannels);
	if(soundfp <= 0) {
		printf("wav can't open sound card!\n");
		goto done1;
	}

	// printf("stop:%d,wave_size:%d\n",wav_stop,WavSize );
	while(!wav_stop && WavSize) {
		//���ļ�����д��spi�ӿڽ��в���
		ReadSize = WavSize>sizeof(cWavBuf)?sizeof(cWavBuf):WavSize;
		ReadSize = fread(cWavBuf,1,ReadSize,fp);
		// printf("ReadSize :%d\n",ReadSize);
		if(ReadSize<=0) {
			printf("Read data size is less zero!\n");
			break;
		}

		Mixer->WriteBuffer(Mixer,soundfp,cWavBuf,ReadSize);
		WavSize-=ReadSize;
	}
	Mixer->ClearPlayBuffer(Mixer);
	Mixer->Close(Mixer, &soundfp);

done1:
	fclose(fp);
done2:
	free(lpfilename);
	if (CallbackProc) {
		CallbackProc();
		CallbackProc = NULL;
	}
	playWavThreadEnd();
	wav_stop = 1;
	thread_exit = 1;
	pthread_exit(NULL);
	return NULL;
}

/* ----------------------------------------------------------------*/
/**
 * @brief playwavfile ������֧����������16λ��WAV�ļ�
 *
 * @param lpfilename �����ļ�����
 * @param Proc ������ϻص�����
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
int playwavfile(char * lpfilename,void (*Proc)(void))
{
	int result;
	char *pName;
	pthread_attr_t threadAttr1;				//�߳�����

	playWavStop(Proc);
	thread_exit = 0; // ע�⣡�жϽ������߳̿�ʼ��������ֹ�����ٴν��뵼������stop������2���߳�
	CallbackProc = Proc;
	pthread_attr_init(&threadAttr1);		//���Ӳ���
	pthread_attr_setdetachstate(&threadAttr1,PTHREAD_CREATE_DETACHED);	//�����߳�Ϊ�Զ�����
	//�ڲ���MP3��ʱ��ͣMP3
	if(PlayMp3->GetStatus(PlayMp3) != MP3_STOP) {
		// PlayMp3->Stop(PlayMp3);
		PlayMp3->Pause(PlayMp3);
	}
	pName = (char*)malloc(256);
	strcpy(pName,lpfilename);
	wav_stop = 0;

	result = pthread_create(&thd_play_wav, &threadAttr1, thread_wav, pName); //��ʼ����,��·��ͨ
	if(result) {
		printf("[%s]pthread failt,Error code:%d\n",__FUNCTION__,result);
	}
	pthread_attr_destroy(&threadAttr1);		//�ͷŸ��Ӳ���
	return 0;
}

