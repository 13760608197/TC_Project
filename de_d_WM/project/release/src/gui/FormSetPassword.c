/*
 * =====================================================================================
 *
 *       Filename:  FormSetPassword.c
 *
 *    Description:  设置密码窗口
 *
 *        Version:  1.0
 *        Created:  2015-11-08 14:05:28
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
/* ----------------------------------------------------------------*
 *                      include head files
 *-----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "externfunc.h"
/* ----------------------------------------------------------------*
 *                  extern variables declare
 *-----------------------------------------------------------------*/
extern PLOGFONT font14;
extern PLOGFONT font18;

/* ----------------------------------------------------------------*
 *                  internal functions declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                        macro define
 *-----------------------------------------------------------------*/
#if DBG_SET_PASSWORD > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

#if (defined TC3000_DE) || (defined TC3000_X6) || (defined TC3000_DK)
#define TEXT_FONT font18
#define BUTTON_FONT font14
#define AREA_WIDTH SCR_WIDTH - 20
#define FONT_HEIGHT 20
#define INTERVAL	5
#define BUTTON_WIDTH 50
#define BUTTON_HEIGHT FONT_HEIGHT + INTERVAL

#define W_LB_TITLE	SCR_WIDTH
#define H_LB_TITLE	FONT_HEIGHT
#define X_LB_TITLE	0
#define Y_LB_TITLE	5

#define W_LB_NAME	SCR_WIDTH / 5 * 4 + 10
#define H_LB_NAME	FONT_HEIGHT
#define X_LB_NAME	10
#define Y_LB_NAME	5 + H_LB_TITLE

#define W_EDIT	SCR_WIDTH / 5 * 1 - 10
#define H_EDIT	FONT_HEIGHT
#define X_EDIT	AREA_WIDTH - SCR_WIDTH / 5 * 1 + 5 + 10
#define Y_EDIT	Y_LB_NAME + H_LB_NAME + INTERVAL

#define W_BUTTON	80
#define H_BUTTON	FONT_HEIGHT
#define X_BUTTON	AREA_WIDTH - W_BUTTON + 5

#define _Y_LOCATION(x) Y_LB_NAME + ((FONT_HEIGHT+6) * x)
#endif

#if (defined TC3000_18S)
#define TEXT_FONT font18
#define BUTTON_FONT font14
#define AREA_WIDTH SCR_WIDTH - 20
#define FONT_HEIGHT 24
#define INTERVAL	6
#define BUTTON_WIDTH 50
#define BUTTON_HEIGHT FONT_HEIGHT + INTERVAL

#define W_LB_TITLE	SCR_WIDTH
#define H_LB_TITLE	FONT_HEIGHT
#define X_LB_TITLE	0
#define Y_LB_TITLE	5

#define W_LB_NAME	SCR_WIDTH / 5 * 4 + 10
#define H_LB_NAME	FONT_HEIGHT
#define X_LB_NAME	10
#define Y_LB_NAME	5 + H_LB_TITLE

#define W_EDIT	150
#define H_EDIT	FONT_HEIGHT
#define X_EDIT	320
#define Y_EDIT	Y_LB_NAME + H_LB_NAME + INTERVAL

#define W_BUTTON	80
#define H_BUTTON	FONT_HEIGHT
#define X_BUTTON	AREA_WIDTH - W_BUTTON + 5

