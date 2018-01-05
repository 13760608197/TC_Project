/*
 * =====================================================================================
 *
 *       Filename:  FormSetTime.h
 *
 *    Description:  时间设置窗口
 *
 *        Version:  1.0
 *        Created:  2015-11-10 09:28:25 
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _SET_TIME_H
#define _SET_TIME_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MSG_SETTIME_UPDATE_REMINDER		 	(MSG_USER + 1)
#define MSG_SETTIME_UPDATE_REMINDER_BT		(MSG_USER + 2)
	enum {
		IDC_SETTIME_TIMER,
		
		IDC_SETTIME_LB_TITLE,
		IDC_SETTIME_LB_YEAR,
		IDC_SETTIME_LB_MONTH,
		IDC_SETTIME_LB_DAY,
		IDC_SETTIME_LB_HOUR,
		IDC_SETTIME_LB_MINUTE,
		IDC_SETTIME_LB_REMINDER,
		IDC_SETTIME_LB_REMINDER_BT,

		IDC_SETTIME_EDIT_YEAR,
		IDC_SETTIME_EDIT_MONTH,
		IDC_SETTIME_EDIT_DAY,
		IDC_SETTIME_EDIT_HOUR,
		IDC_SETTIME_EDIT_MINUTE,
		
		IDC_SETTIME_BT_SYNC,
		IDC_SETTIME_BT_SAVE,
		IDC_SETTIME_BT_BACK
	};

	enum {
		CTRL_TYPE_SETTIME_EDIT,
		CTRL_TYPE_SETTIME_BUTTON,
	};

	int CreateSetTimeForm(HWND hMainWnd,BITMAP *Bmp_bkg);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
