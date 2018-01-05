/*
 * =====================================================================================
 *
 *       Filename:  FormSetting.c
 *
 *    Description:  系统设置初始界面，提示对应按键选择功能
 *
 *        Version:  1.0
 *        Created:  2015-11-02 10:07:35
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
extern PLOGFONT font18;
extern PLOGFONT font24;
extern PLOGFONT font32;

/* ----------------------------------------------------------------*
 *                  internal functions declare
 *-----------------------------------------------------------------*/
static void Reboot_notify(HWND hwnd, int id, int nc, DWORD add_data);
static void UserSetting(HWND hwnd, int id, int nc, DWORD add_data);
static void LocalSetting(HWND hwnd, int id, int nc, DWORD add_data);
static void SystemPara(HWND hwnd, int id, int nc, DWORD add_data);
static void LocalPassword(HWND hwnd, int id, int nc, DWORD add_data);
static void AccessPara(HWND hwnd, int id, int nc, DWORD add_data);
static void TimeSetting(HWND hwnd, int id, int nc, DWORD add_data);
static void ElevatorSetting(HWND hwnd, int id, int nc, DWORD add_data);
static void Reboot(HWND hwnd, int id, int nc, DWORD add_data);
static void SetDevType(HWND hwnd, int id, int nc, DWORD add_data);

/* ----------------------------------------------------------------*
 *                        macro define
 *-----------------------------------------------------------------*/
#if DBG_SETTING > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

#if (defined TC3000_DE) || (defined TC3000_X6) || (defined TC3000_DK)
#define TITLE_FONT font24
#define REMINDER_FONT font14

//标题
#define W_LB_TITLE SCR_WIDTH
#define H_LB_TITLE 30
#define X_LB_TITLE 0
#define Y_LB_TITLE 5
//软件版本
#define W_LB_SOFTWARE_VERSION SCR_WIDTH 
#define H_LB_SOFTWARE_VERSION 20
#define X_LB_SOFTWARE_VERSION 0
#define Y_LB_SOFTWARE_VERSION SCR_HEIGHT - H_LB_SOFTWARE_VERSION * 3
//发布日期
#define W_LB_DATE SCR_WIDTH / 2
#define H_LB_DATE H_LB_SOFTWARE_VERSION * 2
#define X_LB_DATE W_LB_SOFTWARE_VERSION
#define Y_LB_DATE Y_LB_SOFTWARE_VERSION
//内核版本
#define W_LB_KERNEL_VERSION SCR_WIDTH
#define H_LB_KERNEL_VERSION 20
#define X_LB_KERNEL_VERSION 0
#define Y_LB_KERNEL_VERSION Y_LB_DATE + H_LB_KERNEL_VERSION
//单片机版本
#define W_LB_SINGLE_VERSION SCR_WIDTH / 2 + 20
#define H_LB_SINGLE_VERSION 20
#define X_LB_SINGLE_VERSION 0
#define Y_LB_SINGLE_VERSION Y_LB_KERNEL_VERSION + H_LB_KERNEL_VERSION
//返回请按#键
#define W_LB_HELP SCR_WIDTH / 2 - 20
#define H_LB_HELP 20
#define X_LB_HELP W_LB_SINGLE_VERSION
#define Y_LB_HELP Y_LB_SINGLE_VERSION

//功能
#define W_LB_INFO SCR_WIDTH / 2
#define H_LB_INFO H_LB_TITLE

#define Y_LOCATION_INFO( AIndex ) (30 * AIndex + 40 )
#define STATIC_INFO(x,y,id) \
	STATIC_LB_L(x,y,W_LB_INFO,H_LB_INFO,id,"",0,&TITLE_FONT)
#endif

#if (defined TC3000_18S)
#define TITLE_FONT font32
#define REMINDER_FONT font18