#define _Y_LOCATION(x) Y_LB_NAME + ((FONT_HEIGHT+6) * x)
#endif
/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
static BITMAP *Bmp_background = NULL;
static HWND hCtrl;
static int auto_close_time;
static MY_CTRLDATA ChildCtrls [] = {
	STATIC_LB  (X_LB_TITLE, Y_LB_TITLE, W_LB_TITLE, H_LB_TITLE,
			IDC_SETPSD_BL_TILTE,
			Word[WORD_SETTING_PASSWORD].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(0), SCR_WIDTH, H_LB_NAME,
			IDC_SETPSD_BL_ATTENTION,
			Word[WORD_PLEASE_INSERT_SIX_PASSWORD].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(1), W_LB_NAME, H_LB_NAME,
			IDC_SETPSD_BL_SYSTEM_PASSWORD,
			Word[WORD_NEW_SYSTEM_PASSWORD].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(2), W_LB_NAME, H_LB_NAME,
			IDC_SETPSD_BL_SYSTEM_PASSWORD_CONFIRM,
			Word[WORD_NEW_SYSTEM_PASSWORD_CONFIRM].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(3), W_LB_NAME, H_LB_NAME,
			IDC_SETPSD_BL_UNLOCK_PASSWORD,
			Word[WORD_NEW_UNLOCK_PASSWORD].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(4), W_LB_NAME, H_LB_NAME,
			IDC_SETPSD_BL_UNLOCK_PASSWORD_CONFIRM,
			Word[WORD_NEW_UNLOCK_PASSWORD_CONFIRM].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(5), SCR_WIDTH - W_BUTTON, H_LB_NAME * 2,
			IDC_SETPSD_BL_REMINER,
			"",
			0,
			&BUTTON_FONT),
	STATIC_LB(X_LB_NAME, _Y_LOCATION(7), SCR_WIDTH - W_BUTTON, H_LB_NAME,
			IDC_SETPSD_BL_REMINER_BT,
			"",
			0,
			&TEXT_FONT),
	GROUP_EDIT_PSD_L(X_EDIT, _Y_LOCATION(1), W_EDIT, H_EDIT,
			IDC_SETPSD_EDIT_SYSTEM_PASSWORD,
			"",
			0,
			0),
	EDIT_PSD_L(X_EDIT, _Y_LOCATION(2), W_EDIT, H_EDIT,
			IDC_SETPSD_EDIT_SYSTEM_PASSWORD_CONFIRM,
			"",
			0,
			0),
	EDIT_PSD_L(X_EDIT, _Y_LOCATION(3), W_EDIT, H_EDIT,
			IDC_SETPSD_EDIT_UNLOCK_PASSWORD,
			"",
			0,
			0),
	EDIT_PSD_L(X_EDIT, _Y_LOCATION(4), W_EDIT, H_EDIT,
			IDC_SETPSD_EDIT_UNLOCK_PASSWORD_CONFIRM,
			"",
			0,
			0),
	BUTTON_LB(X_BUTTON, _Y_LOCATION(5), W_BUTTON, H_BUTTON,
			IDC_SETPSD_BT_SAVE,
			Word[WORD_SAVE].string,
			0,
			&BUTTON_FONT),
	BUTTON_LB(X_BUTTON, _Y_LOCATION(6), W_BUTTON, H_BUTTON,
			IDC_SETPSD_BT_BACK,
			Word[WORD_BACK].string,
			0,
			&BUTTON_FONT),
};

static MY_DLGTEMPLATE DlgInitParam =
{
    WS_NONE,
//   WS_EX_AUTOSECONDARYDC,
    WS_EX_NONE,
    0,0,SCR_WIDTH,SCR_HEIGHT,
    "",
    0, 0,       //menu and icon is null
    sizeof(ChildCtrls)/sizeof(MY_CTRLDATA),
    ChildCtrls, //pointer to control array
    0           //additional data,must be zero
};

/* ----------------------------------------------------------------*/
/**
 * @brief JudgeCtrlType 判断控件类型
 *
 * @param id 控件ID号
 *
 * @returns
		CTRL_TYPE_SETPSD_EDIT,   			编辑框
		CTRL_TYPE_SETPSD_BUTTON, 			按钮
 */
