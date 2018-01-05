/*
 * =====================================================================================
 *
 *       Filename:  FormSetDevType.c
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
			IDC_DEVTYPE_LB_TITLE,
			Word[WORD_SETTING_DEVTYPE].string,
			0,
			&TEXT_FONT),

	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(0), W_LB_NAME, H_LB_NAME,
			IDC_DEVTYPE_LB_BUILD,
			Word[WORD_SELECT_DH].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(1), W_LB_NAME, H_LB_NAME,
			IDC_DEVTYPE_LB_UNIT,
			Word[WORD_SELECT_DYH].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(2), W_LB_NAME, H_LB_NAME,
			IDC_DEVTYPE_LB_IP,
			Word[WORD_IP_ADD].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(3), W_LB_NAME, H_LB_NAME,
			IDC_DEVTYPE_LB_MASK,
			Word[WORD_NET_MASK].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(4), W_LB_NAME, H_LB_NAME,
			IDC_DEVTYPE_LB_GATEWAY,
			Word[WORD_GATE_WAY].string,
			0,
			&TEXT_FONT),

	GROUP_COMBOBOX(X_EDIT, _Y_LOCATION(0), W_EDIT, H_LB_NAME,
				IDC_DEVTYPE_LB_BUILD_VALUE,
				"",
				0,
				0),
	GROUP_COMBOBOX(X_EDIT, _Y_LOCATION(1), W_EDIT, H_LB_NAME,
			IDC_DEVTYPE_LB_UNIT_VALUE,
			"",
			0,
			0),
	STATIC_LB_L(X_EDIT, _Y_LOCATION(2), W_EDIT, H_EDIT,
			IDC_DEVTYPE_LB_IP_VALUE,
			"",
			0,
			0),
	STATIC_LB_L(X_EDIT, _Y_LOCATION(3), W_EDIT, H_EDIT,
			IDC_DEVTYPE_LB_MASK_VALUE,
			"",
			0,
			0),
	STATIC_LB_L(X_EDIT, _Y_LOCATION(4), W_EDIT, H_EDIT,
			IDC_DEVTYPE_LB_GATEWAY_VALUE,
			"",
			0,
			0),

	STATIC_LB(X_LB_NAME, _Y_LOCATION(6), SCR_WIDTH - W_BUTTON, H_LB_NAME,
		IDC_DEVTYPE_LB_NOTY,
		"",
		0,
		&BUTTON_FONT),
	
	BUTTON_LB(X_BUTTON, _Y_LOCATION(6), W_BUTTON, H_BUTTON,
			IDC_DEVTYPE_BT_SAVE,
			Word[WORD_SAVE].string,
			0,
			&BUTTON_FONT),
	BUTTON_LB(X_BUTTON, _Y_LOCATION(7), W_BUTTON, H_BUTTON,
			IDC_DEVTYPE_BT_BACK,
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
		CTRL_TYPE_SYSPARA_EDIT,   			编辑框
		CTRL_TYPE_SYSPARA_BUTTON, 			按钮
		CTRL_TYPE_SYSPARA_MENU    			组合框
 */
