/*
 * =====================================================================================
 *
 *       Filename:  FormSetTime.c
 *
 *    Description:  时间设置窗口
 *
 *        Version:  1.0
 *        Created:  2015-11-10 09:27:12
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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include "predefine.h"
#include "main.h"
#include "externfunc.h"
#include "public.h"
#include "Language.h"
#include "MyDialog.h"
#include "FormSetting.h"
#include "FormSetTime.h"
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
#if DBG_SET_TIME > 0
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

#define W_LB_NAME	SCR_WIDTH / 10 * 2
#define H_LB_NAME	FONT_HEIGHT
#define X_LB_NAME	5
#define Y_LB_NAME	5

#define W_EDIT	50
#define H_EDIT	FONT_HEIGHT
#define X_EDIT	X_LB_NAME + W_LB_NAME
#define Y_EDIT	Y_LB_NAME + H_LB_NAME + INTERVAL

#define W_BUTTON	60
#define H_BUTTON	FONT_HEIGHT
#define X_BUTTON	SCR_WIDTH / 2

#define _Y_LOCATION(x) Y_LB_NAME + ((FONT_HEIGHT+18) * x)
#define _X_BUTTON_LOCATION(x) X_BUTTON + ((W_BUTTON+25) * x)
#endif
#if (defined TC3000_18S)
#define TEXT_FONT font18
#define BUTTON_FONT font14
#define AREA_WIDTH SCR_WIDTH - 20
#define FONT_HEIGHT 25
#define INTERVAL	6
#define BUTTON_WIDTH 50
#define BUTTON_HEIGHT FONT_HEIGHT + INTERVAL

#define W_LB_TITLE	SCR_WIDTH
#define H_LB_TITLE	FONT_HEIGHT
#define X_LB_TITLE	0
#define Y_LB_TITLE	5

#define W_LB_NAME	SCR_WIDTH / 10 * 2
#define H_LB_NAME	FONT_HEIGHT
#define X_LB_NAME	5
#define Y_LB_NAME	5

#define W_EDIT	50
#define H_EDIT	FONT_HEIGHT
#define X_EDIT	X_LB_NAME + W_LB_NAME
#define Y_EDIT	Y_LB_NAME + H_LB_NAME + INTERVAL

#define W_BUTTON	60
#define H_BUTTON	FONT_HEIGHT
#define X_BUTTON	SCR_WIDTH / 2

#define _Y_LOCATION(x) Y_LB_NAME + ((FONT_HEIGHT+18) * x)
#define _X_BUTTON_LOCATION(x) X_BUTTON + ((W_BUTTON+25) * x)
#endif
/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
static BITMAP *Bmp_background = NULL;
static HWND hCtrl;
static int auto_close_time;
static int enable_time_sycn;
static MY_CTRLDATA ChildCtrls [] = {
	STATIC_LB  (X_LB_TITLE, Y_LB_TITLE, W_LB_TITLE, H_LB_TITLE,
			IDC_SETTIME_LB_TITLE,
			Word[WORD_TIME_SET].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(2), W_LB_NAME, H_LB_NAME,
			IDC_SETTIME_LB_YEAR,
			Word[WORD_YEAR_ENC].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(3), W_LB_NAME, H_LB_NAME,
			IDC_SETTIME_LB_MONTH,
			Word[WORD_MONTH_ENC].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(4), W_LB_NAME, H_LB_NAME,
			IDC_SETTIME_LB_DAY,
			Word[WORD_DATE_ENC].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(SCR_WIDTH / 2, _Y_LOCATION(2), W_LB_NAME, H_LB_NAME,
			IDC_SETTIME_LB_HOUR,
			Word[WORD_HOUR].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(SCR_WIDTH / 2, _Y_LOCATION(3), W_LB_NAME, H_LB_NAME,
			IDC_SETTIME_LB_MINUTE,
			Word[WORD_MINUTE].string,
			0,
			&TEXT_FONT),
	STATIC_LB(X_LB_NAME, _Y_LOCATION(5), SCR_WIDTH, H_LB_NAME,
			IDC_SETTIME_LB_REMINDER,
			"",
			0,
			&BUTTON_FONT),
	STATIC_LB(X_LB_NAME, _Y_LOCATION(5) + H_LB_NAME , SCR_WIDTH, H_LB_NAME,
			IDC_SETTIME_LB_REMINDER_BT,
			"",
			0,
			&TEXT_FONT),

	BUTTON_CHECKVOX(X_LB_NAME, _Y_LOCATION(1), SCR_WIDTH, H_BUTTON,
			IDC_SETTIME_BT_SYNC,
			Word[WORD_TIME_SET_SYNC].string,
			0,
			&BUTTON_FONT),

	EDIT_L(X_EDIT, _Y_LOCATION(2), W_EDIT, H_EDIT,
			IDC_SETTIME_EDIT_YEAR,
			"",
			0,
			0),
	EDIT_L(X_EDIT, _Y_LOCATION(3), W_EDIT, H_EDIT,
			IDC_SETTIME_EDIT_MONTH,
			"",
			0,
			0),
	EDIT_L(X_EDIT, _Y_LOCATION(4), W_EDIT, H_EDIT,
			IDC_SETTIME_EDIT_DAY,
			"",
			0,
			0),
	EDIT_L(X_EDIT + SCR_WIDTH / 2, _Y_LOCATION(2), W_EDIT, H_EDIT,
			IDC_SETTIME_EDIT_HOUR,
			"",
			0,
			0),
	EDIT_L(X_EDIT + SCR_WIDTH / 2, _Y_LOCATION(3), W_EDIT, H_EDIT,
			IDC_SETTIME_EDIT_MINUTE,
			"",
			0,
			0),

	BUTTON_LB(_X_BUTTON_LOCATION(0), _Y_LOCATION(4), W_BUTTON, H_BUTTON,
			IDC_SETTIME_BT_SAVE,
			Word[WORD_SAVE].string,
			0,
			&BUTTON_FONT),
	BUTTON_LB(_X_BUTTON_LOCATION(1), _Y_LOCATION(4), W_BUTTON, H_BUTTON,
			IDC_SETTIME_BT_BACK,
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
		CTRL_TYPE_SETTIME_EDIT,编辑框
		CTRL_TYPE_SETTIME_BUTTON,按钮
 */
