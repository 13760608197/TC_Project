/*
 * =====================================================================================
 *
 *       Filename:  FormUserSeach.h
 *
 *    Description: 	户主查询
 *
 *        Version:  1.0
 *        Created:  2017-08-07 17:15:15
 *       Revision:  none
 *
 *         Author:  LEE
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _FORM_USERSEARCH_H
#define _FORM_USERSEARCH_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

enum {
	IDC_USERSEARCH_TIMER,
	IDC_USERSEARCH_TITLE,
	IDC_USERSEARCH_TREEVIEW,
	IDC_USERSEARCH_HELP
};


int CreateUserSeachForm(HWND hMainWnd,BITMAP *Bmp_bkg);
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif


