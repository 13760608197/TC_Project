/*
 * =====================================================================================
 *
 *       Filename:  FormTopMessage.c
 *
 *    Description:  弹出窗口
 *
 *        Version:  1.0
 *        Created:  2015-11-12 16:59:51
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
#if DBG_TOPMSG > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

#define TEXT_FONT font18
#define BUTTON_FONT font14
#define AREA_WIDTH SCR_WIDTH - 20
#define FONT_HEIGHT 20
#define INTERVAL	5
#define BUTTON_HEIGHT FONT_HEIGHT + INTERVAL

#define W_LB_TITLE SCR_WIDTH
#define H_LB_TITLE 50
#define X_LB_TITLE 0
#define Y_LB_TITLE 50

#define W_LB_NAME	SCR_WIDTH / 10 * 2
#define H_LB_NAME	FONT_HEIGHT
#define X_LB_NAME	0
#define Y_LB_NAME	5

#define W_EDIT	SCR_WIDTH / 10 * 3
#define H_EDIT	FONT_HEIGHT
#define X_EDIT	X_LB_NAME + W_LB_NAME
#define Y_EDIT	Y_LB_NAME + H_LB_NAME + INTERVAL

#define W_BUTTON	70
#define H_BUTTON	FONT_HEIGHT
#define X_BUTTON	SCR_WIDTH / 10 * 2

#define _Y_LOCATION(x) Y_LB_NAME + ((FONT_HEIGHT+6) * x)
#define _X_BUTTON_LOCATION(x) X_BUTTON + ((W_BUTTON+2) * x)
/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
static BITMAP *Bmp_background = NULL;
static int auto_close_time;
struct topmsg_st *topmsg;

static MY_CTRLDATA ChildCtrls [] = {
	STATIC_LB  (X_LB_TITLE, Y_LB_TITLE, W_LB_TITLE, H_LB_TITLE,
			IDC_TOPMESSAGE_LB_TITLE,
			"11",
			0,
			&TEXT_FONT),
	STATIC_LB(0, SCR_HEIGHT - 30, SCR_WIDTH, 30,
			IDC_TOPMESSAGE_LB_REMINDER,
			Word[WORD_HELP_TOPMESSAGE].string,
			0,
			&TEXT_FONT),
	BUTTON_LB(X_BUTTON, _Y_LOCATION(6), W_BUTTON, H_BUTTON,
			IDC_TOPMESSAGE_BT_CONFIRM,
			Word[WORD_BACK].string,
			0,
			&BUTTON_FONT),
	BUTTON_LB(SCR_WIDTH / 2 + X_BUTTON, _Y_LOCATION(6), W_BUTTON, H_BUTTON,
			IDC_TOPMESSAGE_BT_CANCEL,
			Word[WORD_CONFIRM].string,
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
 * @brief ProUserKeyBoard 处理按键键值
 *
 * @param hWnd  主窗口句柄
 * @param KeyVal 键值
 */
/* ----------------------------------------------------------------*/
static void ProUserKeyBoard(HWND hDlg,int KeyVal)
{
	auto_close_time = FORM_SETTING_ONTIME;
	switch(KeyVal)
	{
		case KEYBACK:
			{
				if (topmsg->notify_func) {
					topmsg->notify_func(hDlg, 0, FALSE,0);
				}
				SendNotifyMessage(hDlg,MSG_CLOSE,0,0);
			} break;

		case KEYOK:
			{
				if (topmsg->notify_func) {
					topmsg->notify_func(hDlg, 0, TRUE,0);
				}
				SendNotifyMessage(hDlg,MSG_CLOSE,0,0);
			} break;

		default:
			break;
	}
}


/* ----------------------------------------------------------------*/
/**
 * @brief TopMessageProc 窗口回调函数
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 *
 * @return
 */
/* ----------------------------------------------------------------*/
static int TopMessageProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_INITDIALOG:
			{
				Screen.Add(hDlg,"TFrmTop");
				auto_close_time = FORM_SETTING_ONTIME;
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_TOPMESSAGE_LB_TITLE),
						WE_FGC_WINDOW,MY_RED);
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_TOPMESSAGE_LB_REMINDER),
						WE_FGC_WINDOW,MY_RED);
				SetWindowText(GetDlgItem(hDlg,IDC_TOPMESSAGE_LB_TITLE),topmsg->title);
				SetTimer(hDlg,IDC_TOPMESSAGE_TIMER,TIME_1S);
			} break;

		case MSG_KEYBOARD :
			ProUserKeyBoard(hDlg,wParam);
			break;

		case MSG_ERASEBKGND:
			if (Bmp_background != NULL)
				DrawBackground(hDlg, (HDC)wParam, (const RECT*)lParam,Bmp_background);
			return 0;

		case MSG_TIMER:
			{
				if (wParam == IDC_TOPMESSAGE_TIMER){
					if (auto_close_time > 0) {
						DBG_P("[%s]time:%d\n", __FILE__,auto_close_time);
						if (--auto_close_time == 0) {
							if (topmsg->notify_func) {
								topmsg->notify_func(hDlg, 0, FALSE,0);
							}
							SendNotifyMessage(hDlg,MSG_CLOSE,0,0);
						}
					}
				}
			} break;

		case MSG_CLOSE:
			DestroyMainWindow (hDlg);
			MainWindowThreadCleanup (hDlg);
			return 0;

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
 * @brief CreateTopMessageForm 创建弹出提示窗口
 *
 * @param hMainWnd
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
int CreateTopMessageForm(HWND hMainWnd,BITMAP *Bmp_bkg,struct topmsg_st *topmsg_data)
{
	HWND Form = Screen.Find("TFrmTop");
	Bmp_background = Bmp_bkg;
	topmsg = topmsg_data;
	if(Form) {
		ShowWindow(Form,SW_SHOWNORMAL);
		return Form;
	} else {
		return CreateMyWindowIndirectParam(&DlgInitParam,hMainWnd,TopMessageProc,0);
	}
}

