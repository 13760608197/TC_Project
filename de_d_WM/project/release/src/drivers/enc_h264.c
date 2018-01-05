/*
    sample code for H264 for pattern 720x480 input and bitstream output
    This sample code is to do encode 1000 stream frames named "/tmp/dev0.264"
    #./h264_main test.yuv
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

#include "favc_avcodec.h"
#include "favc_version.h"
#include "h264.h"

#include "Misc.h"
#include "V4L.h"
#include "enc_h264.h"
#include "externfunc.h"

static int			  favc_enc_fd1=0;
static unsigned int	  out_virt_buffer1;
static avc_workbuf_t  bitstreambuf;
static H264RateControl  h264_ratec;
static int  		  favc_quant=0;
static unsigned int img_width;
static unsigned int img_height;
static unsigned long int bitrate;

struct enc_h264_type h264_type[] = {
	{320,	240,	512000},
	{640,	480,	1024000},
	{1280,	720,	2048000},
};

static void H264RateControlInit(H264RateControl * rate_control,
				unsigned int target_rate,
				unsigned int reaction_delay_factor,
				unsigned int averaging_period,
				unsigned int buffer,
				float framerate,
				int max_quant,
				int min_quant,
				unsigned int initq,
				unsigned int IPInterval)
{
	int i;
  /*
	if(buffer <= 50){
		rate_control->frames = 0;
		rate_control->total_size = 0;
	}else{
		rate_control->frames = framerate * 1;
		rate_control->total_size = target_rate * 1 / 2;
	}
  */
	rate_control->frames = 0;
	rate_control->total_size = 0;

	//rate_control->framerate = (double) framerate / 1000.0;
	rate_control->framerate = (double) framerate;
	rate_control->IPInterval= IPInterval_def;
	rate_control->IPIntervalCnt= 0;
	rate_control->target_rate = (int) target_rate;
//	rate_control->rtn_quant = get_initial_quant(target_rate);
	rate_control->rtn_quant = initq;
	rate_control->pre_rtn_quant = initq;
	rate_control->max_quant = (short) max_quant;
	rate_control->min_quant = (short) min_quant;
	for (i = 0; i < rc_MAX_QUANT; ++i) {
		rate_control->quant_error[i] = 0.0;
	}
	rate_control->target_framesize =
		(double) target_rate / 8.0 / rate_control->framerate;
	rate_control->sequence_quality = 2.0 / (double) rate_control->rtn_quant;
	rate_control->avg_framesize = rate_control->target_framesize;
	rate_control->reaction_delay_factor = (int) reaction_delay_factor;
	rate_control->averaging_period = (int) averaging_period;
	rate_control->buffer = (int) buffer;
}