/* ----------------------------------------------------------------*/
static int JudgeCtrlType(int id)
{
	if (id >= IDC_SETPSD_BT_SAVE) {
		return CTRL_TYPE_SETPSD_BUTTON;
	} else {
		return CTRL_TYPE_SETPSD_EDIT;
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief UpdateBtReminder 更新按键提示语言
 *
 * @param hwnd
 * @param string
 */
/* ----------------------------------------------------------------*/
static void UpdateBtReminder(HWND hwnd,char *string)
{
	SendMessage(hwnd, MSG_SETPSD_UPDATE_REMINDER_BT,
			0,
			(LPARAM)string);
}


/* ----------------------------------------------------------------*/
/**
 * @brief UpdateReminder 更新底部提示语言
 *
 * @param hwnd
 * @param string
 */
/* ----------------------------------------------------------------*/
static void UpdateReminder(HWND hwnd,char *string)
{
	SendMessage(hwnd, MSG_SETPSD_UPDATE_REMINDER,
			0,
			(LPARAM)string);
}

/* ----------------------------------------------------------------*/
/**
 * @brief btSave 保存按钮
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void btSave(HWND hwnd, int id, int nc, DWORD add_data)
{
	char sys_password[16],sys_password_confirm[16];
	char unlock_password[16],unlock_password_confirm[16];
	char buf[32];
	int edit_text_len;
	edit_text_len = SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETPSD_EDIT_SYSTEM_PASSWORD),
			MSG_GETTEXT, 16, (LPARAM)sys_password );
	SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETPSD_EDIT_SYSTEM_PASSWORD_CONFIRM),
			MSG_GETTEXT, 16, (LPARAM)sys_password_confirm );
	if ((edit_text_len != 6) || (strcmp(sys_password,sys_password_confirm) != 0)) {
		sprintf(buf,"%s%s%s",
				Word[WORD_SYSTEM_PASSWORD].string,Word[WORD_SETTING].string,Word[WORD_FAILED].string);
		UpdateReminder(GetParent(hwnd),buf);
		return;
	}

	edit_text_len = SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETPSD_EDIT_UNLOCK_PASSWORD),
			MSG_GETTEXT, 16, (LPARAM)unlock_password );
	SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETPSD_EDIT_UNLOCK_PASSWORD_CONFIRM),
			MSG_GETTEXT, 16, (LPARAM)unlock_password_confirm );

	if ((edit_text_len != 6) || (strcmp(unlock_password,unlock_password_confirm) != 0)) {
		sprintf(buf,"%s%s%s",
				Word[WORD_UNLOCK_PASSWORD].string,Word[WORD_SETTING].string,Word[WORD_FAILED].string);
		UpdateReminder(GetParent(hwnd),buf);
		return;
	}
	memcpy(Public.SystemPassword,sys_password,sizeof(sys_password));
	SetValueToEtcFile("interface.ini","SECURE","SystemPassword",Public.SystemPassword);

	memcpy(Public.UnlockPassword,unlock_password,sizeof(unlock_password));
	SetValueToEtcFile_Ext("interface.ini","SECURE","UnlockPassword",Public.UnlockPassword);
	sprintf(buf,"%s%s",Word[WORD_SETTING].string,Word[WORD_SUCCESS].string);
	UpdateReminder(GetParent(hwnd),buf);
}

/* ----------------------------------------------------------------*/
/**
 * @brief btBack 返回按钮
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void btBack(HWND hwnd, int id, int nc, DWORD add_data)
{
	SendNotifyMessage(GetParent(hwnd),MSG_CLOSE,0,0);
}

/* ----------------------------------------------------------------*/
/**
 * @brief edit_notify 编辑框回调函数，用于获得焦点时设置光标位置
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void edit_notify(HWND hwnd, int id, int nc, DWORD add_data)
{
	char text[50];
	if (nc == EN_SETFOCUS){
		int len = SendMessage (GetDlgItem(GetParent(hwnd),id), MSG_GETTEXT, 50,  (LPARAM)text);
		SendMessage (GetDlgItem(GetParent(hwnd),id), EM_SETCARETPOS, 0, len);
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief ProUserKeyBoard 处理按键键值
 *
 * @param hWnd  主窗口句柄
 * @param KeyVal 键值
 */
/* ----------------------------------------------------------------*/
static void ProUserKeyBoard(HWND hDlg,int KeyVal)
{
	static int ctrl_type;
	auto_close_time = FORM_SETTING_ONTIME;
	switch(KeyVal)
	{
		case KEYBACK:
			{
				hCtrl = GetNextDlgTabItem (hDlg,hCtrl,FALSE);
				SendMessage(hDlg,MSG_KEYDOWN,SCANCODE_TAB,0);

				int id = GetDlgCtrlID(hCtrl);
				switch (JudgeCtrlType(id))
				{
					case CTRL_TYPE_SETPSD_EDIT:
						{
							if (ctrl_type != CTRL_TYPE_SETPSD_EDIT) {
								UpdateBtReminder(hDlg,Word[WORD_HELP_EDIT].string);
							}
							ctrl_type = CTRL_TYPE_SETPSD_EDIT;
						} break;

					case CTRL_TYPE_SETPSD_BUTTON:
						{
							if (ctrl_type != CTRL_TYPE_SETPSD_BUTTON) {
								UpdateBtReminder(hDlg,Word[WORD_HELP_BUTTON].string);
							}
							ctrl_type = CTRL_TYPE_SETPSD_BUTTON;
						} break;

					default:
						break;
				}
			} break;

		case KEYOK:
			{
				int id = GetDlgCtrlID(hCtrl);
				if (JudgeCtrlType(id) == CTRL_TYPE_SETPSD_EDIT) {
					SendMessage(hCtrl,MSG_CHAR,'\b',0);
				} else {
					SendMessage(hCtrl,BM_CLICK,0,0);
				}
			} break;

		default:
			{
				int id = GetDlgCtrlID(hCtrl);
				if (JudgeCtrlType(id) == CTRL_TYPE_SETPSD_EDIT) {
					if (KeyVal != KEYLEAVEWORD) {
						SendMessage(hCtrl,MSG_CHAR,'0' + KeyVal,0);
					}
				}
			} break;
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief SetPasswordProc 窗口回调函数
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 *
 * @return
 */
/* ----------------------------------------------------------------*/
static int SetPasswordProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_INITDIALOG:
			{
				Screen.Add(hDlg,"TFrmSetPsd");
				hCtrl = 0;
				auto_close_time = FORM_SETTING_ONTIME;
				int i;
				for (i=IDC_SETPSD_BL_TILTE;i<=IDC_SETPSD_BL_UNLOCK_PASSWORD_CONFIRM; i++){
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW,MY_WHIGHT);
				}
				for (i=IDC_SETPSD_EDIT_SYSTEM_PASSWORD;i<=IDC_SETPSD_EDIT_UNLOCK_PASSWORD_CONFIRM; i++){
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW,MY_BLACK);
					SendDlgItemMessage(hDlg,i, EM_LIMITTEXT, 6, 0L);
					SetNotificationCallback (GetDlgItem(hDlg,i), edit_notify);
				}
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_SETPSD_BL_REMINER),
						WE_FGC_WINDOW,COLOR_green);
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_SETPSD_BL_REMINER_BT),
						WE_FGC_WINDOW,COLOR_green);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_SETPSD_BT_SAVE), btSave);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_SETPSD_BT_BACK), btBack);

				UpdateBtReminder(hDlg,Word[WORD_HELP_BUTTON].string);
				SetTimer(hDlg,IDC_SETPSD_TIMER,TIME_1S);
			} break;

		case MSG_SETPSD_UPDATE_REMINDER :
			SetWindowText(GetDlgItem(hDlg,IDC_SETPSD_BL_REMINER),(char *)lParam);
			break;

		case MSG_SETPSD_UPDATE_REMINDER_BT :
			SetWindowText(GetDlgItem(hDlg,IDC_SETPSD_BL_REMINER_BT),(char *)lParam);
			break;

		case MSG_KEYBOARD :
			ProUserKeyBoard(hDlg,wParam);
			break;

		case MSG_ERASEBKGND:
			if (Bmp_background != NULL)
				DrawBackground(hDlg, (HDC)wParam, (const RECT*)lParam,Bmp_background);
			return 0;

		case MSG_TIMER:
			{
				if (wParam == IDC_SETPSD_TIMER){
					if (auto_close_time > 0) {
						DBG_P("[%s]time:%d\n", __FILE__,auto_close_time);
						if (--auto_close_time == 0) {
							SendNotifyMessage(hDlg,MSG_CLOSE,0,0);
						}
					}
				}
			} break;

		case MSG_CLOSE:
			{
				HWND Form = Screen.Find("TFrmSetting");
				SendNotifyMessage(Form,MSG_SETTING_RESET_TIME,0,0);
				KillTimer (hDlg,IDC_SETPSD_TIMER);
				DestroyMainWindow (hDlg);
				MainWindowThreadCleanup (hDlg);
			} return 0;

		case MSG_DESTROY:
			Screen.Del(hDlg);
			DestroyAllControls (hDlg);
			return 0;

		default:
			break;
	}
    return DefaultDialogProc(hDlg, message, wParam, lParam);
}

/* ----------------------------------------------------------------*/
/**
 * @brief CreateSetPasswordForm 创建设置密码窗口
 *
 * @param hMainWnd
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
int CreateSetPasswordForm(HWND hMainWnd,BITMAP *Bmp_bkg)
{
	HWND Form = Screen.Find("TFrmSetPsd");
	Bmp_background = Bmp_bkg;
	if(Form) {
		ShowWindow(Form,SW_SHOWNORMAL);
		return Form;
	} else {
		return CreateMyWindowIndirectParam(&DlgInitParam,hMainWnd,SetPasswordProc,0);
	}
}

