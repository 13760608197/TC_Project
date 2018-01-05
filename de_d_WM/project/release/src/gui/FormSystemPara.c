/*
 * =====================================================================================
 *
 *       Filename:  FormSystemPara.c
 *
 *    Description:  系统参数设置
 *
 *        Version:  1.0
 *        Created:  2015-11-06 10:38:39
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
static void Save_notify(HWND hwnd, int id, int nc, DWORD add_data);

/* ----------------------------------------------------------------*
 *                        macro define
 *-----------------------------------------------------------------*/
#if DBG_SYS_PARA > 0
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

#define W_LB_TITLE	SCR_WIDTH / 2
#define H_LB_TITLE	FONT_HEIGHT
#define X_LB_TITLE	0
#define Y_LB_TITLE	5

#define W_LB_NAME	SCR_WIDTH / 5 * 3
#define H_LB_NAME	FONT_HEIGHT
#define X_LB_NAME	10
#define Y_LB_NAME	5 + H_LB_TITLE

#define W_EDIT	SCR_WIDTH / 5 * 2
#define H_EDIT	FONT_HEIGHT
#define X_EDIT	AREA_WIDTH - SCR_WIDTH / 5 * 2 + 5
#define Y_EDIT	Y_LB_NAME + H_LB_NAME + INTERVAL

#define W_BUTTON	70
#define H_BUTTON	FONT_HEIGHT
#define X_BUTTON	AREA_WIDTH - W_BUTTON + 5

#define W_MENU	SCR_WIDTH / 5 * 2
#define H_MENU	H_EDIT

#define _Y_LOCATION(x) Y_LB_NAME + ((FONT_HEIGHT+6) * x)
#endif

#if (defined TC3000_18S)
#define TEXT_FONT font18
#define BUTTON_FONT font14
#define AREA_WIDTH SCR_WIDTH - 20
#define FONT_HEIGHT 24
#define INTERVAL	5
#define BUTTON_WIDTH 50
#define BUTTON_HEIGHT FONT_HEIGHT + INTERVAL

#define W_LB_TITLE	SCR_WIDTH / 2
#define H_LB_TITLE	FONT_HEIGHT
#define X_LB_TITLE	0
#define Y_LB_TITLE	5

#define W_LB_NAME	SCR_WIDTH / 5 * 3
#define H_LB_NAME	FONT_HEIGHT
#define X_LB_NAME	10
#define Y_LB_NAME	5 + H_LB_TITLE

#define W_EDIT	SCR_WIDTH / 5 * 2
#define H_EDIT	FONT_HEIGHT
#define X_EDIT	AREA_WIDTH - SCR_WIDTH / 5 * 2 + 5
#define Y_EDIT	Y_LB_NAME + H_LB_NAME + INTERVAL

#define W_BUTTON	70
#define H_BUTTON	FONT_HEIGHT
#define X_BUTTON	AREA_WIDTH - W_BUTTON + 5

#define W_MENU	SCR_WIDTH / 5 * 2
#define H_MENU	H_EDIT

