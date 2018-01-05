/*
 * =====================================================================================
 *
 *       Filename:  FormNetSetting.h
 *
 *    Description:  网络参数设置窗口
 *
 *        Version:  1.0
 *        Created:  2015-11-05 14:07:24 
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _NET_SETTING_H
#define _NET_SETTING_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MSG_NETSET_UPDATE_REMINDER		 	(MSG_USER + 1)
#define MSG_NETSET_UPDATE_REMINDER_BT		(MSG_USER + 2)

	enum {
		IDC_NETSET_TIMER,
		IDC_NETSET_TIMER_GETID,

		IDC_NETSET_LB_TITLE,
		IDC_NETSET_LB_IP,
		IDC_NETSET_LB_MASK,
		IDC_NETSET_LB_GATEWAY,
		IDC_NETSET_LB_CENTER_IP,
		IDC_NETSET_LB_IMEI,
		IDC_NETSET_LB_MAC,
		IDC_NETSET_LB_MAC_ADD,
		IDC_NETSET_LB_REMINDER,
		IDC_NETSET_LB_REMINDER_BT,

		IDC_NETSET_EDIT_IP,
		IDC_NETSET_EDIT_MASK,
		IDC_NETSET_EDIT_GATEWAY,
		IDC_NETSET_EDIT_CENTER_IP,
		IDC_NETSET_EDIT_IMEI,
		
		IDC_NETSET_BT_GET_IMEI,
		IDC_NETSET_BT_SAVE,
		IDC_NETSET_BT_BACK

	};


	int CreateNetSettingForm(HWND hMainWnd,BITMAP *Bmp_bkg);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