//标题
#define W_LB_TITLE SCR_WIDTH
#define H_LB_TITLE 40
#define X_LB_TITLE 0
#define Y_LB_TITLE 5
//软件版本
#define W_LB_SOFTWARE_VERSION SCR_WIDTH 
#define H_LB_SOFTWARE_VERSION 22
#define X_LB_SOFTWARE_VERSION 20
#define Y_LB_SOFTWARE_VERSION SCR_HEIGHT - H_LB_SOFTWARE_VERSION * 3
//发布日期
#define W_LB_DATE SCR_WIDTH / 2
#define H_LB_DATE H_LB_SOFTWARE_VERSION * 2
#define X_LB_DATE W_LB_SOFTWARE_VERSION
#define Y_LB_DATE Y_LB_SOFTWARE_VERSION
//内核版本
#define W_LB_KERNEL_VERSION SCR_WIDTH
#define H_LB_KERNEL_VERSION 22
#define X_LB_KERNEL_VERSION 20
#define Y_LB_KERNEL_VERSION Y_LB_DATE + H_LB_KERNEL_VERSION
//单片机版本
#define W_LB_SINGLE_VERSION SCR_WIDTH / 2 + 20
#define H_LB_SINGLE_VERSION 22
#define X_LB_SINGLE_VERSION 20
#define Y_LB_SINGLE_VERSION Y_LB_KERNEL_VERSION + H_LB_KERNEL_VERSION
//返回请按#键
#define W_LB_HELP SCR_WIDTH / 2 - 20
#define H_LB_HELP 22
#define X_LB_HELP W_LB_SINGLE_VERSION
#define Y_LB_HELP Y_LB_SINGLE_VERSION

//功能
#define W_LB_INFO SCR_WIDTH / 2
#define H_LB_INFO H_LB_TITLE

#define Y_LOCATION_INFO( AIndex ) (35 * AIndex + 55 )
#define STATIC_INFO(x,y,id) \
	STATIC_LB_L(x + 20,y,W_LB_INFO,H_LB_INFO,id,"",0,&TITLE_FONT)
#endif

/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
static BITMAP *Bmp_background = NULL; 
static MY_CTRLDATA ChildCtrls [] = {

	STATIC_LB(X_LB_TITLE, Y_LB_TITLE, W_LB_TITLE, H_LB_TITLE,
			IDC_SETTING_LB_TITLE,
			Word[WORD_SETTING_PRESS_KEY].string,
			0,
			&TITLE_FONT),
	STATIC_LB_L(X_LB_SOFTWARE_VERSION, Y_LB_SOFTWARE_VERSION, W_LB_SOFTWARE_VERSION, H_LB_SOFTWARE_VERSION,
			IDC_SETTING_LB_SOFTWARE_VERSION,
			"",
			0,
			&REMINDER_FONT),
	STATIC_LB_L(X_LB_DATE, Y_LB_DATE, W_LB_DATE, H_LB_DATE,
			IDC_SETTING_LB_DATE,
			"",
			0,
			&REMINDER_FONT),
	STATIC_LB_L(X_LB_KERNEL_VERSION, Y_LB_KERNEL_VERSION, W_LB_KERNEL_VERSION, H_LB_KERNEL_VERSION,
			IDC_SETTING_LB_KERNEL_VERSION,
			"",
			0,
			&REMINDER_FONT),
	STATIC_LB_L(X_LB_SINGLE_VERSION, Y_LB_SINGLE_VERSION, W_LB_SINGLE_VERSION, H_LB_SINGLE_VERSION,
			IDC_SETTING_LB_SINGLE_VERSION,
			"",
			0,
			&REMINDER_FONT),
	STATIC_LB_L(X_LB_HELP, Y_LB_HELP, W_LB_HELP, H_LB_HELP,
			IDC_SETTING_LB_HELP,
			"",
			0,
			&REMINDER_FONT),
	STATIC_INFO(0,			Y_LOCATION_INFO(0),  IDC_SETTING_LB_OP1),
	STATIC_INFO(W_LB_INFO,	Y_LOCATION_INFO(0),  IDC_SETTING_LB_OP2),
	STATIC_INFO(0,			Y_LOCATION_INFO(1),  IDC_SETTING_LB_OP3),
	STATIC_INFO(W_LB_INFO,	Y_LOCATION_INFO(1),  IDC_SETTING_LB_OP4),
	STATIC_INFO(0,			Y_LOCATION_INFO(2),  IDC_SETTING_LB_OP5),
	STATIC_INFO(W_LB_INFO,	Y_LOCATION_INFO(2),  IDC_SETTING_LB_OP6),
	STATIC_INFO(0,			Y_LOCATION_INFO(3),  IDC_SETTING_LB_OP7),
	STATIC_INFO(W_LB_INFO,	Y_LOCATION_INFO(3),  IDC_SETTING_LB_OP8),
	STATIC_INFO(W_LB_INFO,	Y_LOCATION_INFO(4),  IDC_SETTING_LB_OP9),

};