static void H264RateControlUpdate(H264RateControl *rate_control,
				  short quant,
				  int frame_size,
				  int keyframe)
{
	//long long deviation;
	int64_t deviation;
	double overflow, averaging_period, reaction_delay_factor;
	double quality_scale, base_quality, target_quality;
	int rtn_quant;

	rate_control->rtn_quant = rate_control->pre_rtn_quant;
	if ((quant == rate_control->min_quant) && (frame_size < rate_control->target_framesize))
		goto skip_integrate_err;
	else if ((quant == rate_control->max_quant) && (frame_size > rate_control->target_framesize))
		goto skip_integrate_err;

	rate_control->frames++;
	rate_control->total_size += frame_size;

skip_integrate_err:
	deviation =
		/*(long long)*/(int64_t) ((double) rate_control->total_size -
				   ((double) ((double) rate_control->target_rate / 8.0 /
					 (double) rate_control->framerate) * (double) rate_control->frames));

	if (rate_control->rtn_quant >= 2) {
		averaging_period = (double) rate_control->averaging_period;
		rate_control->sequence_quality -=
			rate_control->sequence_quality / averaging_period;
		rate_control->sequence_quality +=
			2.0 / (double) rate_control->rtn_quant / averaging_period;
		if (rate_control->sequence_quality < 0.1)
			rate_control->sequence_quality = 0.1;
		if (!keyframe) {
			reaction_delay_factor =
				(double) rate_control->reaction_delay_factor;
			rate_control->avg_framesize -=
				rate_control->avg_framesize / reaction_delay_factor;
			rate_control->avg_framesize += frame_size / reaction_delay_factor;
		}
	}

	quality_scale =
		rate_control->target_framesize / rate_control->avg_framesize *
		rate_control->target_framesize / rate_control->avg_framesize;

	base_quality = rate_control->sequence_quality;
	if (quality_scale >= 1.0) {
		base_quality = 1.0 - (1.0 - base_quality) / quality_scale;
	} else {
		//base_quality = 0.06452 + (base_quality - 0.06452) * quality_scale;
		base_quality = quality_const + (base_quality - quality_const) * quality_scale;
	}
	overflow = -((double) deviation / (double) rate_control->buffer);
  /*
	target_quality =
		base_quality + (base_quality -
						0.06452) * overflow / rate_control->target_framesize;*/
	target_quality =
		base_quality + (base_quality - quality_const) * overflow / rate_control->target_framesize;
	/*
	if (target_quality > 2.0)
		target_quality = 2.0;
	else if (target_quality < 0.06452)
		target_quality = 0.06452;*/
	if (target_quality > 2.0)
		target_quality = 2.0;
	else if (target_quality < quality_const)
		target_quality = quality_const;

	rtn_quant = (int) (2.0 / target_quality);
	if (rtn_quant < rc_MAX_QUANT) {
		rate_control->quant_error[rtn_quant] +=
			2.0 / target_quality - rtn_quant;
		if (rate_control->quant_error[rtn_quant] >= 1.0) {
			rate_control->quant_error[rtn_quant] -= 1.0;
			rtn_quant++;
		}
	}
	if(rate_control->framerate<=10) {
		if (rtn_quant > rate_control->rtn_quant + 3)
			rtn_quant = rate_control->rtn_quant + 3;
		else if (rtn_quant < rate_control->rtn_quant - 3)
			rtn_quant = rate_control->rtn_quant - 3;
	}else{
		if (rtn_quant > rate_control->rtn_quant + 1)
			rtn_quant = rate_control->rtn_quant + 1;
		else if (rtn_quant < rate_control->rtn_quant - 1)
			rtn_quant = rate_control->rtn_quant - 1;
	}

	if (rtn_quant > rate_control->max_quant)
		rtn_quant = rate_control->max_quant;
	else if (rtn_quant < rate_control->min_quant)
		rtn_quant = rate_control->min_quant;
	rate_control->pre_rtn_quant = rtn_quant;
	if(rate_control->framerate <= 10 && rate_control->target_rate <= 128000) {
		if(++rate_control->IPIntervalCnt % rate_control->IPInterval == 0) {
			rtn_quant -= 5;
			if(rtn_quant <= 25){
				rtn_quant = 25;
			}
			//rate_control->IPIntervalCnt =0;
		}
	}
	if(keyframe == 1)
		rate_control->IPIntervalCnt=1;
	rate_control->rtn_quant = rtn_quant;
}

static int h264enc_init(video_profile *video_setting)
{
    FAVC_ENC_PARAM     enc_param;
    int YUVoffset;

    // Open driver handler1
    if(favc_enc_fd1==0)
      favc_enc_fd1=open(FAVC_ENCODER_DEV,O_RDWR);

    if(favc_enc_fd1 <= 0)
    {
        printf("H1 Fail to open %s\n",FAVC_ENCODER_DEV);
        return -1;
    }


    //-----------------------------------------------
    //  driver handle 1
    //-----------------------------------------------
	// Get Bitstream Buffer Information
	if((ioctl(favc_enc_fd1, FAVC_IOCTL_ENCODE_GET_BSINFO, &bitstreambuf)) < 0)
	{
		close(favc_enc_fd1);
		printf("Get avc Enc bitstream info fail\n");
		return -1;
	}
//	printf("H1 Get Enc BS Buffer Physical addr = 0x%x, size = 0x%x,\n",bitstreambuf.addr,bitstreambuf.size);

	out_virt_buffer1 = (unsigned int)mmap(NULL, bitstreambuf.size, PROT_READ|PROT_WRITE, MAP_SHARED, favc_enc_fd1, bitstreambuf.offset);

	if((void *)out_virt_buffer1 == MAP_FAILED)
	{
		printf("Map ENC Bitstream Buffer Failed!\n");
		return -1;;
	}
//	printf("H1 Mapped ENC Bitstream Buffer Virtual addr = 0x%x\n",out_virt_buffer1);

    //-----------------------------------------------
    //  Issue Encode parameter to driver handle 1 & 2
    //-----------------------------------------------

    memset(&enc_param, 0, sizeof(FAVC_ENC_PARAM));

    enc_param.u32API_version = H264VER;

    enc_param.u32FrameWidth=video_setting->width;
    enc_param.u32FrameHeight=video_setting->height;

    enc_param.fFrameRate = video_setting->framerate;
    enc_param.u32IPInterval = video_setting->gop_size; //60, IPPPP.... I, next I frame interval
    enc_param.u32MaxQuant       =video_setting->qmax;
    enc_param.u32MinQuant       =video_setting->qmin;
    enc_param.u32Quant = video_setting->quant; //32
    enc_param.u32BitRate = video_setting->bit_rate;
    enc_param.ssp_output = -1;
    enc_param.intra = -1;

    enc_param.bROIEnable = 0;
    enc_param.u32ROIX = 0;
    enc_param.u32ROIY = 0;
    enc_param.u32ROIWidth = 0;
    enc_param.u32ROIHeight = 0;

#ifdef RATE_CTL
    memset(&h264_ratec, 0, sizeof(H264RateControl));
    H264RateControlInit(&h264_ratec, enc_param.u32BitRate,
        RC_DELAY_FACTOR,RC_AVERAGING_PERIOD, RC_BUFFER_SIZE_QUALITY, //RC_BUFFER_SIZE_BITRATE,
        (int)enc_param.fFrameRate,
        (int) enc_param.u32MaxQuant,
        (int)enc_param.u32MinQuant,
        (unsigned int)enc_param.u32Quant,
        enc_param.u32IPInterval);
#endif

    favc_quant = video_setting->quant;

//    printf("APP : FAVC_IOCTL_ENCODE_INIT\n");

    if (ioctl(favc_enc_fd1, FAVC_IOCTL_ENCODE_INIT, &enc_param) < 0)
    {
        close(favc_enc_fd1);
        printf("Handler_1 Error to set FAVC_IOCTL_ENCODE_INIT\n");
        return -1;
    }
//	printf("%s() End\n", __FUNCTION__);
    return 0;
}

