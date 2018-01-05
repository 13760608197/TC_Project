/*
 * =====================================================================================
 *
 *       Filename:  FormSetAccess.h
 *
 *    Description:  门禁设置
 *
 *        Version:  1.0
 *        Created:  2015-11-09 09:52:27 
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _SET_ACCESS_H
#define _SET_ACCESS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


	enum{
		IDC_SETACCESS_TIMER,

		IDC_SETACCESS_LB_ROOM_NUM,
		IDC_SETACCESS_LB_IP,
		IDC_SETACCESS_LB_CARD_NUM,
		IDC_SETACCESS_LB_CARD_TYPE,
		IDC_SETACCESS_LB_LIST,
		IDC_SETACCESS_LB_REMINDER,

		IDC_SETACCESS_MENU_CARD_TYPE,

		IDC_SETACCESS_EDIT_ROOM_NUM,
		IDC_SETACCESS_EDIT_CARD_NUM,
		IDC_SETACCESS_EDIT_IP,

		IDC_SETACCESS_BT_ADD,
		IDC_SETACCESS_BT_DELETE,
		IDC_SETACCESS_BT_CLEAR,
		IDC_SETACCESS_BT_BACK,


		IDC_SETACCESS_LISTVIEW
	};

	enum {
		CTRL_TYPE_SETACCESS_MENU,
		CTRL_TYPE_SETACCESS_EDIT,
		CTRL_TYPE_SETACCESS_EDIT_IP,
		CTRL_TYPE_SETACCESS_BUTTON,
	};

	int CreateSetAccessForm(HWND hMainWnd,BITMAP *Bmp_bkg);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