static MY_DLGTEMPLATE DlgInitParam =
{
    WS_NONE,
//   WS_EX_AUTOSECONDARYDC,
    WS_EX_NONE,
    0,0,SCR_WIDTH,SCR_HEIGHT,
    "设置选择窗口",
    0, 0,       //menu and icon is null
    sizeof(ChildCtrls)/sizeof(MY_CTRLDATA),
    ChildCtrls, //pointer to control array
    0           //additional data,must be zero
};

static MYMENUITEM SettingFunc[]= {
	{UserSetting,			Word[WORD_SETTING_USER].string},
	{LocalSetting,			Word[WORD_SETTING_LOCAL].string},
	{SystemPara,			Word[WORD_SETTING_SYSTEM].string},
	{LocalPassword,			Word[WORD_SETTING_PASSWORD].string},
	{AccessPara,			Word[WORD_SETTING_ACCESS].string},
	{TimeSetting,			Word[WORD_TIME_SET].string},
	{ElevatorSetting,		Word[WORD_SETTING_ELEVATOR].string},
	{Reboot,				Word[WORD_SETTING_REBOOT].string},
	{SetDevType,			Word[WORD_SETTING_DEVTYPE].string},
};

static struct topmsg_st topmsg = {
	.title = Word[WORD_COMFIRM_AND_REBOOT].string,
	.notify_func =  Reboot_notify,
};

static int auto_close_time;
static int Isdisplay = 0;

/* ----------------------------------------------------------------*/
/**
 * @brief Reboot_notify 重启回调函数 
 *
 * @param hwnd
 * @param id
 * @param nc 1为重启
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void Reboot_notify(HWND hwnd, int id, int nc, DWORD add_data)
{
	if (nc) {
		SendMessage(Screen.hMainWnd,MSG_CLOSE,0,0);
	} else {
		HWND Form = Screen.Find("TFrmSetting");
		SendNotifyMessage(Form,MSG_SETTING_RESET_TIME,0,0);
	}
	
}

/* ----------------------------------------------------------------*/
/**
 * @brief InitDisplay 初始化显示界面
 *
 * @param hWnd
 */