static int h264enc_close(void)
{
//    printf("Enter enc_close\n");
    //---------------------------------
    //  Close driver handle 1
    //---------------------------------
    if(favc_enc_fd1) {
        close(favc_enc_fd1);
    }

    favc_enc_fd1 = 0;
    //printf("Enter enc_close\n");

//	if(out_virt_buffer1)
//		munmap((void *)out_virt_buffer1, bitstreambuf.size);
//	out_virt_buffer1 = 0;
    return 0;
}

static int favc_encode(int fileDecriptor, int *keyframe, unsigned char *frame, void * data)
{
    AVFrame             *pict=(AVFrame *)data;
    FAVC_ENC_PARAM     enc_param;

    enc_param.pu8YFrameBaseAddr = (unsigned char *)pict->data[0];   //input user continued virtual address (Y), Y=0 when NVOP
    enc_param.pu8UFrameBaseAddr = (unsigned char *)pict->data[1];   //input user continued virtual address (U)
    enc_param.pu8VFrameBaseAddr = (unsigned char *)pict->data[2];   //input user continued virtual address (V)

    enc_param.bitstream = frame;  //output User Virtual address
    enc_param.ssp_output = -1;
    enc_param.intra = -1;
    enc_param.u32IPInterval = 0; // use default IPInterval that set in INIT

    enc_param.u32Quant = favc_quant;
    enc_param.bitstream_size = 0;

    //printf("APP : FAVC_IOCTL_ENCODE_FRAME\n");
    if (ioctl(fileDecriptor, FAVC_IOCTL_ENCODE_FRAME, &enc_param) < 0)
    {
        printf("Dev =%d Error to set FAVC_IOCTL_ENCODE_FRAME\n", fileDecriptor);
        return 0;
    }

#ifdef RATE_CTL
    if (enc_param.keyframe == 0) {
        //printf("%d %d %d\n", enc_param.u32Quant, enc_param.bitstream_size, 0);
        H264RateControlUpdate(&h264_ratec, enc_param.u32Quant, enc_param.bitstream_size , 0);
    } else  {
        //printf("%d %d %d\n", enc_param.u32Quant, enc_param.bitstream_size, 1);
        H264RateControlUpdate(&h264_ratec, enc_param.u32Quant, enc_param.bitstream_size , 1);
    }
    favc_quant = h264_ratec.rtn_quant;

    //printf(" favc_quant = %d\n",favc_quant);

    //H264RateControlUpdate(&h264_ratec,enc_param.bitstream_size,enc_param.frame_cost);
#endif

    *keyframe = enc_param.keyframe;

    return enc_param.bitstream_size;
}

