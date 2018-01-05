/*
 * =============================================================================
 *
 *       Filename:  Rtp.c
 *
 *    Description:  ���䴦��
 *
 *        Version:  1.0
 *        Created:  2016-03-01 14:27:57
 *       Revision:  1.0
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =============================================================================
 */
/* ---------------------------------------------------------------------------*
 *                      include head files
 *----------------------------------------------------------------------------*/
#include "externfunc.h"

/* ---------------------------------------------------------------------------*
 *                  extern variables declare
 *----------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*
 *                  internal functions declare
 *----------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*
 *                        macro define
 *----------------------------------------------------------------------------*/
#if DBG_RTP > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

#ifndef TC3000_BZ
#define AUDIO_ADPCM_ENABLE
#endif

#define AUDIO_SIZE		1024

typedef struct _PspPpsH264 {
	int len;
	unsigned char data[32];
}PspPpsH264;

static int rtp_video_type;
int SendSeq = 0;
int SendSeq2 = 0;
int FirstCheckVType = 1;
int FirstCheckVType2 = 1;
extern MyMonitor * monitor;

/* ---------------------------------------------------------------------------*
 *                      variables define
 *----------------------------------------------------------------------------*/
static BOOL RTPTerminate;		//RTP�߳��Ƿ����
static BOOL SPITerminate = TRUE;		//SPI�߳��Ƿ����
static BOOL SPITerminateForGuest = TRUE;		//SPI�߳��Ƿ����
static BOOL EncodeTerminate = TRUE;	//Eecode�߳��Ƿ����
static BOOL EncodeTerminateForGuest = TRUE;	//Eecode�߳��Ƿ����
static BOOL RTPThreadExit = TRUE;		//RTP�߳��Ƿ����
static BOOL SPIThreadExit = TRUE;		//SPI�߳��Ƿ����
static BOOL SPIThreadExitForGuest = TRUE;		//SPI�߳��Ƿ����
static BOOL EncodeThreadExit = TRUE;	//Decode�߳��Ƿ����
static BOOL EncodeThreadExitForGuest = TRUE;	//Decode�߳��Ƿ����

static TRTPObject *RTPObject = NULL;	//RTP�������
static TRTPObject *RTPObjectForGuest = NULL;	//RTP�������


// ��ӡ֡��
int seq = 0;
int cnt_test = 0;
static int TempRTPPort = 8800;
static ShareMemory *RtpDcMem=NULL;			//RTP��Dc�Ĺ����ڴ�
static ShareMemoryForGuest *RtpDcMemForGuest=NULL;			//RTP��Dc�Ĺ����ڴ�
static pthread_t RTP_pthread;
static pthread_t Encode_pthread;
static pthread_t Encode_pthreadForGuest;

static pthread_t SPI_pthread;
static pthread_t SPI_pthreadForGuest;

static PspPpsH264 h264_psp_pps_tmp;
static PspPpsH264 h264_psp_pps_tmp2;


static void (*rtpAbortCallback)(void *callBack);
static void *call_back_data;
/* ---------------------------------------------------------------------------*/
/**
 * @brief saveH264Head ����h264��psp pps��Ƶͷ
 *
 * @param sdata ��������
 *
 * @returns  ��Ƶͷ����
 */
/* ---------------------------------------------------------------------------*/
static int saveH264Head(unsigned char *sdata)
{
	// printf("[%s]\n", __FUNCTION__);
	int i=0;
	for (i = 0; i < 32; ++i) {
		if ((*sdata & 0x1f) == 5) {
			return i - 4;
		}
		h264_psp_pps_tmp.data[i] = *sdata;
		sdata++;
	}
	return i - 4;
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief saveH264Head2 ����h264��psp pps��Ƶͷ   Ϊ�ͻ�����
 *
 * @param sdata ��������
 *
 * @returns  ��Ƶͷ����
 */
/* ---------------------------------------------------------------------------*/
static int saveH264Head2(unsigned char *sdata)
{
	// printf("[%s]\n", __FUNCTION__);
	int i=0;
	for (i = 0; i < 32; ++i) {
		if ((*sdata & 0x1f) == 5) {
			return i - 4;
		}
		h264_psp_pps_tmp2.data[i] = *sdata;
		sdata++;
	}
	return i - 4;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief addH264Head
 *
 * @param sdata ��Ƶ����
 * @param size  ��Ƶ����
 *
 * @return 0ʧ�� 1�ɹ�
 */
/* ---------------------------------------------------------------------------*/
static int addH264Head(unsigned char *sdata,int *size)
{
	unsigned char buf[MAXENCODEPACKET];
	unsigned char *head_h264;
	int len = 0;
	memset(buf,0,MAXENCODEPACKET);
	if (h264_psp_pps_tmp.len) {
		head_h264 = h264_psp_pps_tmp.data;
		len = h264_psp_pps_tmp.len;
	}

	if (len <= 0) {
		printf("Error! video type didn't know!!\n");
		return 0;
	}

	memcpy(buf,head_h264,len);
	memcpy(&buf[len],sdata,MAXENCODEPACKET - len);
	// print_data(head_h264,len);
	if (*size + len > MAXENCODEPACKET) {
		*size = MAXENCODEPACKET;
		printf("Warn large Iframe size:%d\n",*size);
	} else {
		*size += len;
	}
	memcpy(sdata,buf,*size);
	return 1;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief addH264Head2    Ϊ�ͻ�����
 *
 * @param sdata ��Ƶ����
 * @param size  ��Ƶ����
 *
 * @return 0ʧ�� 1�ɹ�
 */
/* ---------------------------------------------------------------------------*/
static int addH264Head2(unsigned char *sdata,int *size)
{
	unsigned char buf[MAXENCODEPACKET];
	unsigned char *head_h264;
	int len = 0;
	memset(buf,0,MAXENCODEPACKET);
	if (h264_psp_pps_tmp2.len) {
		head_h264 = h264_psp_pps_tmp2.data;
		len = h264_psp_pps_tmp2.len;
	}

	if (len <= 0) {
		printf("Error! video type didn't know!!\n");
		return 0;
	}

	memcpy(buf,head_h264,len);
	memcpy(&buf[len],sdata,MAXENCODEPACKET - len);
	// print_data(head_h264,len);
	if (*size + len > MAXENCODEPACKET) {
		*size = MAXENCODEPACKET;
		printf("Warn large Iframe size:%d\n",*size);
	} else {
		*size += len;
	}
	memcpy(sdata,buf,*size);
	return 1;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief rtpAudioDecode ������Ƶ
 *
 * @param this
 * @param pbody ��Ƶ���ݽṹ
 */
/* ---------------------------------------------------------------------------*/
static void rtpAudioDecode(Rtp* this,rec_body *pbody)
{
	//������Ƶ
	if(pbody->alen <= 0 || pbody->alen >= 12000) { //����Զ��˥��ǿ��
		printf("Error! pbody->alen:%d\n",pbody->alen);
		return;
	}
#ifdef	AUDIO_ADPCM_ENABLE
	short AudioBuf[32*1024];
	//����ADPCM
	int AudioSize = adpcm_decoder((char *)pbody->sdata,
			AudioBuf,pbody->alen);
	if (this->silence == FALSE) {
		if(this->fpAudio) {
			Mixer->Write(Mixer,this->fpAudio,AudioBuf,AudioSize);
		}
	}
#else
	if (this->silence == FALSE) {
		if(this->fpAudio) {
			Mixer->Write(Mixer,this->fpAudio,pbody->sdata,pbody->alen);
		}
	}
#endif
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief rtpThreadExecute RTP�̣߳�������Ƶ����
 *
 * @param ThreadData
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static void * rtpThreadExecute(void *ThreadData)
{
	Rtp* this = (Rtp*)ThreadData;
	char *pData;
	int Size=0;

	pData = (char*)malloc(sizeof(rec_body));
	if(pData == NULL) {
		printf("Can't alloc memory!\n");
		RTPThreadExit = TRUE;
		pthread_exit(NULL);
		return NULL;
	}

	while(!RTPTerminate) {
		if(RTPObject != NULL) {
			Size = RTPObject->RecvBuffer(RTPObject,pData,sizeof(rec_body),500);
		}
		if(Size == -1) {
			//�Է��ѹر�����
			printf("RTP receive is abort!\n");
			rtpAbortCallback(call_back_data);
			break;
		} else if(Size > 0) {
			rec_body *pbody = (rec_body*)pData;
			// test delay---
			// if ((pbody->seq < 3) && (pbody->seq > 0)) {
			// DBG_STR("---------------------------------------->get voice!!!");
			// }

			// printf("seq=%d data=%d\n",
					// pbody->seq,
					// (short)(pbody->sdata[1] * 256 + pbody->sdata[0]));
			// if (   (short)(pbody->sdata[1] * 256 + pbody->sdata[0]) > 1024
			// || (short)(pbody->sdata[1] * 256 + pbody->sdata[0]) < -1024) {
			// DBG_STR("--------------------------------------->get voice!!!");
			// }
			//--------------
			if(pbody->alen) { //������Ƶ
				rtpAudioDecode(this,pbody);
			}
		}
		DelayMs(1);
	}

	if (this->fpAudio) {
		Mixer->DeInitPlay(Mixer,&this->fpAudio);
	}

	free(pData);
	pData = NULL;
	RTPThreadExit = TRUE;
	pthread_exit(NULL);
	return NULL;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief rtpThreadCreate ����RTP�߳�
 *
 * @param this
 */
/* ---------------------------------------------------------------------------*/
static void rtpThreadCreate(Rtp* this)
{
	int result;
	struct sched_param SchedParam;			//���ȼ�
	pthread_attr_t threadAttr1;				//�߳�����

	RTPTerminate = FALSE;
	RTPThreadExit = FALSE;

	pthread_attr_init(&threadAttr1);		//���Ӳ���

	SchedParam.sched_priority = 5;
	pthread_attr_setschedparam(&threadAttr1, &SchedParam);
	pthread_attr_setdetachstate(&threadAttr1,PTHREAD_CREATE_DETACHED);	//�����߳�Ϊ�Զ�����
	result = pthread_create(&RTP_pthread,&threadAttr1,rtpThreadExecute,this);
	if(result) {
		RTPThreadExit = TRUE;
		printf("rtpThreadCreate() pthread failt,Error code:%d\n",result);
	}
	pthread_attr_destroy(&threadAttr1);		//�ͷŸ��Ӳ���
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief spiVideo ������Ƶ
 *
 * @param this
 * @param LastTime ��¼�տ�ʼʱ��
 * @param FramPeriod ֡��
 */
/* ---------------------------------------------------------------------------*/
static void spiVideo(Rtp* this,
		unsigned long long int *LastTime,
		unsigned long long int FramPeriod
		)
{
	rec_body *pbody = NULL; //,*pAudio;
	unsigned long long int LeaveTime;
	if(this->bTransVideo == 0 || !RtpDcMem) {
		return;
	}
	
	LeaveTime = GetMs();
	if((LeaveTime - *LastTime) <= FramPeriod) {
		return;
	}
	int packsize = 0;
	if (RtpDcMem->WriteCnt(RtpDcMem) > 0) {
		pbody = (rec_body *)RtpDcMem->GetStart(RtpDcMem,&packsize);
	} else {
		pbody = NULL;
	}
	
	if(!pbody || packsize == 0) { //������Ƶ
		*LastTime = LeaveTime;
		return;
	}
	seq = pbody->seq; // ��ӡ֡����
	//ȷ�Ϸ��͵ĵ�һ֡Ϊ�ؼ�֡		
	if(pbody->vtype != 1 && FirstCheckVType == 1 && (pbody->seq % Public.FramPeriod != 0))
	{
		goto outGetEnd;
		
	}else {	
		FirstCheckVType = 0;
	}		

	// ����ǰ10֡,����������Ļ������һ�β���ͼ��
	
	if (pbody->seq >= IPInterval_def) {
		pbody->seq -= IPInterval_def;
		if(RTPObject){
			if(RTPObject->SendBuffer(RTPObject,pbody,pbody->tlen) > 0){
			//	printf("Send Data to IP : [%s] , seq : [%d] , Data Lens : [%d] \n",RTPObject->cPeerIP,pbody->seq,pbody->tlen);
			}
		}			
	}
	outGetEnd:
	RtpDcMem->GetEnd(RtpDcMem);
	*LastTime = LeaveTime;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief spiVideoForGuest ������Ƶ Ϊ�ͻ�����
 *
 * @param this
 * @param LastTime ��¼�տ�ʼʱ��
 * @param FramPeriod ֡��
 */
/* ---------------------------------------------------------------------------*/
static void spiVideoForGuest(Rtp* this,
		unsigned long long int *LastTime,
		unsigned long long int FramPeriod
		)
{
	rec_body *pbody = NULL; //,*pAudio;
	unsigned long long int LeaveTime;
	if(this->bTransVideo == 0 || !RtpDcMemForGuest) {
		return;
	}
	LeaveTime = GetMs();
	if((LeaveTime - *LastTime) <= FramPeriod) {
		return;
	}
	int packsize = 0;

	if (RtpDcMemForGuest->WriteCnt(RtpDcMemForGuest) > 0) {
		pbody = (rec_body *)RtpDcMemForGuest->GetStart(RtpDcMemForGuest,&packsize);
	} else {
		pbody = NULL;
	}

	if(!pbody || packsize == 0) { //������Ƶ
		*LastTime = LeaveTime;
		return;
	}

	//ȷ�Ϸ��͵ĵ�һ֡Ϊ�ؼ�֡		
	if(pbody->vtype != 1 && FirstCheckVType2 == 1 && (pbody->seq % Public.FramPeriod != 0))
	{
		goto OutRtpDcMemForGuestGetEnd;
		
	}else {	
		FirstCheckVType2 = 0;
	}	
	
	// ����ǰ10֡,����������Ļ������һ�β���ͼ��
	if (pbody->seq >= IPInterval_def_Guest) {
		pbody->seq -= IPInterval_def_Guest;
		if(RTPObjectForGuest){
			if(RTPObjectForGuest->SendBuffer(RTPObjectForGuest,pbody,pbody->tlen) > 0){
				//printf("Send Data to IP : [%s] , seq : [%d] , Data Lens : [%d] \n",RTPObjectForGuest->cPeerIP,pbody->seq,pbody->tlen);
			}
		}
	}
	OutRtpDcMemForGuestGetEnd:
	RtpDcMemForGuest->GetEnd(RtpDcMemForGuest);
	*LastTime = LeaveTime;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief spiAudio ������Ƶ
 *
 * @param this
 * @param pAudioPack
 *
 * @returns 0ʧ�� 1�ɹ�
 */
/* ---------------------------------------------------------------------------*/
static int spiAudio(Rtp* this,
		rec_body *pAudioPack)
{
	if(this->bTransAudio == 0 || !pAudioPack) {
		return 0;
	}
	int packsize = 0;
	short AudioBuf[AUDIO_SIZE];
	packsize = Mixer->ReadBuf(Mixer,AudioBuf,AUDIO_SIZE);
	// printf("send size:%d\n",packsize);
	if(packsize <= 0) {
		return 1;
	}
	pAudioPack->seq++;
	pAudioPack->vtype = 0;
	pAudioPack->slen = 0;
#ifdef	AUDIO_ADPCM_ENABLE
	pAudioPack->alen = adpcm_coder((short*)AudioBuf,
			pAudioPack->sdata,packsize);
#else
	memcpy(pAudioPack->sdata, AudioBuf, packsize);
	pAudioPack->alen = packsize;
#endif
	pAudioPack->tlen = RECHEADSIZE+pAudioPack->alen;
	if(RTPObject) {
		RTPObject->SendBuffer(RTPObject,pAudioPack,pAudioPack->tlen);
	}
	return 1;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief spiHeart û����Ƶ����ʱ����������
 *
 * @param pAudioPack
 */
/* ---------------------------------------------------------------------------*/
static void spiHeart(rec_body *pAudioPack)
{
	pAudioPack->seq = 0;
	pAudioPack->vtype = 0;
	pAudioPack->slen = 0;
	pAudioPack->alen = 0;
	pAudioPack->tlen = RECHEADSIZE+pAudioPack->alen+32;
	if(RTPObject) {
		RTPObject->SendBuffer(RTPObject,pAudioPack,pAudioPack->tlen);
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief spiThreadExecute ִ��SPI��������Ƶ����Ƶ
 *
 * @param ThreadData
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static void * spiThreadExecute(void *ThreadData)
{
	Rtp * this = (Rtp *) ThreadData;
//	printf("Check spiThreadExecute Param , Flag : [%d] \n");
	//	struct timeb timeptr;
	unsigned long long int LastTime;
	unsigned long long int FramPeriod;
	rec_body *pAudioPack;
	pAudioPack = (rec_body*)malloc(sizeof(rec_body));
	LastTime = 0;
	LastTime = GetTickCount();
	seq=0;
	// ֡�� fps
	FramPeriod = 1000 / this->frame_period;
	while(!SPITerminate) {
		
		spiVideo(this,&LastTime,FramPeriod);
		if (spiAudio(this,pAudioPack) == 0) {
			spiHeart(pAudioPack);
		}	
		DelayMs(5);
	}
	FirstCheckVType = 1;
	seq = 0;
	cnt_test = 0;
	//if(this->Flag == 0){
		Mixer->ClearRecBuffer(Mixer);
	//}
	if(pAudioPack)
		free(pAudioPack);
	pAudioPack = NULL;	
	SPIThreadExit = TRUE;
	pthread_exit(NULL);
	return NULL;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief spiThreadCreate ����SPI�߳�
 *
 * @param this
 */
/* ---------------------------------------------------------------------------*/
static void spiThreadCreate(Rtp* this)
{
	int result;
	SPITerminate = FALSE;
	SPIThreadExit = FALSE;
	pthread_attr_t threadAttr1;				//�߳�����
	struct sched_param SchedParam;	//���ȼ�
	
	SchedParam.sched_priority = 6;
	pthread_attr_init(&threadAttr1);

	pthread_attr_setdetachstate(&threadAttr1,PTHREAD_CREATE_DETACHED);	//�����߳�Ϊ�Զ�����

	result = pthread_create(&SPI_pthread,&threadAttr1,spiThreadExecute,this);
	
	if(result) {
		SPIThreadExit = TRUE;
		printf("spiThreadCreate() pthread failt,Error code:%d\n",result);
	}
	pthread_attr_destroy(&threadAttr1);		//�ͷŸ��Ӳ���
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief spiThreadExecuteForGuest ִ��SPI��������Ƶ����Ƶ  Ϊ�ͻ�����
 *
 * @param ThreadData
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static void * spiThreadExecuteForGuest(void *ThreadData)
{
	Rtp * this = (Rtp *) ThreadData;
//	printf("Check spiThreadExecute Param , Flag : [%d] \n");
	//	struct timeb timeptr;
	unsigned long long int LastTime;
	unsigned long long int FramPeriod;
	LastTime = 0;
	LastTime = GetTickCount();
	// ֡�� fps
	FramPeriod = 1000 / this->frame_period;
	while(!SPITerminateForGuest) {
		spiVideoForGuest(this,&LastTime,FramPeriod);
		DelayMs(5);
	}
	FirstCheckVType2 = 1;
	cnt_test = 0;
	SPIThreadExitForGuest = TRUE;
	pthread_exit(NULL);
	return NULL;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief spiThreadCreateForGuest ����SPI�߳�  Ϊ�ͻ�����
 *
 * @param this
 */
/* ---------------------------------------------------------------------------*/
static void spiThreadCreateForGuest(Rtp* this)
{
	int result;
	SPITerminateForGuest = FALSE;
	SPIThreadExitForGuest = FALSE;
	pthread_attr_t threadAttr1;				//�߳�����
	struct sched_param SchedParam;	//���ȼ�
	
	SchedParam.sched_priority = 6;
	pthread_attr_init(&threadAttr1);

	pthread_attr_setdetachstate(&threadAttr1,PTHREAD_CREATE_DETACHED);	//�����߳�Ϊ�Զ�����

	result = pthread_create(&SPI_pthreadForGuest,&threadAttr1,spiThreadExecuteForGuest ,this);
	
	if(result) {
		SPIThreadExitForGuest= TRUE;
		printf("spiThreadCreate() pthread failt,Error code:%d\n",result);
	}
	pthread_attr_destroy(&threadAttr1);		//�ͷŸ��Ӳ���
}
//--------------------------------------------------
/**
 * @brief IsIframe �ж��Ƿ�Ϊ�ؼ�֡
 *
 * @param sbuf ����
 *
 * @return 0:P֡ 1:I֡ 2:SPS��PPS
 */
static int IsIframe(unsigned char *sdata)
{
	if (	   (sdata[0] == 0x00)
			&& (sdata[1] == 0x00)
			&& (sdata[2] == 0x00)
			&& (sdata[3] == 0x01)) {
		if (sdata[4] == 0x65){
			return 1;
		} else if (sdata[4] == 0x67){
			return 2;
		}
	}
	return 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief encodeThreadExecute �����߳�ʵ�� H264����
 *
 * @param ThreadData
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static void * encodeThreadExecute(void *ThreadData)
{
	Rtp* this = (Rtp*)ThreadData;
	int enc_ret;
	int IsEncode = 1;
	int IsEncodeForGuest = 1;
	int IsGetKey = 1;
	int IsGetKeyForGuest = 1;
	pthread_mutex_t EncodeState_Lock;
	pthread_mutex_init(&EncodeState_Lock,NULL);
	
	enc_ret = videoin_init(rtp_video_type);
	if(enc_ret != 0){
		printf("H264 Init Fail .. !!! \n");
		return ;
	}
	sleep(1); //��ʱ1s��֤��ʼ�����
	while(1) {
		pthread_mutex_lock(&EncodeState_Lock);
		if(EncodeTerminate){
			EncodeThreadExit = TRUE;
		}else{
			EncodeThreadExit = FALSE;
		}
		if(EncodeTerminateForGuest){
			EncodeThreadExitForGuest= TRUE;
		}else{
			EncodeThreadExitForGuest= FALSE;
		}
		if(EncodeTerminate && EncodeTerminateForGuest){
			printf("EncodeThreadExecute Will Break ! \n");
			break;		
		}		
		pthread_mutex_unlock(&EncodeState_Lock);
		
/*
		if(this->bTransVideo <= 0) {
			DelayMs(10);
			continue;
		}
*/
		unsigned char *pBufdata = NULL;
		unsigned char *pBufdata2 = NULL;
		int keyframe = 0;
		int VideoSize = videoin_encode(&pBufdata, &keyframe);
		pBufdata2 = pBufdata;

		int writesize = 0;
		int writesize2 = 0;
		
		if( !VideoSize || !pBufdata || !pBufdata2) {
			DelayMs(10);
			continue;
		}
		
		writesize = VideoSize;
		writesize2 = VideoSize;
//		printf("writesize : [%d]  , writesize2 : [%d] \n",writesize,writesize2);

				
		if(VideoSize > MAXENCODEPACKET) {
			printf("Error overflow,seq:%d,size:%d!!!\n",SendSeq,VideoSize);
			printf("Error overflow,seq2:%d,size:%d!!!\n",SendSeq2,VideoSize);
			//SaveLog("Error overflow,seq:%d,size:%d!!!\n",SendSeq,VideoSize);
			if(!EncodeTerminate  && IsEncode == 2 ){
				SendSeq++;	
			}
			if(!EncodeTerminateForGuest  && IsEncodeForGuest == 2 ){
				SendSeq2++;
			}
			DelayMs(10);
			continue;
		}
		
		//ȡ���������ڴ���
		if(RtpDcMem){
			if(!EncodeTerminateForGuest && IsEncode == 1){		//����ͻ�����API�����Ѿ�����������ԭ����δ��ʼ��һ��Ҫ�ӵ�һ֡�ؼ�֡��ʼд��!
				int ret = IsIframe(pBufdata);
				if( ret == 0 && IsGetKey == 1){
					goto OutRtpDcMem;					
				}else{
					IsGetKey = 2;	//ȷ���Ѿ��õ��ؼ�֡
				}	
				rec_body *pbody = (rec_body *)RtpDcMem->SaveStart(RtpDcMem);
				if(pbody){
					printf("Write Video Head Data Begin ..........!!! \n");
					pbody->seq = SendSeq;
					//if(pBufdata[4] == 0x65){
					//������Ƶͷ
					//memcpy(h264_psp_pps_tmp,h264_psp_pps_tmp2,sizeof(PspPpsH264));
					h264_psp_pps_tmp = h264_psp_pps_tmp2;
					if (addH264Head(pBufdata,&writesize))  {
						IsEncode = 2; //ȷ�ϵ�ǰ�Ѿ���ʼ����
						pbody->vtype = 1;
					} 
					pbody->tlen = RECHEADSIZE + writesize;	//VideoSize -> writesize
					pbody->alen = 0;
					pbody->slen = writesize;
					memcpy(pbody->sdata, pBufdata, writesize);
					RtpDcMem->SaveEnd(RtpDcMem,writesize);
					printf("Write Video Head Data End ..........!!! \n");
				}
				else{
					RtpDcMem->SaveEnd(RtpDcMem,0);
				}	
				SendSeq++;
			}
			else{
			//	printf("RtpDcMem ---------------------------> SendSeq : [%d] IsEncode : [%d] \n",SendSeq,IsEncode);
				rec_body *pbody = (rec_body *)RtpDcMem->SaveStart(RtpDcMem);
				if(pbody) {
					IsEncode = 2; //ȷ�ϵ�ǰ�Ѿ���ʼ����
					pbody->seq = SendSeq;
					if (pbody->seq == 0) {
						h264_psp_pps_tmp.len = saveH264Head(pBufdata);
						pbody->vtype = 1;
					} else if (pBufdata[4] == 0x65) {
			//			printf("The IFrame Key Seq ------------> [%d] \n",pbody->seq);
						// ÿһ���ؼ�֡��������Ƶͷ����֤��ʧ��һ֡ʱ�ܽ���
						if (addH264Head(pBufdata,&writesize))  {
			//				printf("Add H264 Head succeeful  .............\n");
							pbody->vtype = 1;
						} else {
							pbody->vtype = 2;
						}
					} else {
						pbody->vtype = 2;
					}
					pbody->tlen = RECHEADSIZE + writesize;  //VideoSize -> writesize
					pbody->alen = 0;
					pbody->slen = writesize;
					memcpy(pbody->sdata, pBufdata, writesize);
					RtpDcMem->SaveEnd(RtpDcMem,writesize);
				//	printf("RtpDcMem->SaveEnd(RtpDcMem,writesize) \n");
				}
				else {
						RtpDcMem->SaveEnd(RtpDcMem,0);
				}	
				SendSeq++;
			}
			OutRtpDcMem:	;
		}else{
			//��֤��ǰ�ڴ�Ϊ�պ�������ͬʱд���������
			IsGetKey = 1;
			IsEncode = 1;
		//	printf("IsEncode & IsGetKey  has been Changed !!! \n");
		}
		
	/*********************************************************************************************************/
		if(RtpDcMemForGuest){
			if(!EncodeTerminate && IsEncodeForGuest == 1){		//���ԭ�����Ѿ����������ǿͻ�����API����δ��ʼ���Ѿ�û����0֡
				int ret = IsIframe(pBufdata);	
				if( ret == 0 && IsGetKeyForGuest == 1){
					goto OutRtpDcMemForGuest;					
				}else{
					IsGetKeyForGuest= 2;	//ȷ���Ѿ��õ��ؼ�֡
				}	
				rec_body *pbody2 = (rec_body *)RtpDcMemForGuest->SaveStart(RtpDcMemForGuest);
				if(pbody2){
			//		printf("Write Video Head Data Begin ..........!!! \n");
					pbody2->seq = SendSeq2;
					h264_psp_pps_tmp2 = h264_psp_pps_tmp;
					if (addH264Head2(pBufdata2,&writesize2))  {
						IsEncodeForGuest = 2; //ȷ�ϵ�ǰ�Ѿ���ʼ����
						pbody2->vtype = 1;
					} 
					pbody2->tlen = RECHEADSIZE + writesize2;	//VideoSize -> writesize
					pbody2->alen = 0;
					pbody2->slen = writesize2;
					memcpy(pbody2->sdata, pBufdata2, writesize2);
					RtpDcMemForGuest->SaveEnd(RtpDcMemForGuest,writesize2);
			//		printf("Write Video Head Data End ..........!!! \n");
				}
				else{
					RtpDcMemForGuest->SaveEnd(RtpDcMemForGuest,0);
				}	
				SendSeq2++;
			}
			else{
			//	printf("RtpDcMemForGuest ---------------> SendSeq2 : [%d], IsEncodeForGuest : [%d] \n",SendSeq2,IsEncodeForGuest);
				rec_body *pbody2 = (rec_body *)RtpDcMemForGuest->SaveStart(RtpDcMemForGuest);
				if(pbody2) {
					IsEncodeForGuest = 2; //ȷ�ϵ�ǰ�Ѿ���ʼ����
					pbody2->seq = SendSeq2;
					if (pbody2->seq == 0) {
						h264_psp_pps_tmp2.len = saveH264Head2(pBufdata2);
						pbody2->vtype = 1;
					} else if (pBufdata2[4] == 0x65) {
						// ÿһ���ؼ�֡��������Ƶͷ����֤��ʧ��һ֡ʱ�ܽ���
						if (addH264Head2(pBufdata2,&writesize2))  {
							pbody2->vtype = 1;
						} else {
							pbody2->vtype = 2;
						}
					} else {
						pbody2->vtype = 2;
					}
					pbody2->tlen = RECHEADSIZE + writesize2;  //VideoSize -> writesize
					pbody2->alen = 0;
					pbody2->slen = writesize2;
					memcpy(pbody2->sdata, pBufdata2, writesize2);
					RtpDcMemForGuest->SaveEnd(RtpDcMemForGuest,writesize2);
				//	printf("RtpDcMem->SaveEnd(RtpDcMem,writesize) \n");
				}
				else {
					RtpDcMemForGuest->SaveEnd(RtpDcMemForGuest,0);
				}	
				SendSeq2++;
			}
			OutRtpDcMemForGuest: ;
		}else{
			//��֤���ڴ汻��պ�������ͬʱд���������
			IsGetKeyForGuest = 1;
			IsEncodeForGuest = 1;
		}
		
		DelayMs(10);
	}
	videoin_close();
	printf("*******************************End Encode******************************8\n");
	EncodeThreadExit = TRUE;
	EncodeThreadExitForGuest = TRUE;
	pthread_exit(NULL);
	return NULL;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief encodeThreadCreate ����decode�߳�
 *
 * @param this
 */
/* ---------------------------------------------------------------------------*/
static void encodeThreadCreate(Rtp* this)
{
	int result;
	struct sched_param SchedParam;	//���ȼ�
	pthread_attr_t threadAttr1;				//�߳�����

	SchedParam.sched_priority = 5;
	pthread_attr_init(&threadAttr1);

	pthread_attr_setschedparam(&threadAttr1, &SchedParam);
	pthread_attr_setdetachstate(&threadAttr1,PTHREAD_CREATE_DETACHED);	//�����߳�Ϊ�Զ�����
	result = pthread_create(&Encode_pthread,&threadAttr1,encodeThreadExecute,this);
	if(result) {
		EncodeThreadExit = TRUE;
		EncodeThreadExitForGuest= TRUE;
		printf("%s() pthread failt,Error code:%d\n", __FUNCTION__, result);
	}
	pthread_attr_destroy(&threadAttr1);		//�ͷŸ��Ӳ���
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief rtpInit ��ʼ��RTP����
 *
 * @param this
 * @param IP ����IP
 * @param Port
 *
 * @returns CALL_FAIL ʧ�� CALL_OK �ɹ�
 */
/* ---------------------------------------------------------------------------*/
static int rtpInit(Rtp *this,
		const char *dest_ip, char *local_ip, char *server_ip, int Port, int Flag)
{
	if(Flag == 0){
		if(RTPObject == NULL) {
		RTPObject = TRTPObject_Create(dest_ip, local_ip,
				"","",Port,server_ip);
		}

		if(RTPObject == NULL) {
			return CALL_FAIL;
		}
	}else if (Flag == 1){
		
		if(RTPObjectForGuest== NULL) {
			RTPObjectForGuest = TRTPObject_Create(dest_ip, local_ip,
					"","",Port,server_ip);
		}
		if(RTPObjectForGuest == NULL) {
			return CALL_FAIL;
		}
	}
	/*
	//�رռ�����
	if(monitor){
		monitor->MonitorStop();
	}
	*/
	return CALL_OK;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief rtpBuildConnect ����RTP����
 *
 * @param this 
 */
/* ---------------------------------------------------------------------------*/
static void rtpBuildConnect(Rtp *this,int frame_period)
{
	this->fpAudio = 0;
	this->frame_period = frame_period;

	if(this->Flag == 0){
		this->bTransAudio = FALSE;
		this->bTransVideo = TRUE;
		if(RtpDcMem == NULL) {
			RtpDcMem = CreateShareMemory(sizeof(rec_body),1,0);
		}
		adpcm_init(); //��ʼ��ADPCM����
		rtpThreadCreate(this);	//��RTP�߳�Ϊ��Ƶ�����߳�	
	}

	if(this->Flag == 1){
		if(RtpDcMemForGuest == NULL) {
			RtpDcMemForGuest = CreateShareMemoryForGuest(sizeof(rec_body),1,0);
			if(RtpDcMemForGuest){
			}
		}
	}
	
	
	//��H246�����߳�δ������Ҫ�ٴδ���
	if(this->Flag == 0){
		if(EncodeThreadExit){
			EncodeTerminate = FALSE;
			SendSeq = 0;
			memset(&h264_psp_pps_tmp,0,sizeof(PspPpsH264));
		}		
	}else if(this->Flag == 1){
		if(EncodeThreadExitForGuest){	
			EncodeTerminateForGuest = FALSE;
			SendSeq2 = 0;
			memset(&h264_psp_pps_tmp2,0,sizeof(PspPpsH264));
		}
	}


	if(EncodeThreadExit == 1 && EncodeThreadExitForGuest == 1){	
		encodeThreadCreate(this);	//H264�����߳�
	}

	if(this->Flag == 0){
		if(SPIThreadExit){
			spiThreadCreate(this);	//����Ƶ�����߳�
		}
	}else if(this->Flag == 1){
		if(SPIThreadExitForGuest){			
			spiThreadCreateForGuest(this);	//����Ƶ�����߳�
		}
	}
		
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief rtpStartAudio ʹ�ܴ�����Ƶ
 *
 * @param this
 */
/* ---------------------------------------------------------------------------*/
static void rtpStartAudio(Rtp *this)
{
	this->bTransAudio = TRUE;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief rtpGetFpAudio ����ͨ����Ƶ�����ַ
 *
 * @param this
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int *rtpGetFpAudio(Rtp *this)
{
	return &this->fpAudio;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief rtpSetSilence ����ͨ��ʱ�����Ƿ�����
 * ����ͨ��ʱͬʱ����MP3���
 *
 * @param this
 * @param value TRUE ���� FALSE �Ǿ���
 */
/* ---------------------------------------------------------------------------*/
static void rtpSetSilence(Rtp *this,BOOL value)
{
	this->silence = value;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief rtpGetSilence ��õ�ǰ����״̬
 *
 * @param this
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
static int rtpGetSilence(Rtp *this)
{
	return this->silence;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief rtpSetPeerIp ����ͨѶIP
 *
 * @param this
 * @param ip
 */
/* ---------------------------------------------------------------------------*/
static void rtpSetPeerIp(Rtp *this,char *ip)
{
	strncpy(RTPObject->cPeerIP,ip,15);
	RTPObject->dwPeerIP = inet_addr(ip);
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief rtpClose ͨ���������ر�RTP�߳�
 *
 * @param this
 */
/* ---------------------------------------------------------------------------*/
static void rtpClose(Rtp *this)
{

	if(this->Flag == 0){
		this->silence = FALSE;
		this->bTransAudio = FALSE;
		this->bTransVideo = FALSE;
		if(RtpDcMem) {
			RtpDcMem->CloseMemory(RtpDcMem);   //bug maybe
		}
		printf("Wait RTPThreadExit\n");
		RTPTerminate = TRUE;
		while(!RTPThreadExit) {usleep(10000);}
	
		printf("Wait EncodeThreadExit\n");
		EncodeTerminate = TRUE;
		while(!EncodeThreadExit) {usleep(10000);}
	
		printf("Wait SPIThreadExit\n");
		SPITerminate = TRUE;
		while(!SPIThreadExit) {usleep(10000);}

		if(RTPObject) {
			RTPObject->Destroy(RTPObject);  //bug maybe ---Jack
			RTPObject = NULL;
		}
		if(RtpDcMem) {
			RtpDcMem->Destroy(RtpDcMem);   //bug maybe
			RtpDcMem = NULL;
		}
	}
	else if (this->Flag == 1){
		if(RtpDcMemForGuest) {
			RtpDcMemForGuest->CloseMemory(RtpDcMemForGuest);   
		}	

		if(!EncodeTerminateForGuest){
			printf("Wait EncodeTerminateForGuest\n");
			EncodeTerminateForGuest = TRUE;
			while(!EncodeThreadExitForGuest) {usleep(10000);}
		}

		if(!SPITerminateForGuest){
			printf("Wait SPITerminateForGuest\n");
			SPITerminateForGuest= TRUE;
			while(!SPIThreadExitForGuest) {usleep(10000);}
		}

		if(RTPObjectForGuest) {
			RTPObjectForGuest->Destroy(RTPObjectForGuest);  
			RTPObjectForGuest = NULL;
		}
		if(RtpDcMemForGuest) {
			RtpDcMemForGuest->Destroy(RtpDcMemForGuest);  
			RtpDcMemForGuest= NULL;
		}
	}
	/*
	//�򿪼�����
	monitor->MonitorStart(Public.VideoType);
	*/
}
static void rtpDestroy(Rtp **this)
{
	free(*this);
	*this = NULL;
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief createRtp ����RTP�������
 *
 * @param h264_type_tmp ��Ƶ�ֱ��ʽṹ
 * @param abortCallBack ��Ƶ�ж�ʱ�ص�����
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
Rtp * createRtp(int video_type,
		void (*abortCallBack)(void *),void *callBackData)
{
	Rtp * this = (Rtp *)calloc(1,sizeof(Rtp));
	rtp_video_type = video_type;

	rtpAbortCallback = abortCallBack;
	call_back_data = callBackData;
	this->getFpAudio = rtpGetFpAudio;
	this->setSilence = rtpSetSilence;
	this->getSilence = rtpGetSilence;
	this->setPeerIp = rtpSetPeerIp;
	this->init = rtpInit;
	this->buildConnect = rtpBuildConnect;
	this->startAudio = rtpStartAudio;
	this->close = rtpClose;
	this->Destroy = rtpDestroy;

	return this;
}
