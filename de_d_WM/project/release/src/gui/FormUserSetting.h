/*
 * =====================================================================================
 *
 *       Filename:  FormUserSetting.h
 *
 *    Description:  用户设置
 *
 *        Version:  1.0
 *        Created:  2015-11-02 16:34:13 
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _USER_SETTING_H
#define _USER_SETTING_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MSG_USERSETTING_UPDATE_REMINDER		 	(MSG_USER + 1)
#define MSG_USERSETTING_UPDATE_REMINDER_BT		(MSG_USER + 2)
#define MSG_USERSETTING_UPDATE_REMINDER_LIST	(MSG_USER + 3)
	enum {
		IDC_USERSET_TIMER,

		IDC_USERSET_LB_ROOMNUM,
		IDC_USERSET_LB_IP,
		IDC_USERSET_LB_LISTCOUNT,
		IDC_USERSET_LB_REMINDER,
		IDC_USERSET_LB_REMINDER_BT,

		IDC_USERSET_BT_ADD,
		IDC_USERSET_BT_DEL,
		IDC_USERSET_BT_CLEAR,
		IDC_USERSET_BT_FIND,
		IDC_USERSET_BT_BACK,

		IDC_USERSET_EDIT_ROOMNUM,
		IDC_USERSET_EDIT_IP,

		IDC_USERSET_LIST_ROOMNUM,

		IDC_USERSET_NUM
	};
	
	typedef struct _ListData {
		int index;
		char data[64];
	}ListData;
	int CreateUserSettingForm(HWND hMainWnd,BITMAP *Bmp_bkg);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