/* ----------------------------------------------------------------*/
static int JudgeCtrlType(int id)
{
	if (id >= IDC_SETTIME_BT_SYNC) {
		return CTRL_TYPE_SETTIME_BUTTON;
	} else {
		return	CTRL_TYPE_SETTIME_EDIT;
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
	SendMessage(hwnd, MSG_SETTIME_UPDATE_REMINDER_BT,
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
	SendMessage(hwnd, MSG_SETTIME_UPDATE_REMINDER,
			0,
			(LPARAM)string);
}

/* ----------------------------------------------------------------*/
/**
 * @brief EnableEdit 设置编辑框为可输入状态
 *
 * @param hwnd
 */
/* ----------------------------------------------------------------*/
static void EnableEdit(HWND hwnd)
{
	int i;
	for (i = IDC_SETTIME_EDIT_YEAR; i <= IDC_SETTIME_EDIT_MINUTE; ++i) {
		SendMessage(GetDlgItem(GetParent(hwnd),i),EM_SETREADONLY ,FALSE,0);
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief DisableEdit 设置编辑框为只读状态
 *
 * @param hwnd
 */
/* ----------------------------------------------------------------*/
static void DisableEdit(HWND hwnd)
{
	int i;
	for (i = IDC_SETTIME_EDIT_YEAR; i <= IDC_SETTIME_EDIT_MINUTE; ++i) {
		SendMessage(GetDlgItem(GetParent(hwnd),i),EM_SETREADONLY ,TRUE,0);
	}
	
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
	char buf[32];
	int e_year,e_month,e_day,e_hour,e_min;
	Public.SyncTime = enable_time_sycn;
	sprintf(buf, "%d",Public.SyncTime);                          
	SetValueToEtcFile_Ext("interface.ini","MODE","SyncTime",buf);
	if (Public.SyncTime == FALSE) {
		SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETTIME_EDIT_YEAR), 
				MSG_GETTEXT, 10,  (LPARAM)buf);
		e_year = atoi(buf);
		SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETTIME_EDIT_MONTH), 
				MSG_GETTEXT, 10,  (LPARAM)buf);
		e_month = atoi(buf);
		SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETTIME_EDIT_DAY), 
				MSG_GETTEXT, 10,  (LPARAM)buf);
		e_day = atoi(buf);
		SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETTIME_EDIT_HOUR), 
				MSG_GETTEXT, 10,  (LPARAM)buf);
		e_hour = atoi(buf);
		SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETTIME_EDIT_MINUTE), 
				MSG_GETTEXT, 10,  (LPARAM)buf);
		e_min = atoi(buf);

		adjustdate(e_year,e_month,e_day,e_hour,e_min,0);
	} else {
		Public.reSyncTime();
	}
	sprintf(buf,"%s%s",Word[WORD_SAVE].string,Word[WORD_SUCCESS].string);
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
 * @brief btEnableSyncTime 使能同步管理中心时间按钮回调函数
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void btEnableSyncTime(HWND hwnd, int id, int nc, DWORD add_data)
{
	enable_time_sycn ^= 1;
	if (enable_time_sycn) {
		DisableEdit(hwnd);
		UpdateReminder(GetParent(hwnd),Word[WORD_HELP_SYNC_TIME].string);
	} else {
		EnableEdit(hwnd);
		UpdateReminder(GetParent(hwnd),"");
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief JudgeDay 根据年份和月份判断日期是否符合标准
 *
 * @param hwnd
 */
/* ----------------------------------------------------------------*/
static void JudgeDay(HWND hwnd)
{
	char buf[10];
	int year,month,day;
	SendMessage (GetDlgItem(hwnd,IDC_SETTIME_EDIT_YEAR), MSG_GETTEXT, 10,  (LPARAM)buf);
	year = atoi(buf);
	SendMessage (GetDlgItem(hwnd,IDC_SETTIME_EDIT_MONTH), MSG_GETTEXT, 10,  (LPARAM)buf);
	month = atoi(buf);
	int max_day = JudgeMonth(year,month);

	SendMessage (GetDlgItem(hwnd,IDC_SETTIME_EDIT_DAY), MSG_GETTEXT, 10,  (LPARAM)buf);
	day = atoi(buf);
	if (max_day) {
		if (day > max_day) {
			sprintf(buf,"%d",max_day);
			SetWindowText(GetDlgItem(hwnd,IDC_SETTIME_EDIT_DAY),buf);
		}
	}
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
	int time;
	if ((nc == EN_UPDATE) || (nc == EN_SETFOCUS)){
		int len = SendMessage (GetDlgItem(GetParent(hwnd),id), MSG_GETTEXT, 50,  (LPARAM)text);
		SendMessage (GetDlgItem(GetParent(hwnd),id), EM_SETCARETPOS, 0, len);
	} else if (nc == EN_CHANGE) {
		SendMessage (GetDlgItem(GetParent(hwnd),id), MSG_GETTEXT, 50,  (LPARAM)text);
		time = atoi(text);		
		switch (id) 
		{
			case IDC_SETTIME_EDIT_YEAR:
				{
					if (time > 2080)
						time = 2080;
					sprintf(text,"%d",time);
					SetWindowText(GetDlgItem(GetParent(hwnd),id),text);
					JudgeDay(GetParent(hwnd));
				} break;
		
			case IDC_SETTIME_EDIT_MONTH:
				{
					if (time > 12)
						time = 12;
					sprintf(text,"%d",time);
					SetWindowText(GetDlgItem(GetParent(hwnd),id),text);
					JudgeDay(GetParent(hwnd));
				} break;
		
			case IDC_SETTIME_EDIT_DAY:
				{
					JudgeDay(GetParent(hwnd));
				} break;
		
			case IDC_SETTIME_EDIT_HOUR:
				{
					if (time > 23)
						time = 23;
					sprintf(text,"%d",time);
					SetWindowText(GetDlgItem(GetParent(hwnd),id),text);
				} break;
		
			case IDC_SETTIME_EDIT_MINUTE:
				{
					if (time > 59)
						time = 59;
					sprintf(text,"%d",time);
					SetWindowText(GetDlgItem(GetParent(hwnd),id),text);
					
				} break;
		
			default:
				break;
		}
	} else if (nc == EN_KILLFOCUS) {
		SendMessage (GetDlgItem(GetParent(hwnd),id), MSG_GETTEXT, 50,  (LPARAM)text);
		time = atoi(text);		
		switch (id) 
		{
			case IDC_SETTIME_EDIT_YEAR:
				{
					if (time < 2000)
						time = 1999;
					sprintf(text,"%d",time);
					SetWindowText(GetDlgItem(GetParent(hwnd),id),text);
				} break;
		
			case IDC_SETTIME_EDIT_MONTH:
				{
					if (time < 1)
						time = 1;
					sprintf(text,"%d",time);
					SetWindowText(GetDlgItem(GetParent(hwnd),id),text);
				} break;
		
			case IDC_SETTIME_EDIT_DAY:
				{
					if (time < 1)
						time = 1;
					sprintf(text,"%d",time);
					SetWindowText(GetDlgItem(GetParent(hwnd),id),text);
				} break;
		
			default:
				break;
		}
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

				switch (JudgeCtrlType(GetDlgCtrlID(hCtrl)))
				{
					case CTRL_TYPE_SETTIME_EDIT:
						{
							if (ctrl_type != CTRL_TYPE_SETTIME_EDIT) {
								UpdateBtReminder(hDlg,Word[WORD_HELP_EDIT].string);
							}
							ctrl_type = CTRL_TYPE_SETTIME_EDIT;
						} break;

					case CTRL_TYPE_SETTIME_BUTTON:
						{
							if (ctrl_type != CTRL_TYPE_SETTIME_BUTTON) {
								UpdateBtReminder(hDlg,Word[WORD_HELP_BUTTON].string);
							}
							ctrl_type = CTRL_TYPE_SETTIME_BUTTON;
						} break;

					default:
						break;
				}
			} break;

		case KEYOK:
			{
				switch (JudgeCtrlType(GetDlgCtrlID(hCtrl)))
				{
					case CTRL_TYPE_SETTIME_EDIT:
						{
							SendMessage(hCtrl,MSG_CHAR,'\b',0);
						} break;

					case CTRL_TYPE_SETTIME_BUTTON:
						{
							SendMessage(hCtrl,BM_CLICK,0,0);
						} break;

					default:
						break;
				}
			} break;

		default:
			{
				switch (JudgeCtrlType(GetDlgCtrlID(hCtrl)))
				{
					case CTRL_TYPE_SETTIME_EDIT:
						{
							if (KeyVal != KEYLEAVEWORD) {
								SendMessage(hCtrl,MSG_CHAR,'0' + KeyVal,0);
							}
						} break;

					default:
						break;
				}
			} break;
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief InitSetTime 初始化时间设置编辑框
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 */
/* ----------------------------------------------------------------*/
static void InitSetTime(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	char buf[64];
	time_t t;
	struct tm *tm;
	time(&t);
	tm = localtime(&t);
	sprintf(buf,"%d",tm->tm_year+1900);
	SetWindowText(GetDlgItem(hDlg,IDC_SETTIME_EDIT_YEAR),buf);
	sprintf(buf,"%d",tm->tm_mon+1);
	SetWindowText(GetDlgItem(hDlg,IDC_SETTIME_EDIT_MONTH),buf);
	sprintf(buf,"%d",tm->tm_mday);
	SetWindowText(GetDlgItem(hDlg,IDC_SETTIME_EDIT_DAY),buf);
	sprintf(buf,"%d",tm->tm_hour);
	SetWindowText(GetDlgItem(hDlg,IDC_SETTIME_EDIT_HOUR),buf);
	sprintf(buf,"%d",tm->tm_min);
	SetWindowText(GetDlgItem(hDlg,IDC_SETTIME_EDIT_MINUTE),buf);

	if (enable_time_sycn != Public.SyncTime) {
		SendDlgItemMessage(hDlg,IDC_SETTIME_BT_SYNC,BM_CLICK,0,0);
		UpdateReminder(hDlg,Word[WORD_HELP_SYNC_TIME].string);
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief SetTimeProc 窗口回调函数
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 *
 * @return
 */
/* ----------------------------------------------------------------*/
static int SetTimeProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_INITDIALOG:
			{
				Screen.Add(hDlg,"TFrmTime");
				hCtrl = 0;
				enable_time_sycn = 0;
				auto_close_time = FORM_SETTING_ONTIME;
				int i;
				for (i=IDC_SETTIME_LB_TITLE;i<=IDC_SETTIME_LB_MINUTE; i++){
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW,MY_WHIGHT);
				}
				for (i=IDC_SETTIME_EDIT_YEAR;i<=IDC_SETTIME_EDIT_MINUTE; i++){
					if (i == IDC_SETTIME_EDIT_YEAR)
						SendDlgItemMessage(hDlg,i, EM_LIMITTEXT, 4, 0L);
					else
						SendDlgItemMessage(hDlg,i, EM_LIMITTEXT, 2, 0L);
					SetNotificationCallback (GetDlgItem(hDlg,i), edit_notify);
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW,MY_BLACK);
				}
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_SETTIME_LB_REMINDER),
						WE_FGC_WINDOW,MY_RED);
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_SETTIME_LB_REMINDER_BT),
						WE_FGC_WINDOW,MY_RED);
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_SETTIME_BT_SYNC),
						WE_FGC_THREED_BODY,MY_WHIGHT);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_SETTIME_BT_SAVE), btSave);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_SETTIME_BT_BACK), btBack);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_SETTIME_BT_SYNC), btEnableSyncTime);

				InitSetTime(hDlg, message, wParam, lParam);
				UpdateBtReminder(hDlg,Word[WORD_HELP_BUTTON].string);
				SetTimer(hDlg,IDC_SETTIME_TIMER,TIME_1S);
			} break;

		case MSG_KEYBOARD :
			ProUserKeyBoard(hDlg,wParam);
			break;

		case MSG_SETTIME_UPDATE_REMINDER :
			SetWindowText(GetDlgItem(hDlg,IDC_SETTIME_LB_REMINDER),(char *)lParam);
			break;

		case MSG_SETTIME_UPDATE_REMINDER_BT :
			SetWindowText(GetDlgItem(hDlg,IDC_SETTIME_LB_REMINDER_BT),(char *)lParam);
			break;

		case MSG_ERASEBKGND:
			if (Bmp_background != NULL)
				DrawBackground(hDlg, (HDC)wParam, (const RECT*)lParam,Bmp_background);
			return 0;

		case MSG_TIMER:
			{
				if (wParam == IDC_SETTIME_TIMER){
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
				KillTimer (hDlg,IDC_SETTIME_TIMER);
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
 * @brief CreateSetTimeForm 创建设置时间窗口
 *
 * @param hMainWnd
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
int CreateSetTimeForm(HWND hMainWnd,BITMAP *Bmp_bkg)
{
	HWND Form = Screen.Find("TFrmTime");
	Bmp_background = Bmp_bkg;
	if(Form) {
		ShowWindow(Form,SW_SHOWNORMAL);
		return Form;
	} else {
		return CreateMyWindowIndirectParam(&DlgInitParam,hMainWnd,SetTimeProc,0);
	}
}

