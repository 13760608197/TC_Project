/*
 * =====================================================================================
 *
 *       Filename:  FormSetElevator.c
 *
 *    Description:  电梯联动设置窗口
 *
 *        Version:  1.0
 *        Created:  2015-11-10 14:28:09
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
#if DBG_SET_ELEVATOR > 0
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
#define BUTTON_HEIGHT FONT_HEIGHT + INTERVAL

#define W_LB_TITLE	SCR_WIDTH
#define H_LB_TITLE	FONT_HEIGHT
#define X_LB_TITLE	0
#define Y_LB_TITLE	5

#define W_LB_NAME	SCR_WIDTH / 10 * 4
#define H_LB_NAME	FONT_HEIGHT
#define X_LB_NAME	50
#define Y_LB_NAME	5

#define W_EDIT	50
#define H_EDIT	FONT_HEIGHT
#define X_EDIT	X_LB_NAME + SCR_WIDTH / 2
#define Y_EDIT	Y_LB_NAME + H_LB_NAME + INTERVAL

#define W_BUTTON	60
#define H_BUTTON	FONT_HEIGHT + 5
#define X_BUTTON	SCR_WIDTH / 4

#define _Y_LOCATION(x) Y_LB_NAME + ((FONT_HEIGHT+12) * x)
#define _X_BUTTON_LOCATION(x) X_BUTTON + ((W_BUTTON+40) * x)
#endif
#if (defined TC3000_18S)
#define TEXT_FONT font18
#define BUTTON_FONT font14
#define AREA_WIDTH SCR_WIDTH - 20
#define FONT_HEIGHT 25
#define INTERVAL	5
#define BUTTON_HEIGHT FONT_HEIGHT + INTERVAL

#define W_LB_TITLE	SCR_WIDTH
#define H_LB_TITLE	FONT_HEIGHT
#define X_LB_TITLE	0
#define Y_LB_TITLE	5

#define W_LB_NAME	SCR_WIDTH / 10 * 4
#define H_LB_NAME	FONT_HEIGHT
#define X_LB_NAME	50
#define Y_LB_NAME	5

#define W_EDIT	50
#define H_EDIT	FONT_HEIGHT
#define X_EDIT	X_LB_NAME + SCR_WIDTH / 2
#define Y_EDIT	Y_LB_NAME + H_LB_NAME + INTERVAL

#define W_BUTTON	60
#define H_BUTTON	FONT_HEIGHT + 5
#define X_BUTTON	SCR_WIDTH / 4

#define _Y_LOCATION(x) Y_LB_NAME + ((FONT_HEIGHT+12) * x)
#define _X_BUTTON_LOCATION(x) X_BUTTON + ((W_BUTTON+40) * x)
#endif
/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
static BITMAP *Bmp_background = NULL; 
static HWND hCtrl;
static int auto_close_time;
static int enable_elevator;
static MY_CTRLDATA ChildCtrls [] = {
	STATIC_LB  (X_LB_TITLE, Y_LB_TITLE, W_LB_TITLE, H_LB_TITLE,
			IDC_SETELE_LB_TITLE,
			Word[WORD_SETTING_ELEVATOR].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(1), W_LB_NAME, H_LB_NAME,
			IDC_SETELE_LB_ADDRESS,
			Word[WORD_ELEVATOR_ADDRESS].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(2), W_LB_NAME, H_LB_NAME,
			IDC_SETELE_LB_FLOOR,
			Word[WORD_FLOOR_NUM].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(3), W_LB_NAME, H_LB_NAME,
			IDC_SETELE_LB_LOBBY,
			Word[WORD_ELEVATOR_LOBBY].string,
			0,
			&TEXT_FONT),
	STATIC_LB(0, _Y_LOCATION(6), SCR_WIDTH, H_LB_NAME,
			IDC_SETELE_LB_REMINDER,
			"",
			0,
			&BUTTON_FONT),
	STATIC_LB(0, _Y_LOCATION(6) + H_LB_NAME, SCR_WIDTH, H_LB_NAME,
			IDC_SETELE_LB_REMINDER_BT,
			"",
			0,
			&TEXT_FONT),

	GROUP_EDIT_L(X_EDIT, _Y_LOCATION(1), W_EDIT, H_EDIT,
			IDC_SETELE_EDIT_ADDRESS,
			"",
			0,
			0),
	EDIT_L(X_EDIT, _Y_LOCATION(2), W_EDIT, H_EDIT,
			IDC_SETELE_EDIT_FLOOR,
			"",
			0,
			0),
	EDIT_L(X_EDIT, _Y_LOCATION(3), W_EDIT, H_EDIT,
			IDC_SETELE_EDIT_LOBBY,
			"",
			0,
			0),
	BUTTON_CHECKVOX(X_LB_NAME, _Y_LOCATION(4), SCR_WIDTH, H_LB_NAME,
			IDC_SETELE_BT_ENABLE_EVEVATOR,
			Word[WORD_ENABLE_ELEVATOR].string,
			0,
			&BUTTON_FONT),
	BUTTON_LB(_X_BUTTON_LOCATION(0), _Y_LOCATION(5), W_BUTTON, H_BUTTON,
			IDC_SETELE_BT_SAVE,
			Word[WORD_SAVE].string,
			0,
			&BUTTON_FONT),
	BUTTON_LB(_X_BUTTON_LOCATION(1), _Y_LOCATION(5), W_BUTTON, H_BUTTON,
			IDC_SETELE_BT_BACK,
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
		CTRL_TYPE_SETELE_EDIT,编辑框
		CTRL_TYPE_SETELE_BUTTON,按钮
 */
