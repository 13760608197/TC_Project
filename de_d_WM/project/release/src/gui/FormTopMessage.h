/*
 * =====================================================================================
 *
 *       Filename:  FormTopMessage.h
 *
 *    Description:  弹出式对话框
 *
 *        Version:  1.0
 *        Created:  2015-11-12 16:41:43 
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _TOP_MESSAGE_H
#define _TOP_MESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MSG_UPDATE_TOPMESSAGE		 	(MSG_USER + 1)
	enum {
		IDC_TOPMESSAGE_TIMER,

		IDC_TOPMESSAGE_LB_TITLE,
		IDC_TOPMESSAGE_LB_REMINDER,

		IDC_TOPMESSAGE_BT_CONFIRM,
		IDC_TOPMESSAGE_BT_CANCEL,
	};

	struct topmsg_st {
		char *title;
		void (*notify_func)(HWND hwnd, int id, int nc, DWORD add_data);
	};

	int CreateTopMessageForm(HWND hMainWnd,BITMAP *Bmp_bkg,struct topmsg_st *topmsg_data);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
