#ifndef  _ENC_H264_H
#define  _ENC_H264_H

#ifdef __cplusplus
extern "C"
{
#endif

//#define     ENC_IMG_WIDTH   640
//#define     ENC_IMG_HEIGHT  480
//#define		ENC_BITRATE		1024000

//#define     ENC_IMG_WIDTH   320
//#define     ENC_IMG_HEIGHT  240
//#define		ENC_BITRATE		512000

// #define     ENC_IMG_WIDTH   1280
// #define     ENC_IMG_HEIGHT  720
// #define		ENC_BITRATE		(512000 * 4)
	struct enc_h264_type {
		unsigned int img_width;
		unsigned int img_height;
		unsigned long int bitrate;		
	};

	extern int videoin_init(int video_type);
	extern int videoin_encode(unsigned char **vData, int *keyframe);
	extern void videoin_close(void);
	extern struct enc_h264_type h264_type[];

#ifdef __cplusplus
}
#endif

#endif