/* ----------------------------------------------------------------*/
static int JudgeCtrlType(int id)
{
	if (id >= IDC_SETELE_BT_ENABLE_EVEVATOR) {
		return CTRL_TYPE_SETELE_BUTTON;
	} else {
		return	CTRL_TYPE_SETELE_EDIT;
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
	SendMessage(hwnd, MSG_SETELE_UPDATE_REMINDER_BT,
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
	SendMessage(hwnd, MSG_SETELE_UPDATE_REMINDER,
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
	char buf[32];
	Public.eElevator =  enable_elevator;
	sprintf(buf,"%d",Public.eElevator);
	SetValueToEtcFile("interface.ini","MODE","EnableElevator",buf);

	SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETELE_EDIT_ADDRESS), 
			MSG_GETTEXT, 10,  (LPARAM)buf);
	Public.ElevatorAddr = atoi(buf);
	SetValueToEtcFile("interface.ini","PUBLIC","ElevatorAddr",buf);

	SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETELE_EDIT_FLOOR), 
			MSG_GETTEXT, 10,  (LPARAM)buf);
	Public.ElevatorFloorCode = atoi(buf);
	SetValueToEtcFile_Ext("interface.ini","PUBLIC","ElevatorFloorCode",buf);
	
	SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETELE_EDIT_LOBBY), 
			MSG_GETTEXT, 10,  (LPARAM)buf);
	Public.ElevatorLobby = atoi(buf);
	SetValueToEtcFile_Ext("interface.ini","PUBLIC","ElevatorLobby",buf);
	
	sprintf(buf,"%s%s",Word[WORD_SAVE].string,Word[WORD_SUCCESS].string);
	UpdateReminder(GetParent(hwnd),buf);
	// printf("en:%d,add:%d,floor:%d\n", Public.eElevator,Public.ElevatorAddr,Public.ElevatorFloorCode);
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
 * @brief btEnableElevator 使能启动电梯联动按钮
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void btEnableElevator(HWND hwnd, int id, int nc, DWORD add_data)
{
	enable_elevator ^= 1;
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
	int num;
	if ((nc == EN_UPDATE) || (nc == EN_SETFOCUS)){
		int len = SendMessage (GetDlgItem(GetParent(hwnd),id), MSG_GETTEXT, 50,  (LPARAM)text);
		SendMessage (GetDlgItem(GetParent(hwnd),id), EM_SETCARETPOS, 0, len);
	} else if (nc == EN_CHANGE) {
		SendMessage (GetDlgItem(GetParent(hwnd),id), MSG_GETTEXT, 50,  (LPARAM)text);
		num = atoi(text);		
	} else if (nc == EN_KILLFOCUS) {
		SendMessage (GetDlgItem(GetParent(hwnd),id), MSG_GETTEXT, 50,  (LPARAM)text);
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
					case CTRL_TYPE_SETELE_EDIT:
						{
							if (ctrl_type != CTRL_TYPE_SETELE_EDIT) {
								UpdateBtReminder(hDlg,Word[WORD_HELP_EDIT_ELEVATOR].string);
							}
							ctrl_type = CTRL_TYPE_SETELE_EDIT;
						} break;

					case CTRL_TYPE_SETELE_BUTTON:
						{
							if (ctrl_type != CTRL_TYPE_SETELE_BUTTON) {
								UpdateBtReminder(hDlg,Word[WORD_HELP_BUTTON].string);
							}
							ctrl_type = CTRL_TYPE_SETELE_BUTTON;
						} break;

					default:
						break;
				}
			} break;

		case KEYOK:
			{
				switch (JudgeCtrlType(GetDlgCtrlID(hCtrl)))
				{
					case CTRL_TYPE_SETELE_EDIT:
						{
							SendMessage(hCtrl,MSG_CHAR,'\b',0);
						} break;

					case CTRL_TYPE_SETELE_BUTTON:
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
					case CTRL_TYPE_SETELE_EDIT:
						{
							if (KeyVal == KEYLEAVEWORD) {
								SendMessage(hCtrl,MSG_CHAR,'-',0);
							} else {
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
 * @brief InitSetElevator 初始化电梯编辑款及复选按钮
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 */
/* ----------------------------------------------------------------*/
static void InitSetElevator(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	char buf[64];
	sprintf(buf,"%d",Public.ElevatorAddr);
	SetWindowText(GetDlgItem(hDlg,IDC_SETELE_EDIT_ADDRESS),buf);
	sprintf(buf,"%d",Public.ElevatorFloorCode);
	SetWindowText(GetDlgItem(hDlg,IDC_SETELE_EDIT_FLOOR),buf);
	sprintf(buf,"%d",Public.ElevatorLobby);
	SetWindowText(GetDlgItem(hDlg,IDC_SETELE_EDIT_LOBBY),buf);
	if (enable_elevator != Public.eElevator) {
		SendDlgItemMessage(hDlg,IDC_SETELE_BT_ENABLE_EVEVATOR,BM_CLICK,0,0);
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief SetElevatorProc 窗口回调函数
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 *
 * @return
 */
/* ----------------------------------------------------------------*/
static int SetElevatorProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_INITDIALOG:
			{
				Screen.Add(hDlg,"TFrmSEle");
				auto_close_time = FORM_SETTING_ONTIME;
				hCtrl = 0;
				enable_elevator = 0;
				int i;
				for (i=IDC_SETELE_LB_TITLE;i<=IDC_SETELE_LB_LOBBY; i++){
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW,MY_WHIGHT);
				}
				for (i=IDC_SETELE_EDIT_ADDRESS;i<=IDC_SETELE_EDIT_LOBBY; i++){
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW,MY_BLACK);
					SendDlgItemMessage(hDlg,i, EM_LIMITTEXT, 10, 0L);
					SetNotificationCallback (GetDlgItem(hDlg,i), edit_notify);
				}
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_SETELE_LB_REMINDER),
						WE_FGC_WINDOW,MY_RED);
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_SETELE_LB_REMINDER_BT),
						WE_FGC_WINDOW,MY_RED);
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_SETELE_BT_ENABLE_EVEVATOR),
						WE_FGC_THREED_BODY,MY_WHIGHT);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_SETELE_BT_SAVE), btSave);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_SETELE_BT_BACK), btBack);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_SETELE_BT_ENABLE_EVEVATOR), btEnableElevator);

				InitSetElevator(hDlg, message, wParam, lParam);
				UpdateBtReminder(hDlg,Word[WORD_HELP_BUTTON].string);
				SetTimer(hDlg,IDC_SETELE_TIMER,TIME_1S);
			} break;

		case MSG_KEYBOARD :
			ProUserKeyBoard(hDlg,wParam);
			break;

		case MSG_SETELE_UPDATE_REMINDER :
			SetWindowText(GetDlgItem(hDlg,IDC_SETELE_LB_REMINDER),(char *)lParam);
			break;

		case MSG_SETELE_UPDATE_REMINDER_BT :
			SetWindowText(GetDlgItem(hDlg,IDC_SETELE_LB_REMINDER_BT),(char *)lParam);
			break;

		case MSG_TIMER:
			{
				if (wParam == IDC_SETELE_TIMER){
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
				HWND Form = Screen.Find("TFrmSetting");
				SendNotifyMessage(Form,MSG_SETTING_RESET_TIME,0,0);
				KillTimer(hDlg,IDC_SETELE_TIMER);
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
 * @brief CreateSetElevatorForm 创建设置电梯联动窗口
 *
 * @param hMainWnd
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
int CreateSetElevatorForm(HWND hMainWnd,BITMAP *Bmp_bkg)
{
	HWND Form = Screen.Find("TFrmSEle");
	Bmp_background = Bmp_bkg;
	if(Form) {
		ShowWindow(Form,SW_SHOWNORMAL);
		return Form;
	} else {
		return CreateMyWindowIndirectParam(&DlgInitParam,hMainWnd,SetElevatorProc,0);
	}
}

