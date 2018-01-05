/*
 * =====================================================================================
 *
 *       Filename:  FormPassword.h
 *
 *    Description:  输入密码窗口
 *
 *        Version:  1.0
 *        Created:  2015-11-02 09:06:21 
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _FORM_PASSWORD_H
#define _FORM_PASSWORD_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MSG_PSD_UPDATE_PASSWORD		(MSG_USER + 1)

	typedef enum _ENUM_TYPE_PASSWORD {
		TYPE_PASSWORD_UNLOCK,
		TYPE_PASSWORD_SETTING,
	}ENUM_TYPE_PASSWORD;

	enum {
		IDC_PASSWORD_TIMER,

		IDC_PASSWORD_LB_TOP_WORD,
		IDC_PASSWORD_LB_REMINDER_WRONG,
		IDC_PASSWORD_LB_REMINDER,

		IDC_PASSWORD_EDIT_PWD
	};

	typedef struct PASSWORD{
		int index;
		char buf[20];
	}ST_PASSWORD;


int CreatePasswordForm(HWND hMainWnd,ENUM_TYPE_PASSWORD type,BITMAP *Bmp_bkg);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif


