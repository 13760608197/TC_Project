/*
 * =====================================================================================
 *
 *       Filename:  adpcm.h
 *
 *    Description:  adpc编码
 *
 *        Version:  1.0
 *        Created:  2016-03-03 13:21:44 
 *       Revision:  1.0
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _ADPCM_H
#define _ADPCM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

	void adpcm_init();
	int adpcm_coder(short *indata,char *outdata,int len);
	int adpcm_decoder(char *indata,short *outdata,int len);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
