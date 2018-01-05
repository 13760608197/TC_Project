/*
 * =====================================================================================
 *
 *       Filename:  FormUpdate.h
 *
 *    Description:  升级窗口
 *
 *        Version:  1.0
 *        Created:  2015-11-20 10:59:32 
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _FORMUPDATE_H
#define _FORMUPDATE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MSG_UPDATE_SUCCESS		 	(MSG_USER + 1)
#define MSG_UPDATE_FAIL			 	(MSG_USER + 2)
#define MSG_UPDATE_IC_SUCCESS		(MSG_USER + 3)
#define MSG_UPDATE_USER_SUCCESS		(MSG_USER + 4)
#define MSG_UPDATE_WTD				(MSG_USER + 5)

	enum {
	   UPDATE_TYPE_PROCESS,
	   UPDATE_TYPE_RECOVER,
	   UPDATE_TYPE_UNBROKEN,
	   UPDATE_TYPE_IC_CARD,
	   UPDATE_TYPE_USER,
	};
	enum {
		IDC_UPDATE_TIMER1,
		IDC_UPDATE_TIMER2,
		IDC_UPDATE_TIMER_WTD,

		IDC_UPDATE_LB_REMINDER
	};

	int CreateUpdateForm(HWND hMainWnd,
			BITMAP *Bmp_bkg,
			void *arg,
			int form_type);
	// int CreateUpdateForm(HWND hMainWnd,BITMAP *Bmp_bkg,void *arg);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
