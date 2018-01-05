#ifndef _H264_DEFINE_H_
#define _H264_DEFINE_H_

// Constant value, don't change
/*
#define OUTPUT_FMT_CbYCrY       0
#define OUTPUT_FMT_RGB555       1
#define OUTPUT_FMT_RGB888       2
#define OUTPUT_FMT_RGB565       3
#define OUTPUT_FMT_YUV420       4
#define OUTPUT_FMT_YUV422       5
*/
#define USE_MMAP

#define TEST_WIDTH  176 //720
#define TEST_HEIGHT 144 //480

//-------------------------------------
// Decoder part
//-------------------------------------
#define FAVC_DECODER_DEV  "/dev/w55fa92_264dec"

//-------------------------------------
// Encoder part
//-------------------------------------
#define FAVC_ENCODER_DEV  "/dev/w55fa92_264enc"
#define RATE_CTL

#define FIX_QUANT	0
#define IPInterval_def	10
#define IPInterval_def_Guest	10


//-------------------------------------
// Data structure
//-------------------------------------
typedef struct {
    uint8_t *data[4];
} AVFrame;

typedef struct video_profile
{
    unsigned int bit_rate;
    unsigned int width;   //length per dma buffer
    unsigned int height;
    unsigned int framerate;
    unsigned int frame_rate_base;
    unsigned int gop_size;
    unsigned int qmax;
    unsigned int qmin;
    unsigned int quant;
    unsigned int intra;
    AVFrame *coded_frame;
    char *priv;
} video_profile;

typedef struct
{
	int width;
	int height;
}VedioInfo;

#endif //#ifndef _H264_DEFINE_H_