#define _Y_LOCATION(x) Y_LB_NAME + ((FONT_HEIGHT+6) * x)
#endif
/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
static BITMAP *Bmp_background = NULL;
static HWND hCtrl;
static int auto_close_time;
static int enable_alarm_door,enable_password,enable_unbroken;
static int Isdisplay = 0;
static MY_CTRLDATA ChildCtrls [] = {

	STATIC_LB  (X_LB_TITLE, Y_LB_TITLE, W_LB_TITLE, H_LB_TITLE,
			IDC_SYSPARA_LB_TITLE,
			Word[WORD_SETTING_SYSTEM].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(0), W_LB_NAME, H_LB_NAME,
			IDC_SYSPARA_LB_LANGUAGE,
			Word[WORD_LANGUAGE].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(1), W_LB_NAME, H_LB_NAME,
			IDC_SYSPARA_LB_VOLUME,
			"",
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(2), W_LB_NAME, H_LB_NAME,
			IDC_SYSPARA_LB_FRAME_PERIOD,
			"",
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(3), W_LB_NAME, H_LB_NAME,
			IDC_SYSPARA_LB_ALARM_DOOR,
			"",
			0,
			&TEXT_FONT),
	STATIC_LB(X_LB_TITLE + W_LB_TITLE, Y_LB_TITLE, W_LB_TITLE, H_LB_TITLE,
			IDC_SYSPARA_LB_REMINDER,
			"",
			0,
			&TEXT_FONT),
	STATIC_LB(0, _Y_LOCATION(7) + 10,  SCR_WIDTH, H_LB_NAME,
			IDC_SYSPARA_LB_REMINDER_BT,
			"",
			0,
			&TEXT_FONT),
	GROUP_COMBOBOX(AREA_WIDTH - W_MENU + 5, _Y_LOCATION(0), W_MENU, H_LB_NAME,
			IDC_SYSPARA_MENU_LANGUAGE,
			"",
			0,
			0),
	EDIT_L(X_EDIT, _Y_LOCATION(1), W_EDIT, H_EDIT,
			IDC_SYSPARA_EDIT_VOLUME,
			"",
			0,
			0),
	EDIT_L(X_EDIT, _Y_LOCATION(2), W_EDIT, H_EDIT,
			IDC_SYSPARA_EDIT_FRAME_PERIOD,
			"",
			0,
			0),
	EDIT_L(X_EDIT, _Y_LOCATION(3), W_EDIT, H_EDIT,
			IDC_SYSPARA_EDIT_ALAMR_DOOR,
			"",
			0,
			0),
	COMBOBOX(X_LB_NAME, _Y_LOCATION(4),  W_MENU, H_LB_NAME,
			IDC_SYSPARA_MENU_VIDEO_STYLE,
			"",
			0,
			0),
	COMBOBOX(AREA_WIDTH - W_MENU + 5, _Y_LOCATION(4),  W_MENU, H_LB_NAME,
			IDC_SYSPARA_MENU_VIDEO_RESOLUTION,
			"",
			0,
			0),
	BUTTON_CHECKVOX(0, _Y_LOCATION(5), SCR_WIDTH - W_BUTTON, H_BUTTON,
			IDC_SYSPARA_BT_ENABLE_ALARM_DOOR,
			Word[WORD_ENABLE_ALRM_DOOR].string,
			0,
			&BUTTON_FONT),
	BUTTON_CHECKVOX(0, _Y_LOCATION(5) + 18, SCR_WIDTH - W_BUTTON, H_BUTTON,
			IDC_SYSPARA_BT_ENABLE_PASSWORD,
			Word[WORD_ENABLE_PASSWORD].string,
			0,
			&BUTTON_FONT),
	BUTTON_CHECKVOX(0, _Y_LOCATION(5) + 36, SCR_WIDTH - W_BUTTON, H_BUTTON,
			IDC_SYSPARA_BT_ENABLE_UNBROKEN,
			Word[WORD_ENABLE_UNBROKEN].string,
			0,
			&BUTTON_FONT),
	BUTTON_LB(X_BUTTON, _Y_LOCATION(5), W_BUTTON, H_BUTTON,
			IDC_SYSPARA_BT_SAVE,
			Word[WORD_SAVE].string,
			0,
			&BUTTON_FONT),
	BUTTON_LB(X_BUTTON, _Y_LOCATION(6), W_BUTTON, H_BUTTON,
			IDC_SYSPARA_BT_BACK,
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

static struct topmsg_st topmsg = {
	.title = Word[WORD_COMFIRM_AND_REBOOT].string,
	.notify_func =  Save_notify,
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
	if (id >= IDC_SYSPARA_BT_ENABLE_ALARM_DOOR) {
		return CTRL_TYPE_SYSPARA_BUTTON;
	} else if (id >= IDC_SYSPARA_EDIT_VOLUME){
		return CTRL_TYPE_SYSPARA_EDIT;
	} else {
		return CTRL_TYPE_SYSPARA_MENU;
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
	SendMessage(hwnd, MSG_SYSPARA_UPDATE_REMINDER_BT,
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
	SendMessage(hwnd, MSG_SYSPARA_UPDATE_REMINDER,
			0,
			(LPARAM)string);
}

/* ----------------------------------------------------------------*/
/**
 * @brief Save_notify 按下保存按键时，如果修改过语言需要重启
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void Save_notify(HWND hwnd, int id, int nc, DWORD add_data)
{
	Isdisplay = 1;
	auto_close_time = FORM_SETTING_ONTIME;
	if (nc == FALSE) {
		return;
	}
	printf("exit the process!!\n");
	exit(0);//直接退出程序
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
	char buf[64];
	int volume,frame_period,alarm_door_time,language,video_type;
	int language_change = 0;
	int video_change = 0;
	language = SendMessage (GetDlgItem(GetParent(hwnd),IDC_SYSPARA_MENU_LANGUAGE), CB_GETCURSEL, 0, 0);
	if (language != Public.language) {
		language_change = 1;
		Public.language = language;
		if (Public.language == CHINEASE)
			SetValueToEtcFile("interface.ini","PUBLIC","Language","中文");
		else if (Public.language == ENGLISH)
			SetValueToEtcFile("interface.ini","PUBLIC","Language","English");
		else if (Public.language == TURKISH)
			SetValueToEtcFile("interface.ini","PUBLIC","Language","Turkish");
		else if (Public.language == TAIWAN)
			SetValueToEtcFile("interface.ini","PUBLIC","Language","Taiwan");
		else if (Public.language == RUSSIAN)
			SetValueToEtcFile("interface.ini","PUBLIC","Language","Russian");	
		else if (Public.language == UKRAINIAN)
			SetValueToEtcFile("interface.ini","PUBLIC","Language","Ukrainian");
	}

	video_type = SendMessage (GetDlgItem(GetParent(hwnd),IDC_SYSPARA_MENU_VIDEO_RESOLUTION), CB_GETCURSEL, 0, 0);
	if (video_type != Public.VideoType) {
		video_change = 1;
		Public.VideoType = video_type;
		sprintf(buf,"%d",Public.VideoType);
		SetValueToEtcFile("interface.ini","PUBLIC","VideoType",buf);
	}
	SendMessage (GetDlgItem(GetParent(hwnd),IDC_SYSPARA_EDIT_VOLUME), MSG_GETTEXT, 20, (LPARAM) buf);
	volume = atoi(buf);
	Public.CurrentVolume = volume;
	Mixer->SetVolumeEx(Mixer,Public.CurrentVolume);
	sprintf(buf,"%d",volume);
	SetValueToEtcFile("interface.ini","PUBLIC","CurrentVolume",buf);
	SendMessage (GetDlgItem(GetParent(hwnd),IDC_SYSPARA_EDIT_FRAME_PERIOD), MSG_GETTEXT, 20, (LPARAM) buf);
	frame_period = atoi(buf);
	Public.FramPeriod = frame_period;
	sprintf(buf,"%d",frame_period);
	SetValueToEtcFile("interface.ini","PUBLIC","FramPeriod",buf);

	SendMessage (GetDlgItem(GetParent(hwnd),IDC_SYSPARA_EDIT_ALAMR_DOOR), MSG_GETTEXT, 20, (LPARAM) buf);
	alarm_door_time = atoi(buf);
	Public.AlarmDoorTime = alarm_door_time;
	sprintf(buf,"%d",alarm_door_time);
	SetValueToEtcFile("interface.ini","PUBLIC","AlarmDoorTime",buf);

	Public.EnableAlarmDoor = enable_alarm_door;
	sprintf(buf,"%d",enable_alarm_door);
	SetValueToEtcFile("interface.ini","MODE","EnableAlarmDoor",buf);

	Public.EnableDoorPassword = enable_password;
	sprintf(buf,"%d",enable_password);
	SetValueToEtcFile("interface.ini","MODE","EnableDoorPassword",buf);

	Public.bUnbrokenUse = enable_unbroken;
	sprintf(buf,"%d",enable_unbroken);
	SetValueToEtcFile_Ext("interface.ini","MODE","Broken",buf);

	if (Public.EnableAlarmDoor)
		uart->ToSingleChip(uart,UART_ALARMDOOR,Public.AlarmDoorTime);
	else 
		uart->ToSingleChip(uart,UART_ALARMDOOR,0);

	if (Public.bUnbrokenUse) 
		uart->ToSingleChip(uart,UART_ENALARM);
	else 
		uart->ToSingleChip(uart,UART_DISALARM);
	// form_main->timerStart(IDC_TIMER_SEND_CMD_100ms);
	// SetTimer(Screen.hMainWnd,IDC_MAIN_TIMER_100ms,TIME_1S / 10); // 向单片机发送协议

	sprintf(buf,"%s%s",Word[WORD_SETTING].string,Word[WORD_SUCCESS].string);
	UpdateReminder(GetParent(hwnd),buf);
	if (language_change || video_change) {
		Isdisplay = 0;	
		CreateTopMessageForm(GetParent(hwnd),Bmp_background,&topmsg);
	}
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
 * @brief btEnableAlarmDoor 使能开门报警按钮回调函数
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void btEnableAlarmDoor(HWND hwnd, int id, int nc, DWORD add_data)
{
	enable_alarm_door ^= 1;
}

/* ----------------------------------------------------------------*/
/**
 * @brief btEnablePassword 使能密码开锁功能按钮回调函数
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void btEnablePassword(HWND hwnd, int id, int nc, DWORD add_data)
{
	enable_password ^= 1;
}

/* ----------------------------------------------------------------*/
/**
 * @brief btEnableUbroken 使能防拆功能按键
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void btEnableUbroken(HWND hwnd, int id, int nc, DWORD add_data)
{
	enable_unbroken ^= 1;
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
		switch (id)
		{
			case IDC_SYSPARA_EDIT_VOLUME:
				{
					if (num > 99)
						num = 99;
					sprintf(text,"%d",num);
					SetWindowText(GetDlgItem(GetParent(hwnd),id),text);
				} break;

			case IDC_SYSPARA_EDIT_FRAME_PERIOD:
				{
					if (num > 20)
						num = 20;
					sprintf(text,"%d",num);
					SetWindowText(GetDlgItem(GetParent(hwnd),id),text);
				} break;

			case IDC_SYSPARA_EDIT_ALAMR_DOOR:
				{
					if (num > 255)
						num = 255;
					sprintf(text,"%d",num);
					SetWindowText(GetDlgItem(GetParent(hwnd),id),text);
				} break;

			default:
				break;
		}
	} else if (nc == EN_KILLFOCUS) {
		SendMessage (GetDlgItem(GetParent(hwnd),id), MSG_GETTEXT, 50,  (LPARAM)text);
		num = atoi(text);
		switch (id)
		{
			case IDC_SYSPARA_EDIT_FRAME_PERIOD:
				{
					if (num < 5)
						num = 5;
					sprintf(text,"%d",num);
					SetWindowText(GetDlgItem(GetParent(hwnd),id),text);
				} break;

			case IDC_SYSPARA_EDIT_ALAMR_DOOR:
				{
					if (num < 1)
						num = 1;
					sprintf(text,"%d",num);
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

				int id = GetDlgCtrlID(hCtrl);
				switch (JudgeCtrlType(id))
				{
					case CTRL_TYPE_SYSPARA_EDIT:
						{
							if (ctrl_type != CTRL_TYPE_SYSPARA_EDIT) {
								UpdateBtReminder(hDlg,Word[WORD_HELP_EDIT].string);
							}
							ctrl_type = CTRL_TYPE_SYSPARA_EDIT;
						} break;

					case CTRL_TYPE_SYSPARA_BUTTON:
						{
							if (ctrl_type != CTRL_TYPE_SYSPARA_BUTTON) {
								UpdateBtReminder(hDlg,Word[WORD_HELP_BUTTON].string);
							}
							ctrl_type = CTRL_TYPE_SYSPARA_BUTTON;
						} break;

					case CTRL_TYPE_SYSPARA_MENU:
						{
							if (ctrl_type != CTRL_TYPE_SYSPARA_MENU) {
								UpdateBtReminder(hDlg,Word[WORD_HELP_MENU].string);
							}
							ctrl_type = CTRL_TYPE_SYSPARA_MENU;
						} break;

					default:
						break;
				}
			} break;

		case KEYOK:
			{
				int id = GetDlgCtrlID(hCtrl);
				if (JudgeCtrlType(id) == CTRL_TYPE_SYSPARA_EDIT) {
					SendMessage(hCtrl,MSG_CHAR,'\b',0);
				} else if (JudgeCtrlType(id) == CTRL_TYPE_SYSPARA_MENU){
					SendMessage(hDlg,MSG_KEYDOWN,SCANCODE_ENTER,0);
				} else {
					SendMessage(hCtrl,BM_CLICK,0,0);
				}
			} break;

		default:
			{
				int id = GetDlgCtrlID(hCtrl);
				if (JudgeCtrlType(id) == CTRL_TYPE_SYSPARA_EDIT) {
					if (KeyVal != KEYLEAVEWORD) {
						SendMessage(hCtrl,MSG_CHAR,'0' + KeyVal,0);
					}
				} else if (JudgeCtrlType(id) == CTRL_TYPE_SYSPARA_MENU) {
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
 * @brief InitSysPara 初始化编辑框
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 */
/* ----------------------------------------------------------------*/
static void InitSysPara(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	char buf[512];
	sprintf(buf,"%s(0-99)",Word[WORD_VOLUME_SET].string);
	SetWindowText(GetDlgItem(hDlg,IDC_SYSPARA_LB_VOLUME),buf);
	sprintf(buf,"%d",Public.CurrentVolume);
	SetWindowText(GetDlgItem(hDlg,IDC_SYSPARA_EDIT_VOLUME),buf);

	sprintf(buf,"%s(5-20FPS)",Word[WORD_FRAME_PERIOD].string);
	SetWindowText(GetDlgItem(hDlg,IDC_SYSPARA_LB_FRAME_PERIOD),buf);
	sprintf(buf,"%d",Public.FramPeriod);
	SetWindowText(GetDlgItem(hDlg,IDC_SYSPARA_EDIT_FRAME_PERIOD),buf);

	sprintf(buf,"%s(1-255s)",Word[WORD_ALRM_DOOR].string);
	SetWindowText(GetDlgItem(hDlg,IDC_SYSPARA_LB_ALARM_DOOR),buf);
	sprintf(buf,"%d",Public.AlarmDoorTime);
	SetWindowText(GetDlgItem(hDlg,IDC_SYSPARA_EDIT_ALAMR_DOOR),buf);
	SendDlgItemMessage(hDlg, IDC_SYSPARA_MENU_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)"Chinese");
	SendDlgItemMessage(hDlg, IDC_SYSPARA_MENU_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)"English");
	SendDlgItemMessage(hDlg, IDC_SYSPARA_MENU_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)"Turkish");
	SendDlgItemMessage(hDlg, IDC_SYSPARA_MENU_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)"Taiwan");
	SendDlgItemMessage(hDlg, IDC_SYSPARA_MENU_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)"Russian");
	SendDlgItemMessage(hDlg, IDC_SYSPARA_MENU_LANGUAGE, CB_ADDSTRING, 0, (LPARAM)"Ukrainin");
	SendDlgItemMessage(hDlg, IDC_SYSPARA_MENU_LANGUAGE, CB_SETCURSEL, Public.language, 0);

	SendDlgItemMessage(hDlg, IDC_SYSPARA_MENU_VIDEO_STYLE, CB_ADDSTRING, 0, (LPARAM)"H.264");
	SendDlgItemMessage(hDlg, IDC_SYSPARA_MENU_VIDEO_STYLE, CB_SETCURSEL, 0, 0);

	SendDlgItemMessage(hDlg, IDC_SYSPARA_MENU_VIDEO_RESOLUTION, CB_ADDSTRING, 0, (LPARAM)"320x240");
	SendDlgItemMessage(hDlg, IDC_SYSPARA_MENU_VIDEO_RESOLUTION, CB_ADDSTRING, 0, (LPARAM)"640x480");
	SendDlgItemMessage(hDlg, IDC_SYSPARA_MENU_VIDEO_RESOLUTION, CB_ADDSTRING, 0, (LPARAM)"1280x720");
	SendDlgItemMessage(hDlg, IDC_SYSPARA_MENU_VIDEO_RESOLUTION, CB_SETCURSEL, Public.VideoType, 0);

	if (enable_alarm_door != Public.EnableAlarmDoor) {
		SendDlgItemMessage(hDlg,IDC_SYSPARA_BT_ENABLE_ALARM_DOOR,BM_CLICK,0,0);
	}

	if (enable_password != Public.EnableDoorPassword) {
		SendDlgItemMessage(hDlg,IDC_SYSPARA_BT_ENABLE_PASSWORD,BM_CLICK,0,0);
	}

	if (enable_unbroken != Public.bUnbrokenUse) {
		SendDlgItemMessage(hDlg,IDC_SYSPARA_BT_ENABLE_UNBROKEN,BM_CLICK,0,0);
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief SysParaProc 窗口回调函数
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 *
 * @return
 */
/* ----------------------------------------------------------------*/
static int SysParaProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	switch (message)
	{
		case MSG_INITDIALOG:
			{
				Screen.Add(hDlg,"TFrmSysP");
				hCtrl = 0;
				enable_alarm_door = 0;
				enable_password = 0;
				enable_unbroken = 0;
				Isdisplay = 1;
				auto_close_time = FORM_SETTING_ONTIME;
				int i;
				for (i=IDC_SYSPARA_LB_TITLE;i<=IDC_SYSPARA_LB_ALARM_DOOR; i++){
					// hdc = BeginPaint (GetDlgItem(hDlg,i));
					// SetTextColor(hdc, COLOR_lightwhite);
					// TextOut(hdc, 0,0, "This is a sample");
					// EndPaint (GetDlgItem(hDlg,i), hdc);

					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW,MY_WHIGHT);
				}
				for (i=IDC_SYSPARA_EDIT_VOLUME;i<=IDC_SYSPARA_EDIT_ALAMR_DOOR; i++){
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW,MY_BLACK); //黑色
					SendDlgItemMessage(hDlg,i, EM_LIMITTEXT, 20, 0L);
					SetNotificationCallback (GetDlgItem(hDlg,i), edit_notify);
				}
				for (i=IDC_SYSPARA_MENU_LANGUAGE;i<=IDC_SYSPARA_MENU_VIDEO_RESOLUTION; i++){
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW,MY_BLACK);
				}

				SetWindowElementAttr(GetDlgItem(hDlg,IDC_SYSPARA_LB_REMINDER),
						WE_FGC_WINDOW,MY_RED);
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_SYSPARA_LB_REMINDER_BT),
						WE_FGC_WINDOW,MY_RED);
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_SYSPARA_BT_ENABLE_ALARM_DOOR),
						WE_FGC_THREED_BODY,MY_WHIGHT);
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_SYSPARA_BT_ENABLE_PASSWORD),
						WE_FGC_THREED_BODY,MY_WHIGHT);
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_SYSPARA_BT_ENABLE_UNBROKEN),
						WE_FGC_THREED_BODY,MY_WHIGHT);

				SetNotificationCallback (GetDlgItem(hDlg,IDC_SYSPARA_BT_SAVE), btSave);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_SYSPARA_BT_BACK), btBack);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_SYSPARA_BT_ENABLE_ALARM_DOOR), btEnableAlarmDoor);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_SYSPARA_BT_ENABLE_PASSWORD), btEnablePassword);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_SYSPARA_BT_ENABLE_UNBROKEN), btEnableUbroken);

				for (i = IDC_SYSPARA_MENU_LANGUAGE; i <= IDC_SYSPARA_MENU_VIDEO_RESOLUTION; ++i) {
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW,MY_BLACK_TOP); //黑色
				}
				InitSysPara(hDlg, message, wParam, lParam);
				UpdateBtReminder(hDlg,Word[WORD_HELP_BUTTON].string);
				SetTimer(hDlg,IDC_SYSPARA_TIMER,TIME_1S);
			} break;

		case MSG_KEYBOARD :
			ProUserKeyBoard(hDlg,wParam);
			break;

		case MSG_SYSPARA_UPDATE_REMINDER :
			SetWindowText(GetDlgItem(hDlg,IDC_SYSPARA_LB_REMINDER),(char *)lParam);
			break;

		case MSG_SYSPARA_UPDATE_REMINDER_BT :
			SetWindowText(GetDlgItem(hDlg,IDC_SYSPARA_LB_REMINDER_BT),(char *)lParam);
			break;

		case MSG_TIMER:
			{
				if (wParam == IDC_SYSPARA_TIMER){
					if ((auto_close_time > 0) && Isdisplay) {
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
				KillTimer (hDlg,IDC_SYSPARA_TIMER);
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
 * @brief CreateSysParaForm 创建系统参数窗口
 *
 * @param hMainWnd
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
int CreateSysParaForm(HWND hMainWnd,BITMAP *Bmp_bkg)
{
	HWND Form = Screen.Find("TFrmSysP");
	Bmp_background = Bmp_bkg;
	if(Form) {
		ShowWindow(Form,SW_SHOWNORMAL);
		return Form;
	} else {
		return CreateMyWindowIndirectParam(&DlgInitParam,hMainWnd,SysParaProc,0);
	}
}

