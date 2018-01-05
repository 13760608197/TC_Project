/*
 * =====================================================================================
 *
 *       Filename:  FormSystemPara.h
 *
 *    Description:  系统参数
 *
 *        Version:  1.0
 *        Created:  2015-11-06 10:39:30 
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _SYSTEM_PARA_H
#define _SYSTEM_PARA_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MSG_SYSPARA_UPDATE_REMINDER		 	(MSG_USER + 1)
#define MSG_SYSPARA_UPDATE_REMINDER_BT		(MSG_USER + 2)

	enum {
		IDC_SYSPARA_TIMER,

		IDC_SYSPARA_LB_TITLE,	
		IDC_SYSPARA_LB_LANGUAGE,
		IDC_SYSPARA_LB_VOLUME,
		IDC_SYSPARA_LB_FRAME_PERIOD,
		IDC_SYSPARA_LB_ALARM_DOOR,
		IDC_SYSPARA_LB_REMINDER,
		IDC_SYSPARA_LB_REMINDER_BT,

		IDC_SYSPARA_MENU_LANGUAGE,
		IDC_SYSPARA_MENU_VIDEO_STYLE,
		IDC_SYSPARA_MENU_VIDEO_RESOLUTION,

		IDC_SYSPARA_EDIT_VOLUME,
		IDC_SYSPARA_EDIT_FRAME_PERIOD,
		IDC_SYSPARA_EDIT_ALAMR_DOOR,

		IDC_SYSPARA_BT_ENABLE_ALARM_DOOR,
		IDC_SYSPARA_BT_ENABLE_PASSWORD,
		IDC_SYSPARA_BT_ENABLE_UNBROKEN,
		IDC_SYSPARA_BT_SAVE,
		IDC_SYSPARA_BT_BACK,
	};
	
	enum {
		CTRL_TYPE_SYSPARA_EDIT,
		CTRL_TYPE_SYSPARA_BUTTON,
		CTRL_TYPE_SYSPARA_MENU
	};
	int CreateSysParaForm(HWND hMainWnd,BITMAP *Bmp_bkg);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
