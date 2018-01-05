/*
 * =====================================================================================
 *
 *       Filename:  FormNetSetting.c
 *
 *    Description:  网络参数设置
 *
 *        Version:  1.0
 *        Created:  2015-11-05 13:08:21
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
#if DBG_NETSETTING > 0
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

#define W_LB_NAME	140
#define H_LB_NAME	FONT_HEIGHT
#define X_LB_NAME	10
#define Y_LB_NAME	5 + H_LB_TITLE

#define W_BUTTON	80
#define H_BUTTON	FONT_HEIGHT + 2
#define X_BUTTON	AREA_WIDTH - W_BUTTON + 5

#define W_EDIT	AREA_WIDTH - W_LB_NAME - X_LB_NAME
#define H_EDIT	FONT_HEIGHT
#define X_EDIT	X_LB_NAME + W_LB_NAME + INTERVAL
#define Y_EDIT	Y_LB_NAME

#define X_LB_ADD 	X_LB_NAME + 105 + INTERVAL

#define _Y_LOCATION(x) Y_LB_NAME + ((FONT_HEIGHT+5) * x)
#endif

#if (defined TC3000_18S)
#define TEXT_FONT font18
#define BUTTON_FONT font14
#define AREA_WIDTH SCR_WIDTH - 20
#define FONT_HEIGHT 24
#define INTERVAL	5
#define BUTTON_WIDTH 50
#define BUTTON_HEIGHT FONT_HEIGHT + INTERVAL

#define W_LB_TITLE	SCR_WIDTH
#define H_LB_TITLE	FONT_HEIGHT
#define X_LB_TITLE	0
#define Y_LB_TITLE	5


#define W_LB_NAME	240
#define H_LB_NAME	FONT_HEIGHT
#define X_LB_NAME	10
#define Y_LB_NAME	5 + H_LB_TITLE

#define W_BUTTON	80
#define H_BUTTON	FONT_HEIGHT + 2
#define X_BUTTON	AREA_WIDTH - W_BUTTON + 5

#define W_EDIT	AREA_WIDTH - W_LB_NAME - X_LB_NAME
#define H_EDIT	FONT_HEIGHT
#define X_EDIT	X_LB_NAME + W_LB_NAME + INTERVAL
#define Y_EDIT	Y_LB_NAME

#define X_LB_ADD 	X_EDIT
#define _Y_LOCATION(x) Y_LB_NAME + ((FONT_HEIGHT+5) * x)
#endif
/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
static BITMAP *Bmp_background = NULL;
static HWND hCtrl;
static int auto_close_time;

static MY_CTRLDATA ChildCtrls [] = {
	STATIC_LB  (X_LB_TITLE, Y_LB_TITLE, W_LB_TITLE, H_LB_TITLE,
			IDC_NETSET_LB_TITLE,
			Word[WORD_NET_SETTING].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(0), W_LB_NAME, H_LB_NAME,
			IDC_NETSET_LB_IP,
			Word[WORD_IP_ADD].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(1), W_LB_NAME, H_LB_NAME,
			IDC_NETSET_LB_MASK,
			Word[WORD_NET_MASK].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(2), W_LB_NAME, H_LB_NAME,
			IDC_NETSET_LB_GATEWAY,
			Word[WORD_GATE_WAY].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(3), W_LB_NAME, H_LB_NAME,
			IDC_NETSET_LB_CENTER_IP,
			Word[WORD_CENTER_IP].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(4), W_LB_NAME, H_LB_NAME,
			IDC_NETSET_LB_IMEI,
			Word[WORD_DEVICE_NUM].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(5), W_LB_NAME, H_LB_NAME,
			IDC_NETSET_LB_MAC,
			Word[WORD_MAC_ADD].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_ADD, _Y_LOCATION(5), W_LB_NAME, H_LB_NAME,
			IDC_NETSET_LB_MAC_ADD,
			"",
			0,
			0),
	STATIC_LB(X_LB_NAME, _Y_LOCATION(6), SCR_WIDTH - W_BUTTON, H_LB_NAME,
			IDC_NETSET_LB_REMINDER,
			"",
			0,
			&BUTTON_FONT),
	STATIC_LB(X_LB_NAME, _Y_LOCATION(7), SCR_WIDTH - W_BUTTON, H_LB_NAME,
			IDC_NETSET_LB_REMINDER_BT,
			"",
			0,
			&TEXT_FONT),
	GROUP_EDIT_L(X_EDIT, _Y_LOCATION(0), W_EDIT, H_EDIT,
			IDC_NETSET_EDIT_IP,
			"",
			0,
			0),
	EDIT_L(X_EDIT, _Y_LOCATION(1), W_EDIT, H_EDIT,
			IDC_NETSET_EDIT_MASK,
			"",
			0,
			0),
	EDIT_L(X_EDIT, _Y_LOCATION(2), W_EDIT, H_EDIT,
			IDC_NETSET_EDIT_GATEWAY,
			"",
			0,
			0),
	EDIT_L(X_EDIT, _Y_LOCATION(3), W_EDIT, H_EDIT,
			IDC_NETSET_EDIT_CENTER_IP,
			"",
			0,
			0),
	EDIT_L(X_EDIT, _Y_LOCATION(4), W_EDIT, H_EDIT,
			IDC_NETSET_EDIT_IMEI,
			"",
			0,
			0),

	BUTTON_LB(X_BUTTON, _Y_LOCATION(5), W_BUTTON, H_BUTTON,
			IDC_NETSET_BT_GET_IMEI,
			Word[WORD_GET_ID].string,
			0,
			&BUTTON_FONT),
	BUTTON_LB(X_BUTTON, _Y_LOCATION(6), W_BUTTON, H_BUTTON,
			IDC_NETSET_BT_SAVE,
			Word[WORD_SAVE].string,
			0,
			&BUTTON_FONT),
	BUTTON_LB(X_BUTTON, _Y_LOCATION(7), W_BUTTON, H_BUTTON,
			IDC_NETSET_BT_BACK,
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
 * @brief IsButton 判断是否是按钮控�? *
 * @param id 控件ID�? *
 * @returns
 */
