/*
 * =====================================================================================
 *
 *       Filename:  FormSetElevator.h
 *
 *    Description:  设置电梯界面
 *
 *        Version:  1.0
 *        Created:  2015-11-10 14:47:17 
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _SET_ELEVATOR_H
#define _SET_ELEVATOR_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MSG_SETELE_UPDATE_REMINDER		 	(MSG_USER + 1)
#define MSG_SETELE_UPDATE_REMINDER_BT		(MSG_USER + 2)

	enum {
		IDC_SETELE_TIMER,

		IDC_SETELE_LB_TITLE,	
		IDC_SETELE_LB_ADDRESS,
		IDC_SETELE_LB_FLOOR,
		IDC_SETELE_LB_LOBBY,
		IDC_SETELE_LB_REMINDER,
		IDC_SETELE_LB_REMINDER_BT,

		IDC_SETELE_EDIT_ADDRESS,
		IDC_SETELE_EDIT_FLOOR,
		IDC_SETELE_EDIT_LOBBY,

		IDC_SETELE_BT_ENABLE_EVEVATOR,
		IDC_SETELE_BT_SAVE,
		IDC_SETELE_BT_BACK	
	};

	enum {
		CTRL_TYPE_SETELE_BUTTON,
		CTRL_TYPE_SETELE_EDIT,
	};

	int CreateSetElevatorForm(HWND hMainWnd,BITMAP *Bmp_bkg);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