/* ----------------------------------------------------------------*/
static int JudgeCtrlType(int id)
{
	if (id == IDC_DEVTYPE_LB_BUILD_VALUE || id == IDC_DEVTYPE_LB_UNIT_VALUE) {
		return CTRL_TYPE_DEVTYPE_MENU;
	} else {
		return CTRL_TYPE_DEVTYPE_BUTTON;
	}
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
	SendMessage(hwnd, MSG_DEVTYPE_UPDATE_REMINDER,
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
	int BuildNum = 0;
	int UnitNum = 0;
	char ret_text[32];
	char buf[64];
	
	//保存栋号
	BuildNum = SendMessage (GetDlgItem(GetParent(hwnd),IDC_DEVTYPE_LB_BUILD_VALUE), CB_GETCURSEL, 0, 0);
	if(BuildNum < 0){
		sprintf(ret_text,"%s%s",Word[WORD_SAVE].string,Word[WORD_FAILED].string);
		UpdateReminder(GetParent(hwnd),ret_text);
		return;
	}
	
	Public.Build_Num = BuildNum+1;
	
	sprintf(buf,"%d",Public.Build_Num);
	SetValueToEtcFile_Ext("interface.ini","PUBLIC","Build_Num",buf);

	//保存单元号
	UnitNum = SendMessage (GetDlgItem(GetParent(hwnd),IDC_DEVTYPE_LB_UNIT_VALUE), CB_GETCURSEL, 0, 0);
	if(UnitNum  < 0){
		sprintf(ret_text,"%s%s",Word[WORD_SAVE].string,Word[WORD_FAILED].string);
		UpdateReminder(GetParent(hwnd),ret_text);
		return;
	}
	Public.Unit_Num = UnitNum;
	sprintf(buf,"%d",Public.Unit_Num);
	SetValueToEtcFile_Ext("interface.ini","PUBLIC","Unit_Num",buf);
	
	//计算IP
	int ip1=100;
	char RetIP[16];
	int i,j;
	for(i=1;i<=ROOM_BUILD_MAX;i++){
		for(j=0;j<=ROOM_UNIT_MAX;j++){
			if(i == Public.Build_Num &&  j == Public.Unit_Num ){
				sprintf(RetIP,"172.16.254.%d",ip1);
				goto cal_end;
			}
			ip1++;
		}
	}
	cal_end :
	
	strncpy(Public.cLocalIP,RetIP,sizeof(Public.cLocalIP)-1);
	SetNetwork(1,Public.cLocalIP,Public.cNetMask,Public.cGateWay,Public.MAC);
	
	SetWindowText(GetDlgItem(GetParent(hwnd),IDC_DEVTYPE_LB_IP_VALUE),Public.cLocalIP);
	
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
					case CTRL_TYPE_DEVTYPE_BUTTON:
						{
							UpdateReminder(hDlg,Word[WORD_HELP_BUTTON].string);
							ctrl_type = CTRL_TYPE_DEVTYPE_BUTTON;
						} break;

					case CTRL_TYPE_DEVTYPE_MENU:
						{
							if (ctrl_type != CTRL_TYPE_DEVTYPE_MENU) {
								UpdateReminder(hDlg,Word[WORD_HELP_MENU].string);
							}
							ctrl_type = CTRL_TYPE_DEVTYPE_MENU;
						} break;

					default:
						break;
				}
			} break;
		case KEYOK:
			{
				int id = GetDlgCtrlID(hCtrl);
				if(JudgeCtrlType(id) == CTRL_TYPE_DEVTYPE_MENU){
					SendMessage(hDlg,MSG_KEYDOWN,SCANCODE_ENTER,0);
				} else {
					SendMessage(hCtrl,BM_CLICK,0,0);
				}
			} break;
		default:
			{
				int id = GetDlgCtrlID(hCtrl);
				if(JudgeCtrlType(id) == CTRL_TYPE_DEVTYPE_MENU) {
					if (KeyVal == 0) {
						SendMessage(hDlg,MSG_KEYDOWN,SCANCODE_CURSORBLOCKDOWN,0);
					} else if (KeyVal == 8) {
						SendMessage(hDlg,MSG_KEYDOWN,SCANCODE_CURSORBLOCKUP,0);
					}
				}
			} break; 	
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief InitDevInfo 初始化信息
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 */
/* ----------------------------------------------------------------*/
static void InitDevInfo(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{	
	SetWindowText(GetDlgItem(hDlg,IDC_DEVTYPE_LB_IP_VALUE),Public.cLocalIP);
	SetWindowText(GetDlgItem(hDlg,IDC_DEVTYPE_LB_MASK_VALUE),Public.cNetMask);
	SetWindowText(GetDlgItem(hDlg,IDC_DEVTYPE_LB_GATEWAY_VALUE),Public.cGateWay);

	int i;
	for(i=1;i<=20;i++){
		char buf[128];
		sprintf(buf,"%d%s",i,Word[WORD_BUILD].string);
		SendDlgItemMessage(hDlg, IDC_DEVTYPE_LB_BUILD_VALUE, CB_ADDSTRING, 0, (LPARAM)buf);	
	}
	SendDlgItemMessage(hDlg, IDC_DEVTYPE_LB_BUILD_VALUE, CB_SETCURSEL,Public.Build_Num-1, 0);

	int k;
	for(k=0;k<=4;k++){
		char buf[128];
		if(k == 0){
			sprintf(buf,"%s",Word[WORD_NO_UNIT].string);
		}else{
			sprintf(buf,"%d%s",k,Word[WORD_UNIT].string);			
		}
		SendDlgItemMessage(hDlg, IDC_DEVTYPE_LB_UNIT_VALUE, CB_ADDSTRING, 0, (LPARAM)buf);	
	}
	SendDlgItemMessage(hDlg, IDC_DEVTYPE_LB_UNIT_VALUE, CB_SETCURSEL,Public.Unit_Num, 0);
}

/* ----------------------------------------------------------------*/
/**
 * @brief SetDevTypeProc 窗口回调函数
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 *
 * @return
 */
/* ----------------------------------------------------------------*/
static int SetDevTypeProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_INITDIALOG:
			{
				Screen.Add(hDlg,"TFrmDevType");
				hCtrl = 0;
				auto_close_time = FORM_SETTING_ONTIME;
				int i;
				for (i=IDC_DEVTYPE_LB_TITLE;i<=IDC_DEVTYPE_LB_GATEWAY_VALUE; i++){
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW,MY_WHIGHT); //黑色
				}

				SetWindowElementAttr(GetDlgItem(hDlg,IDC_DEVTYPE_LB_NOTY),WE_FGC_WINDOW,MY_RED);
				SetWindowText(GetDlgItem(hDlg,IDC_DEVTYPE_LB_NOTY),Word[WORD_HELP_BUTTON].string);
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_DEVTYPE_LB_BUILD_VALUE), WE_FGC_WINDOW,MY_BLACK);
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_DEVTYPE_LB_UNIT_VALUE), WE_FGC_WINDOW,MY_BLACK);
				
				SetNotificationCallback (GetDlgItem(hDlg,IDC_DEVTYPE_BT_SAVE), btSave);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_DEVTYPE_BT_BACK), btBack);
				

				SetWindowElementAttr(GetDlgItem(hDlg,IDC_DEVTYPE_LB_BUILD_VALUE), WE_FGC_WINDOW,MY_BLACK_TOP); //黑色
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_DEVTYPE_LB_UNIT_VALUE), WE_FGC_WINDOW,MY_BLACK_TOP); //黑色
				
				InitDevInfo(hDlg,message,wParam,lParam);

				SetTimer(hDlg,IDC_DEVTYPE_TIMER,TIME_1S);
				
			} break;

		case MSG_KEYBOARD :
			ProUserKeyBoard(hDlg,wParam);
			break;

		case MSG_DEVTYPE_UPDATE_REMINDER :
			SetWindowText(GetDlgItem(hDlg,IDC_DEVTYPE_LB_NOTY),(char *)lParam);
			break;

		case MSG_TIMER:
			{
				if (wParam == IDC_DEVTYPE_TIMER){
					if (auto_close_time > 0) {
						DBG_P("[%s]time:%d\n", __FILE__,auto_close_time);
						if (--auto_close_time == 0) {
							SendNotifyMessage(hDlg,MSG_CLOSE,0,0);
						}
					}
				}
			} break;

		case MSG_ERASEBKGND:
			if (Bmp_background != NULL)
				DrawBackground(hDlg, (HDC)wParam, (const RECT*)lParam,Bmp_background);
			return 0;

		case MSG_CLOSE:
			{
				HWND Form = Screen.Find("TFrmDevType");
				SendNotifyMessage(Form,MSG_SETTING_RESET_TIME,0,0);
				KillTimer (hDlg,IDC_DEVTYPE_TIMER);
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
 * @brief CreateSetDevTypeForm 创建机型设置窗口
 *
 * @param hMainWnd
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
int CreateSetDevTypeForm(HWND hMainWnd,BITMAP *Bmp_bkg)
{
	HWND Form = Screen.Find("TFrmDevType");
	Bmp_background = Bmp_bkg;
	if(Form) {
		ShowWindow(Form,SW_SHOWNORMAL);
		return Form;
	} else {
		return CreateMyWindowIndirectParam(&DlgInitParam,hMainWnd,SetDevTypeProc,0);
	}
}