/* ----------------------------------------------------------------*/
static BOOL IsButton(int id)
{
	if ((id >= IDC_NETSET_BT_GET_IMEI) && (id <= IDC_NETSET_BT_BACK)) {
		return TRUE;
	} else {
		return FALSE;
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
	SendMessage(hwnd, MSG_NETSET_UPDATE_REMINDER_BT,
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
	SendMessage(hwnd, MSG_NETSET_UPDATE_REMINDER,
			0,
			(LPARAM)string);
}
/* ----------------------------------------------------------------*/
/**
 * @brief btGetID 获取设备ID按钮
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void btGetID(HWND hwnd, int id, int nc, DWORD add_data)
{
	Public.GetDeviceIMEI();
	UpdateReminder(GetParent(hwnd),Word[WORD_GET_DEVICE_ID_ING].string);
	SetTimer(GetParent(hwnd),IDC_NETSET_TIMER_GETID,TIME_1S * 5);
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
	char ip[16]={0},mask[16]={0},gateway[16]={0},center_ip[16]={0},imei[20];
	char ret_text[32];
	struct in_addr addr;
	// 判断IP格式
	SendMessage (GetDlgItem(GetParent(hwnd),IDC_NETSET_EDIT_IP), MSG_GETTEXT, 16, (LPARAM) ip);
	if (inet_pton(AF_INET, ip, &addr) <= 0) {
		sprintf(ret_text,"IP%s",Word[WORD_WRONG_FORMATE].string);
		UpdateReminder(GetParent(hwnd),ret_text);
		return;
	}

	// 判断子网掩码格式
	SendMessage (GetDlgItem(GetParent(hwnd),IDC_NETSET_EDIT_MASK), MSG_GETTEXT, 16, (LPARAM) mask);
	if (inet_pton(AF_INET, mask, &addr) <= 0)	{
		sprintf(ret_text,"%s%s",Word[WORD_MAC_ADD].string,Word[WORD_WRONG_FORMATE].string);
		UpdateReminder(GetParent(hwnd),ret_text);
		return;
	}

	// 判断网关格式
	SendMessage (GetDlgItem(GetParent(hwnd),IDC_NETSET_EDIT_GATEWAY), MSG_GETTEXT, 16, (LPARAM) gateway);
	if (inet_pton(AF_INET, gateway, &addr) <= 0) {
		sprintf(ret_text,"%s%s",Word[WORD_GATE_WAY].string,Word[WORD_WRONG_FORMATE].string);
		UpdateReminder(GetParent(hwnd),ret_text);
		return;
	}

	// 判断管理中心IP格式
	SendMessage (GetDlgItem(GetParent(hwnd),IDC_NETSET_EDIT_CENTER_IP), MSG_GETTEXT, 16, (LPARAM) center_ip);
	if (inet_pton(AF_INET,center_ip,&addr) <= 0) {
		sprintf(ret_text,"%sIP%s",Word[WORD_CENTER_IP].string,Word[WORD_WRONG_FORMATE].string);
		UpdateReminder(GetParent(hwnd),ret_text);
		return;
	}
	SendMessage (GetDlgItem(GetParent(hwnd),IDC_NETSET_EDIT_IMEI), MSG_GETTEXT, 20, (LPARAM) imei);
	memcpy(Public.cLocalIP,ip,16);
	memcpy(Public.cNetMask,mask,16);
	memcpy(Public.cGateWay,gateway,16);

	Public.UpdateCenterIP("UserInfo",center_ip,sizeof(center_ip) - 1);
	Public.dwDevID = (unsigned long long)strtoull(imei,NULL,16);
	Public.SaveIMEIandSetMac();
	sprintf(ret_text,"%s%s",Word[WORD_SAVE].string,Word[WORD_SUCCESS].string);
	UpdateReminder(GetParent(hwnd),ret_text);
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
	// HWND Form = Screen.Find("TFrmPassword");
	// if (Form) {
		// SendNotifyMessage(Form,MSG_CLOSE,0,0);
	// }

}

/* ----------------------------------------------------------------*/
/**
 * @brief ProUserKeyBoard 处理按键键�? *
 * @param hWnd  主窗口句�? * @param KeyVal 键�? */
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
				if (IsButton(id)) {
					if (ctrl_type == 0) {
						UpdateBtReminder(hDlg,Word[WORD_HELP_BUTTON].string);
					}
					ctrl_type = 1;
				} else {
					if (ctrl_type == 1) {
						UpdateBtReminder(hDlg,Word[WORD_HELP_EDIT_IP].string);
					}
					ctrl_type = 0;
				}
			} break;

		case KEYOK:
			{
				int id = GetDlgCtrlID(hCtrl);
				if (IsButton(id)) {
					SendMessage(hDlg,MSG_KEYDOWN,SCANCODE_ENTER,0);
					SendMessage(hDlg,MSG_KEYUP,SCANCODE_ENTER,0);
				} else {
					// 退格键
					SendMessage(hCtrl,MSG_CHAR,'\b',0);
				}
			} break;

		default:
			{
				int id = GetDlgCtrlID(hCtrl);
				if (!IsButton(id)) {
					if (KeyVal == KEYLEAVEWORD) {
						SendMessage(hCtrl,MSG_CHAR,'.',0);
					} else {
						SendMessage(hCtrl,MSG_CHAR,'0' + KeyVal,0);
					}
				}
			} break;
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief edit_notify 编辑框回调函数，用于获得焦点时设置光标位�? *
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
		int ip_len = SendMessage (GetDlgItem(GetParent(hwnd),id), MSG_GETTEXT, 50,  (LPARAM)text);
		SendMessage (GetDlgItem(GetParent(hwnd),id), EM_SETCARETPOS, 0, ip_len);
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief InitEdit 初始化编辑框
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 */
/* ----------------------------------------------------------------*/
static void InitEdit(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	char buf[128];
	SetWindowText(GetDlgItem(hDlg,IDC_NETSET_EDIT_IP),Public.cLocalIP);
	SetWindowText(GetDlgItem(hDlg,IDC_NETSET_EDIT_MASK),Public.cNetMask);
	SetWindowText(GetDlgItem(hDlg,IDC_NETSET_EDIT_GATEWAY),Public.cGateWay);
	SetWindowText(GetDlgItem(hDlg,IDC_NETSET_EDIT_CENTER_IP),Public.pCenterMsg->IP);
	sprintf(buf,"%llX",Public.dwDevID);
	SetWindowText(GetDlgItem(hDlg,IDC_NETSET_EDIT_IMEI),buf);
	SetWindowText(GetDlgItem(hDlg,IDC_NETSET_LB_MAC_ADD),Public.MAC);
}

/* ----------------------------------------------------------------*/
/**
 * @brief NetSettingProc 窗口回调函数
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 *
 * @return
 */
/* ----------------------------------------------------------------*/
static int NetSettingProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_INITDIALOG:
			{
				Screen.Add(hDlg,"TFrmNet");
				hCtrl = 0;
				auto_close_time = FORM_SETTING_ONTIME;
				int i;
				for (i=IDC_NETSET_LB_TITLE;i<=IDC_NETSET_LB_MAC_ADD; i++){
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW,MY_WHIGHT); //黑色
				}
				for (i=IDC_NETSET_EDIT_IP;i<=IDC_NETSET_EDIT_IMEI; i++){
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW,MY_BLACK); //黑色
					SendDlgItemMessage(hDlg,i, EM_LIMITTEXT, 20, 0L);
					SetNotificationCallback (GetDlgItem(hDlg,i), edit_notify);
				}

				SetWindowElementAttr(GetDlgItem(hDlg,IDC_NETSET_LB_REMINDER), WE_FGC_WINDOW,COLOR_green);
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_NETSET_LB_REMINDER_BT), WE_FGC_WINDOW,COLOR_green);

				SetNotificationCallback (GetDlgItem(hDlg,IDC_NETSET_BT_GET_IMEI), btGetID);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_NETSET_BT_SAVE), btSave);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_NETSET_BT_BACK), btBack);
				InitEdit(hDlg,message,wParam,lParam);
				UpdateBtReminder(hDlg,Word[WORD_HELP_BUTTON].string);
				SetTimer(hDlg,IDC_NETSET_TIMER,TIME_1S);
			} break;

		case MSG_KEYBOARD :
			ProUserKeyBoard(hDlg,wParam);
			break;

		case MSG_NETSET_UPDATE_REMINDER :
			SetWindowText(GetDlgItem(hDlg,IDC_NETSET_LB_REMINDER),(char *)lParam);
			break;

		case MSG_NETSET_UPDATE_REMINDER_BT :
			SetWindowText(GetDlgItem(hDlg,IDC_NETSET_LB_REMINDER_BT),(char *)lParam);
			break;

		case MSG_TIMER:
			{
				if (wParam == IDC_NETSET_TIMER){
					if (auto_close_time > 0) {
						DBG_P("[%s]time:%d\n", __FILE__,auto_close_time);
						if (--auto_close_time == 0) {
							SendNotifyMessage(hDlg,MSG_CLOSE,0,0);
						}
					}
				} else if (wParam == IDC_NETSET_TIMER_GETID) {
					char buf[128];
					KillTimer(hDlg,IDC_NETSET_TIMER_GETID);
					if(Public.dwDevIDTemp != Public.dwDevID) {
						UpdateReminder(hDlg,Word[WORD_GET_DEVICE_ID_SUCCESS].string);
						sprintf(buf,"%llx",Public.dwDevIDTemp);
						SetWindowText(GetDlgItem(hDlg,IDC_NETSET_EDIT_IMEI),buf);
					} else {
						UpdateReminder(hDlg,Word[WORD_GET_DEVICE_ID_FAIL].string);
					}

				}
			} break;

		case MSG_ERASEBKGND:
			if (Bmp_background != NULL)
				DrawBackground(hDlg, (HDC)wParam, (const RECT*)lParam,Bmp_background);
			return 0;

		case MSG_CLOSE:
			{
				HWND Form = Screen.Find("TFrmSetting");
				SendNotifyMessage(Form,MSG_SETTING_RESET_TIME,0,0);
				KillTimer (hDlg,IDC_NETSET_TIMER);
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
 * @brief CreateNetSettingForm 创建网络参数设置窗口
 *
 * @param hMainWnd
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
int CreateNetSettingForm(HWND hMainWnd,BITMAP *Bmp_bkg)
{
	HWND Form = Screen.Find("TFrmNet");
	Bmp_background = Bmp_bkg;
	if(Form) {
		ShowWindow(Form,SW_SHOWNORMAL);
		return Form;
	} else {
		return CreateMyWindowIndirectParam(&DlgInitParam,hMainWnd,NetSettingProc,0);
	}
}

