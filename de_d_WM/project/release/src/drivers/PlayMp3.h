/*
 * =====================================================================================
 *
 *       Filename:  PlayMp3.h
 *
 *    Description:  创建MP3接口
 *
 *        Version:  1.0
 *        Created:  2015-12-22 10:35:23 
 *       Revision:  1.0
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _PLAYMP3_H
#define _PLAYMP3_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

	enum MP3_STATUS {MP3_PLAY,MP3_PAUSE,MP3_STOP};
	struct _PrivatePlayMp3;

	typedef struct _PlayMp3Struct
	{
		struct _PrivatePlayMp3 *priv;
		int  (*GetPTime)(struct _PlayMp3Struct *this);
		int  (*GetPos)(struct _PlayMp3Struct *this);
		void  (*Seek)(struct _PlayMp3Struct *this,int Pos);
		int  (*GetStatus)(struct _PlayMp3Struct *this);
		int  (*Play)(struct _PlayMp3Struct *this,const char * pFileName,int Model,void (*CallbackProc)(void));   /// 0 is  单曲播放，1 是单曲循环播放
		void (*Stop)(struct _PlayMp3Struct *this);
		int  (*Pause)(struct _PlayMp3Struct *this);
		int  (*Resume)(struct _PlayMp3Struct *this);
		int  (*GetPVolume)(struct _PlayMp3Struct *this);
		int  (*SetPVolume)(struct _PlayMp3Struct *this,int Volume);
		void (*Destroy)(struct _PlayMp3Struct *this);
		void (*CallBackProc)(void);	//播放结束时的回调函数
	} PlayMp3Struct;

	PlayMp3Struct* playMp3Create(void *Mixer);
	extern PlayMp3Struct *PlayMp3;
	extern PlayMp3Struct *PlayUnlock;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
