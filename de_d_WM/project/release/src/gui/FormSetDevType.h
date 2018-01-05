/*
 * =====================================================================================
 *
 *       Filename:  FormSetDevType.h
 *
 *    Description:  设备机型设置
 *
 *        Version:  1.0
 *        Created:  2017-11-03 09:08:21
 *       Revision:  none
 *
 *         Author:  LEE
 *        Company:  Taichuan
 *
 * =====================================================================================
 */

#ifndef _SET_DEVTYPE_H
#define _SET_DEVTYPE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MSG_DEVTYPE_UPDATE_REMINDER		 	(MSG_USER + 1)

enum {
	CTRL_TYPE_DEVTYPE_BUTTON,
	CTRL_TYPE_DEVTYPE_MENU
};

enum {
	IDC_DEVTYPE_TIMER = 100,
		
	IDC_DEVTYPE_LB_TITLE,

	IDC_DEVTYPE_LB_BUILD,
	IDC_DEVTYPE_LB_UNIT,
	IDC_DEVTYPE_LB_IP,
	IDC_DEVTYPE_LB_MASK,
	IDC_DEVTYPE_LB_GATEWAY,

	IDC_DEVTYPE_LB_BUILD_VALUE,
	IDC_DEVTYPE_LB_UNIT_VALUE,
	IDC_DEVTYPE_LB_IP_VALUE,
	IDC_DEVTYPE_LB_MASK_VALUE,
	IDC_DEVTYPE_LB_GATEWAY_VALUE,
	
	IDC_DEVTYPE_BT_SAVE,
	IDC_DEVTYPE_BT_BACK,

	IDC_DEVTYPE_LB_NOTY,

};


	int CreateSetDevTypeForm(HWND hMainWnd,BITMAP *Bmp_bkg);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