/* ----------------------------------------------------------------*/
static void InitDisplay(HWND hWnd)
{
	char buf[64];
	int i;

	for (i = IDC_SETTING_LB_TITLE; i <= IDC_SETTING_LB_OP9; ++i) {
		if ((i >= IDC_SETTING_LB_OP1) && (i <= IDC_SETTING_LB_OP8)) {
			sprintf(buf,"%d.%s",i + 1 - IDC_SETTING_LB_OP1,SettingFunc[i - IDC_SETTING_LB_OP1].name);
			SetWindowText(GetDlgItem(hWnd,i),buf);
		}

		//判断是否显示手动设置设备类型
		if(!Public.IsDownLoadUseInfo && i == IDC_SETTING_LB_OP9){
			sprintf(buf,"%d.%s",i + 1 - IDC_SETTING_LB_OP1,SettingFunc[i - IDC_SETTING_LB_OP1].name);
			SetWindowText(GetDlgItem(hWnd,i),buf);
		}
		
		SetWindowElementAttr(GetDlgItem(hWnd,i), WE_FGC_WINDOW, MY_WHIGHT);
	}

	sprintf(buf,"%s: %d.%d.%d.%d(%s)\n",Word[WORD_SOFTWARE_VERSION].string,
		Public.SoftVer.Major,
		Public.SoftVer.Minor,
		Public.SoftVer.Release,
		Public.SoftVer.Reserve,
		PROTOCOL_MODE);
	SetWindowText(GetDlgItem(hWnd,IDC_SETTING_LB_SOFTWARE_VERSION),buf);

	// sprintf(buf,"%s:%s\n",Word[WORD_RELEASE_DATE].string,Public.BuilderDate);
	// SetWindowText(GetDlgItem(hWnd,IDC_SETTING_LB_DATE),buf);

	sprintf(buf,"%s: %d.%d.%d.%d\n",Word[WORD_SYSTEM_VERSION].string,
		Public.KernelVer.Major,
		Public.KernelVer.Minor,
		Public.KernelVer.Release,
		Public.KernelVer.Reserve);
	SetWindowText(GetDlgItem(hWnd,IDC_SETTING_LB_KERNEL_VERSION),buf);

	sprintf(buf,"%s: %d.%d.%d.%d\n",Word[WORD_SINGLE_VERSION].string,
		Public.m_MCUVer.Major,
		Public.m_MCUVer.Minor,
		Public.m_MCUVer.Release,
		Public.m_MCUVer.Reserve);
	SetWindowText(GetDlgItem(hWnd,IDC_SETTING_LB_SINGLE_VERSION),buf);

	sprintf(buf,"%s",Word[WORD_SETTING_RETURN].string);
	SetWindowText(GetDlgItem(hWnd,IDC_SETTING_LB_HELP),buf);
}

/* ----------------------------------------------------------------*/
/**
 * @brief UserSetting 用户设置
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void UserSetting(HWND hwnd, int id, int nc, DWORD add_data)
{
	CreateUserSettingForm(hwnd,Bmp_background);
}

/* ----------------------------------------------------------------*/
/**
 * @brief LocalSetting 本机设置
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void LocalSetting(HWND hwnd, int id, int nc, DWORD add_data)
{
	CreateNetSettingForm(hwnd,Bmp_background);
}

/* ----------------------------------------------------------------*/
/**
 * @brief SystemPara 系统参数
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void SystemPara(HWND hwnd, int id, int nc, DWORD add_data)
{
	CreateSysParaForm(hwnd,Bmp_background);
}

/* ----------------------------------------------------------------*/
/**
 * @brief LocalPassword 本机密码
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void LocalPassword(HWND hwnd, int id, int nc, DWORD add_data)
{
	CreateSetPasswordForm(hwnd,Bmp_background);
}

/* ----------------------------------------------------------------*/
/**
 * @brief AccessPara 门禁参数
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void AccessPara(HWND hwnd, int id, int nc, DWORD add_data)
{
	CreateSetAccessForm(hwnd,Bmp_background);
}

/* ----------------------------------------------------------------*/
/**
 * @brief TimeSetting 时间设置
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void TimeSetting(HWND hwnd, int id, int nc, DWORD add_data)
{
	CreateSetTimeForm(hwnd,Bmp_background);
}

/* ----------------------------------------------------------------*/
/**
 * @brief ElevatorSetting 电梯联动
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void ElevatorSetting(HWND hwnd, int id, int nc, DWORD add_data)
{
	CreateSetElevatorForm(hwnd,Bmp_background);
}

/* ----------------------------------------------------------------*/
/**
 * @brief Reboot 重新启动
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void Reboot(HWND hwnd, int id, int nc, DWORD add_data)
{
	CreateTopMessageForm(hwnd,Bmp_background,&topmsg); 
}

/* ----------------------------------------------------------------*/
/**
 * @brief SetDevType 设置机型
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void SetDevType(HWND hwnd, int id, int nc, DWORD add_data)
{
	CreateSetDevTypeForm(hwnd,Bmp_background); 
}


/* ----------------------------------------------------------------*/
/**
 * @brief DoKeyFunc 执行案件对应功能
 *
 * @param hWnd 窗口句柄
 * @param KeyVal 案件值
 */
