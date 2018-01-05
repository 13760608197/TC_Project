/*
 * =====================================================================================
 *
 *       Filename:  FormPassword.c
 *
 *    Description:  密码输入窗口
 *
 *        Version:  1.0
 *        Created:  2015-11-02 09:57:15
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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "externfunc.h"

/* ----------------------------------------------------------------*
 *                  extern variables declare
 *-----------------------------------------------------------------*/
extern PLOGFONT font14;
extern PLOGFONT font24;
extern PLOGFONT font18;

/* ----------------------------------------------------------------*
 *                  internal functions declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                        macro define
 *-----------------------------------------------------------------*/
#if DBG_PASSWORD > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

#define PWD_TITLE_FONT font24
#define PWD_REMINDER_FONT font18
#define EDIT_FONT font24

#define W_TITLE SCR_WIDTH
#define H_TITLE 50
#define X_TITLE 0
#define Y_TITLE 50

#define W_EDIT 200
#define H_EDIT 32
#define X_EDIT ((SCR_WIDTH - W_EDIT) / 2)
#define Y_EDIT Y_TITLE + H_TITLE

#define W_REMINDER_WRONG SCR_WIDTH
#define H_REMINDER_WRONG 30
#define X_REMINDER_WRONG 0
#define Y_REMINDER_WRONG Y_EDIT + H_EDIT

#define W_REMINDER SCR_WIDTH
#define H_REMINDER 30
#define X_REMINDER 0
#define Y_REMINDER SCR_HEIGHT - H_REMINDER

#define PASSWORD_NUM 8
/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
static BITMAP *Bmp_background = NULL; 
static ST_PASSWORD password;
static char Star[16];
static ENUM_TYPE_PASSWORD form_type;
static int auto_close_time;
static int Isdisplay = 0;

static MY_CTRLDATA ChildCtrls [] = {

	STATIC_LB(X_TITLE, Y_TITLE, W_TITLE, H_TITLE,
			IDC_PASSWORD_LB_TOP_WORD,
			"",
			0,
			&PWD_TITLE_FONT),

	STATIC_LB(X_REMINDER, Y_REMINDER, W_REMINDER, H_REMINDER,
			IDC_PASSWORD_LB_REMINDER,
			Word[WORD_HELP_PASSWORD].string,
			0,
			&PWD_REMINDER_FONT),

	STATIC_LB_DISVISIBLE(X_REMINDER_WRONG, Y_REMINDER_WRONG, W_REMINDER_WRONG, H_REMINDER_WRONG,
			IDC_PASSWORD_LB_REMINDER_WRONG,
			Word[WORD_INCORRECT_PASSWD].string,
			0,
			&PWD_REMINDER_FONT),

	EDIT(X_EDIT, Y_EDIT, W_EDIT, H_EDIT,
			IDC_PASSWORD_EDIT_PWD,
			"",
			0,
			&EDIT_FONT),
};

static MY_DLGTEMPLATE DlgInitParam =
{
    WS_NONE,
//   WS_EX_AUTOSECONDARYDC,
    WS_EX_NONE,
    0,0,SCR_WIDTH,SCR_HEIGHT,
    "密码输入窗口",
    0, 0,       //menu and icon is null
    sizeof(ChildCtrls)/sizeof(MY_CTRLDATA),
    ChildCtrls, //pointer to control array
    0           //additional data,must be zero
};

/* ----------------------------------------------------------------*/
/**
 * @brief Password_init 初始化密码
 */
/* ----------------------------------------------------------------*/
static void Password_init(void)
{
	password.index = 0;
	memset(password.buf,0,sizeof(password.buf));
	memset(Star,0,sizeof(Star));
}

/* ----------------------------------------------------------------*/
/**
 * @brief UpdateEdit 更新编辑框,*号
 *
 * @param hWnd
 */
/* ----------------------------------------------------------------*/
static void UpdateEdit(HWND hWnd)
{
	SetWindowText(GetDlgItem(hWnd,IDC_PASSWORD_EDIT_PWD),Star);
}

/* ----------------------------------------------------------------*/
/**
 * @brief UpdateNum 更新数字
 *
 * @param hWnd
 * @param num
 */
/* ----------------------------------------------------------------*/
static void UpdateNum(HWND hWnd, int num)
{
	if (password.index < PASSWORD_NUM) {
		Star[password.index] = '*';
		password.buf[password.index++] = num + '0';
	} else {
		password.buf[password.index] = '\0';
		Star[password.index] = '\0';
	}
	ShowWindow(GetDlgItem(hWnd,IDC_PASSWORD_LB_REMINDER_WRONG),SW_HIDE);
	SendMessage(hWnd,MSG_PSD_UPDATE_PASSWORD,0,0);//更新密码
}

/* ----------------------------------------------------------------*/
/**
 * @brief btConfirmClick 确认键
 *
 * @param hWnd
 */
/* ----------------------------------------------------------------*/
static void btConfirmClick(HWND hWnd)
{
	if (form_type == TYPE_PASSWORD_UNLOCK) {
		if(strcmp(password.buf,Public.UnlockPassword)==0) {
			Public.UnlockTheDoor(3);
			SendNotifyMessage(hWnd,MSG_CLOSE,0,1);
			return;
		}
	} else if (form_type == TYPE_PASSWORD_SETTING){
		if(strcmp(password.buf,Public.SystemPassword)==0) {
			Isdisplay = 0;
			Password_init();
			SendMessage(hWnd,MSG_PSD_UPDATE_PASSWORD,0,0);//更新密码
			CreateSettingForm(hWnd,Bmp_background);
			return;
		}
	}
	Password_init();
	SendMessage(hWnd,MSG_PSD_UPDATE_PASSWORD,0,0);//更新密码
	ShowWindow(GetDlgItem(hWnd,IDC_PASSWORD_LB_REMINDER_WRONG),SW_SHOWNORMAL);
}

