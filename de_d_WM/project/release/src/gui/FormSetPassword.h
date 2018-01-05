/*
 * =====================================================================================
 *
 *       Filename:  FormSetPassword.h
 *
 *    Description:  密码设置
 *
 *        Version:  1.0
 *        Created:  2015-11-08 14:08:28 
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _SET_PASSWORD_H
#define _SET_PASSWORD_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MSG_SETPSD_UPDATE_REMINDER		 	(MSG_USER + 1)
#define MSG_SETPSD_UPDATE_REMINDER_BT		(MSG_USER + 2)
	enum {
		IDC_SETPSD_TIMER,

		IDC_SETPSD_BL_TILTE,
		IDC_SETPSD_BL_ATTENTION,
		IDC_SETPSD_BL_SYSTEM_PASSWORD,
		IDC_SETPSD_BL_SYSTEM_PASSWORD_CONFIRM,
		IDC_SETPSD_BL_UNLOCK_PASSWORD,
		IDC_SETPSD_BL_UNLOCK_PASSWORD_CONFIRM,
		IDC_SETPSD_BL_REMINER,
		IDC_SETPSD_BL_REMINER_BT,

		IDC_SETPSD_EDIT_SYSTEM_PASSWORD,
		IDC_SETPSD_EDIT_SYSTEM_PASSWORD_CONFIRM,
		IDC_SETPSD_EDIT_UNLOCK_PASSWORD,
		IDC_SETPSD_EDIT_UNLOCK_PASSWORD_CONFIRM,

		IDC_SETPSD_BT_SAVE,
		IDC_SETPSD_BT_BACK	
	};

	enum {
		CTRL_TYPE_SETPSD_EDIT,
		CTRL_TYPE_SETPSD_BUTTON,
	};

int CreateSetPasswordForm(HWND hMainWnd,BITMAP *Bmp_bkg);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