int videoin_init(int video_type)
{
	unsigned int y_image_size;
	video_profile video_setting;
	img_width = h264_type[video_type].img_width;
	img_height = h264_type[video_type].img_height;
	bitrate = h264_type[video_type].bitrate;

	video_setting.qmax = 51;
	video_setting.qmin = 0;
#ifdef RATE_CTL
	if (img_width > 640) {
#ifdef TC3000_BZ
		video_setting.quant = 40;				// Init Quant
#else
		video_setting.quant = 51;				// Init Quant
#endif
	} else {
		video_setting.quant = 30;				// Init Quant
	}
#else
	video_setting.quant = FIX_QUANT;
#endif
	video_setting.bit_rate = bitrate;
	video_setting.width = img_width;
	video_setting.height = img_height;
	video_setting.framerate = 15; // 30;
	video_setting.frame_rate_base = 1;
	video_setting.gop_size = IPInterval_def;

	y_image_size = video_setting.width*video_setting.height;
	if(camera_init(video_setting.width, video_setting.height) < 0) {
		printf("camera_init Failed!\n");
		return -1;
	}
	// printf("camera_init() ok!\n");
    if(h264enc_init(&video_setting) < 0) {
		printf("h264enc_init Failed!\n");
		if(out_virt_buffer1)
			munmap((void *)out_virt_buffer1, bitstreambuf.size);
		return -1;
    }
	// printf("h264enc_init() ok!\n");
	return 0;
}

//#define ENC_OUTH264FILE
#ifdef ENC_OUTH264FILE
FILE            *enc_h264file = NULL;
#define enc_h264file_name  "./encode.264"
#endif

void videoin_close(void)
{
	if(out_virt_buffer1)
		munmap((void *)out_virt_buffer1, bitstreambuf.size);
	out_virt_buffer1 = 0;

	DelayMs(50);
	h264enc_close();
	DelayMs(50);
	//camera_close();

  #ifdef ENC_OUTH264FILE
	if(enc_h264file)
		fclose(enc_h264file);
	enc_h264file=NULL;
  #endif
}

//#define SAVEONFRAME
#ifdef SAVEONFRAME
#define File264  "./Test.264"
FILE       *fileh264 = NULL;
#endif

int videoin_encode(unsigned char **vData, int *keyframe)
{
#ifdef SAVEONFRAME
	static int k=0;
#endif
	int length;
	unsigned int y_image_size;
	AVFrame pict;
	uint8_t *pu8PicPtr;
	uint64_t u64TS;
	uint32_t u32PicPhyAdr;

	if(out_virt_buffer1 == 0)
	{
		DEBUG_PRINT("[Error]: out_virt_buffer1=0\n");
		return 0;
	}

    if(ReadV4LPicture(&pu8PicPtr, &u64TS, &u32PicPhyAdr) == ERR_V4L_SUCCESS) {
	    TriggerV4LNextFrame();
    } else {
		printf("ReadV4LPicture Failed!--------------\n");
    }
    y_image_size = img_width * img_height;
    pict.data[0]=(unsigned char *)u32PicPhyAdr;
    pict.data[1]=(unsigned char *)(u32PicPhyAdr + y_image_size);
    pict.data[2]=(unsigned char *)(u32PicPhyAdr + y_image_size +(y_image_size >> 2));

//	length = favc_encode(favc_enc_fd1, keyframe, vData,(void *)&pict);
	length = favc_encode(favc_enc_fd1, keyframe, (unsigned char *)out_virt_buffer1,(void *)&pict);
	if(length > 0)
	{
		*vData = (unsigned char *)out_virt_buffer1;

#ifdef SAVEONFRAME
		if(*keyframe==1)
		{
			k++;
			if(k==2)
			{
				int i;
				fileh264=fopen(File264,"wb");
				fwrite((void *)out_virt_buffer1, sizeof(char), length, fileh264);
				fclose(fileh264);
				fileh264=NULL;
				printf("[ENC] Size=%d\n", length);
			}
		}
#endif
	}
//		memcpy(vData, out_virt_buffer1, length);

//	printf("\n");
//	for(i=0; i<30; i++)
//	{
//		printf("0x%02X,", vData[i]);
//	}
//	printf("\n");

  #ifdef ENC_OUTH264FILE
	if(enc_h264file==NULL)
	{
		enc_h264file=fopen(enc_h264file_name,"wb");
		printf("enc_h264file=%d\n", enc_h264file);
	}
	if((enc_h264file!=-1) && (length>0))
	{
		fwrite((void *)vData, 1, length, enc_h264file);
	}
	printf("h264size=%d\n", length);
  #endif
	return length;
}
