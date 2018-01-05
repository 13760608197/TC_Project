/*
 * =====================================================================================
 *
 *       Filename:  FormSetting.h
 *
 *    Description:  系统设置初始界面，提示对应按键选择功能
 *
 *        Version:  virsion
 *        Created:  2015-11-02 10:12:51 
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _FORM_SETTING_H
#define _FORM_SETTING_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MSG_SETTING_RESET_TIME		 	(MSG_USER + 1)
#define MSG_MANUAL_SET_DEVECE_TYPE		(MSG_USER + 2)

	enum {
		IDC_SETTING_TIMER,

		IDC_SETTING_LB_TITLE,
		IDC_SETTING_LB_SOFTWARE_VERSION,
		IDC_SETTING_LB_DATE,
		IDC_SETTING_LB_KERNEL_VERSION,
		IDC_SETTING_LB_SINGLE_VERSION,
		IDC_SETTING_LB_HELP,
		IDC_SETTING_LB_OP1,
		IDC_SETTING_LB_OP2,
		IDC_SETTING_LB_OP3,
		IDC_SETTING_LB_OP4,
		IDC_SETTING_LB_OP5,
		IDC_SETTING_LB_OP6,
		IDC_SETTING_LB_OP7,
		IDC_SETTING_LB_OP8,
		IDC_SETTING_LB_OP9,
	};

int CreateSettingForm(HWND hMainWnd,BITMAP *Bmp_bkg);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