/* ----------------------------------------------------------------*/
/**
 * @brief btCancelClick 取消键
 *
 * @param hWnd
 */
/* ----------------------------------------------------------------*/
static void btCancelClick(HWND hWnd)
{
	if (password.index > 0) {
		password.index--;
		password.buf[password.index] = '\0';
		Star[password.index] = '\0';
		SendMessage(hWnd,MSG_PSD_UPDATE_PASSWORD,0,0);//更新密码
		ShowWindow(GetDlgItem(hWnd,IDC_PASSWORD_LB_REMINDER_WRONG),SW_HIDE);
	} else {
		SendNotifyMessage(hWnd,MSG_CLOSE,0,0);
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
static void ProUserKeyBoard(HWND hWnd,int KeyVal)
{
	auto_close_time = FORM_SETTING_ONTIME;
	switch(KeyVal)
	{
			//退格键
		case KEYBACK:
			{
				btCancelClick(hWnd);
			} break;

			//留言键
		case KEYLEAVEWORD:
			{
				SendNotifyMessage(hWnd,MSG_CLOSE,0,0);
			} break;

		case KEYOK:
			{
				btConfirmClick(hWnd);
			} break;

			//其他数字按键
		default:
			{
				UpdateNum(hWnd,KeyVal);
			} break;
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief SettingPasswordProc 窗口回调函数
 *
 * @param hWnd
 * @param message
 * @param wParam
 * @param lParam
 *
 * @return
 */
/* ----------------------------------------------------------------*/
static int SettingPasswordProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_INITDIALOG:
			{
				Screen.Add(hWnd,"TFrmPassword");
				auto_close_time = FORM_SETTING_ONTIME;
				Isdisplay = 1;
				if (form_type == TYPE_PASSWORD_UNLOCK) {
					SetWindowText(GetDlgItem(hWnd,IDC_PASSWORD_LB_TOP_WORD),Word[WORD_INSERT_UNLOCK_PASSWD].string);
				} else if(form_type == TYPE_PASSWORD_SETTING) {
					SetWindowText(GetDlgItem(hWnd,IDC_PASSWORD_LB_TOP_WORD),Word[WORD_INSERT_SETTING_PASSWD].string);
				}
				SetWindowElementAttr(GetDlgItem(hWnd,IDC_PASSWORD_LB_TOP_WORD), WE_FGC_WINDOW, MY_WHIGHT);
				SetWindowElementAttr(GetDlgItem(hWnd,IDC_PASSWORD_LB_REMINDER), WE_FGC_WINDOW, MY_WHIGHT);
				SetWindowElementAttr(GetDlgItem(hWnd,IDC_PASSWORD_EDIT_PWD), WE_FGC_WINDOW, MY_WHIGHT);
				SetWindowElementAttr(GetDlgItem(hWnd,IDC_PASSWORD_LB_REMINDER_WRONG), WE_FGC_WINDOW, MY_RED);
				Password_init();
				SendMessage (GetDlgItem(hWnd,IDC_PASSWORD_EDIT_PWD), EM_LIMITTEXT, PASSWORD_NUM, 0L);//最多显示6个字符
				SetTimer(hWnd,IDC_PASSWORD_TIMER,TIME_1S);
			} break;

		case MSG_PSD_UPDATE_PASSWORD:
			UpdateEdit(hWnd);
			return 0;

		case MSG_KEYBOARD :
			ProUserKeyBoard(hWnd,wParam);
			break;

		case MSG_ERASEBKGND:
			if (Bmp_background != NULL)
				DrawBackground(hWnd, (HDC)wParam, (const RECT*)lParam,Bmp_background);
			return 0;

		case MSG_TIMER:
			{
				if (wParam == IDC_PASSWORD_TIMER){
					if ((auto_close_time > 0) && Isdisplay) {
						DBG_P("[%s]time:%d\n", __FILE__,auto_close_time);
						if (--auto_close_time == 0) {
							SendNotifyMessage(hWnd,MSG_CLOSE,0,0);
						}
					}
				}
			} break;

		case MSG_CLOSE:
			DestroyMainWindow (hWnd);
			MainWindowThreadCleanup (hWnd);
			return 0;

		case MSG_DESTROY:
			Screen.Del(hWnd);
			Public.m_bSetStatus = FALSE;
			DestroyAllControls (hWnd);
			return 0;
		default:
			break;
	}
    return DefaultDialogProc(hWnd, message, wParam, lParam);
}

/* ----------------------------------------------------------------*/
/**
 * @brief CreatePasswordForm 创建修改密码窗口
 *
 * @param hMainWnd
 * @param type 	TYPE_PASSWORD_UNLOCK开锁密码
 * 				TYPE_PASSWORD_SETTING设置密码
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
int CreatePasswordForm(HWND hMainWnd,ENUM_TYPE_PASSWORD type,BITMAP *Bmp_bkg)
{
	HWND Form = Screen.Find("TFrmPassword");
	form_type = type;
	Bmp_background = Bmp_bkg;
	if(Form) {
		ShowWindow(Form,SW_SHOWNORMAL);
		return Form;
	} else {
		return CreateMyWindowIndirectParam(&DlgInitParam,hMainWnd,SettingPasswordProc,0);
	}
}