/* ----------------------------------------------------------------*/
static void DoKeyFunc(HWND hWnd,int KeyVal)
{
	if ((KeyVal >= 1) && (KeyVal <= 8) ) {
		Isdisplay = 0;	
		SettingFunc[KeyVal - 1].notif_proc(hWnd,0,0,0);
	}

	if((!Public.IsDownLoadUseInfo) && (KeyVal == 9) ){
		Isdisplay = 0;	
		SettingFunc[KeyVal - 1].notif_proc(hWnd,0,0,0);
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
		case KEYOK:
			{
				SendNotifyMessage(hWnd,MSG_CLOSE,0,0);
			} break;

			//其他数字按键
		default:
			{
				DoKeyFunc(hWnd,KeyVal);
			} break;
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief SettingProc 窗口回调函数
 *
 * @param hWnd
 * @param message
 * @param wParam
 * @param lParam
 *
 * @return
 */
/* ----------------------------------------------------------------*/
static int SettingProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_INITDIALOG:
			{
				auto_close_time = FORM_SETTING_ONTIME;
				Isdisplay = 1;
				Screen.Add(hWnd,"TFrmSetting");
				Public.PlayVoice("SystemSetting");
				InitDisplay(hWnd);
				SetTimer(hWnd,IDC_SETTING_TIMER,TIME_1S);
			} break;

		case MSG_KEYBOARD :
			ProUserKeyBoard(hWnd,wParam);
			break;

		case MSG_SETTING_RESET_TIME :
			{
				Isdisplay = 1;
				auto_close_time = FORM_SETTING_ONTIME;
			} break;
		case MSG_MANUAL_SET_DEVECE_TYPE :
			{
				//判断是否显示手动设置设备类型
				char buf[128];
				if(!Public.IsDownLoadUseInfo){
					sprintf(buf,"%d.%s",IDC_SETTING_LB_OP9-IDC_SETTING_LB_HELP,SettingFunc[IDC_SETTING_LB_OP9-IDC_SETTING_LB_HELP-1].name);
					SetWindowText(GetDlgItem(hWnd,IDC_SETTING_LB_OP9),buf);
				}
			
			}break;
		case MSG_TIMER:
			{
				if (wParam == IDC_SETTING_TIMER){
					if ((auto_close_time > 0) && Isdisplay) {
						DBG_P("[%s]time:%d\n", __FILE__,auto_close_time);
						if (--auto_close_time == 0) {
							SendNotifyMessage(hWnd,MSG_CLOSE,0,0);
						}
					}
				}
			} break;


		case MSG_ERASEBKGND:
			if (Bmp_background != NULL)
				DrawBackground(hWnd, (HDC)wParam, (const RECT*)lParam,Bmp_background);
			return 0;

		case MSG_CLOSE:
			{
				HWND Form = Screen.Find("TFrmPassword");
				if (Form) {
					SendNotifyMessage(Form,MSG_CLOSE,0,0);
				}
				KillTimer(hWnd,IDC_SETTING_TIMER);
				DestroyMainWindow (hWnd);
				MainWindowThreadCleanup (hWnd);
			} return 0;

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
 * @brief CreateSettingForm 创建选择系统设置窗口
 *
 * @param hMainWnd
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
int CreateSettingForm(HWND hMainWnd,BITMAP *Bmp_bkg)
{
	HWND Form = Screen.Find("TFrmSetting");
	Bmp_background = Bmp_bkg;
	if(Form) {
		ShowWindow(Form,SW_SHOWNORMAL);
		return Form;
	} else {
		return CreateMyWindowIndirectParam(&DlgInitParam,hMainWnd,SettingProc,0);
	}
}

