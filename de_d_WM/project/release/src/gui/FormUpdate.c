/*
 * =====================================================================================
 *
 *       Filename:  FormUpdate.c
 *
 *    Description:  升级程序界面
 *
 *        Version:  1.0
 *        Created:  2015-11-20 10:58:19
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
extern PLOGFONT font24;

/* ----------------------------------------------------------------*
 *                  internal functions declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                        macro define
 *-----------------------------------------------------------------*/
#if DBG_FORMUPDATE > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

#define W_WORD	SCR_WIDTH
#define H_WORD	50
#define X_WORD	0
#define Y_WORD	((SCR_HEIGHT - H_WORD) / 2)

#define WORD_FONT	font24
/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
static BITMAP *Bmp_background = NULL;
static int form_type = 0;
static int watchdog_time = 0;
static MY_CTRLDATA ChildCtrls [] = {
	STATIC_LB(X_WORD,Y_WORD,W_WORD,H_WORD,IDC_UPDATE_LB_REMINDER,	Word[WORD_UPDATING].string,	0,&WORD_FONT),
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

static void updateWatchdog(void)
{
	
}
/* ----------------------------------------------------------------*/
/**
 * @brief UpdateProc 窗口回调函数
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 *
 * @return
 */
/* ----------------------------------------------------------------*/
static int UpdateProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	TUpdateRevertFile *recdata;
	switch (message)
	{
		case MSG_INITDIALOG:
			{
				Screen.hUpdate = hDlg;
				Screen.Add(hDlg,"TFrmUpdate");
				screensaverStart(LCD_ON);
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_UPDATE_LB_REMINDER), WE_FGC_WINDOW,MY_WHIGHT);

				switch (form_type)
				{
					case UPDATE_TYPE_PROCESS:
						{
							SetWindowText(GetDlgItem(hDlg,IDC_UPDATE_LB_REMINDER),
									Word[WORD_UPDATING].string);
							recdata = (TUpdateRevertFile *)malloc(sizeof(TUpdateRevertFile));
							memcpy(recdata,(TUpdateRevertFile *)lParam,sizeof(TUpdateRevertFile));
							CreateUpdateThread((void *)recdata);
						} break;

					case UPDATE_TYPE_RECOVER:
						{
							SetWindowText(GetDlgItem(hDlg,IDC_UPDATE_LB_REMINDER),
									Word[WORD_RECOVER].string);
							SetTimer(hDlg,IDC_UPDATE_TIMER1,100);        //500ms
						} break;

					case UPDATE_TYPE_UNBROKEN:
						{
							SetWindowText(GetDlgItem(hDlg,IDC_UPDATE_LB_REMINDER),
									Word[WORD_UNBROKEN_ALARMING].string);
							// SetTimer(hDlg,IDC_UPDATE_TIMER1,100);        //500ms
						} break;

					case UPDATE_TYPE_IC_CARD:
						{
							SetWindowText(GetDlgItem(hDlg,IDC_UPDATE_LB_REMINDER),
									Word[WORD_UPDATE_CARD].string);
							SetTimer(hDlg,IDC_UPDATE_TIMER_WTD,TIME_1S );        //500ms
						} break;

					case UPDATE_TYPE_USER:
						{
							SetWindowText(GetDlgItem(hDlg,IDC_UPDATE_LB_REMINDER),
									Word[WORD_UPDATE_USER].string);
							SetTimer(hDlg,IDC_UPDATE_TIMER_WTD,TIME_1S );
						} break;
					
					default : break;
				}
			} break;

		case MSG_UPDATE_SUCCESS:
			SetWindowText (GetDlgItem(hDlg,IDC_UPDATE_LB_REMINDER),Word[WORD_REBOOT].string);
			SetTimer(hDlg,IDC_UPDATE_TIMER1,TIME_1S / 2);

			return 0;

		case MSG_UPDATE_FAIL:
			SetWindowText (GetDlgItem(hDlg,IDC_UPDATE_LB_REMINDER),Word[WORD_UPDATE_FAIL].string);
			SetTimer(hDlg,IDC_UPDATE_TIMER2,TIME_1S / 2);
			return 0;

		case MSG_UPDATE_IC_SUCCESS:
			SetWindowText (GetDlgItem(hDlg,IDC_UPDATE_LB_REMINDER),
									Word[WORD_UPDATE_CARD_SUCCESS].string);
			SetTimer(hDlg,IDC_UPDATE_TIMER2,TIME_1S);			
			return 0;

		case MSG_UPDATE_USER_SUCCESS:			
			SetWindowText (GetDlgItem(hDlg,IDC_UPDATE_LB_REMINDER),
									Word[WORD_UPDATE_USER_SUCCESS].string);
			SetTimer(hDlg,IDC_UPDATE_TIMER2,TIME_1S);


			return 0;

		case MSG_UPDATE_WTD:
			watchdog_time = 30;
			return 0;

		case MSG_TIMER:
			if(wParam == IDC_UPDATE_TIMER1) {
				WatchDogClose();
				KillTimer(hDlg,IDC_UPDATE_TIMER1);
				printf("exit the process!!\n");
				//exit(0);//直接退出程�?				SendMessage(Screen.hMainWnd,MSG_CLOSE,0,0);

			} else if(wParam == IDC_UPDATE_TIMER2) {
				SendMessage(hDlg,MSG_CLOSE,0,0);
			} else if(wParam == IDC_UPDATE_TIMER_WTD) {
				if (watchdog_time) {
					if (-- watchdog_time == 0) {
						if (form_type == UPDATE_TYPE_IC_CARD) {
							SetWindowText (GetDlgItem(hDlg,IDC_UPDATE_LB_REMINDER),
									Word[WORD_UPDATE_CARD_FAIL].string);
							SetTimer(hDlg,IDC_UPDATE_TIMER2,TIME_1S);
						} else if (form_type == UPDATE_TYPE_USER) {
							SetWindowText (GetDlgItem(hDlg,IDC_UPDATE_LB_REMINDER),
									Word[WORD_UPDATE_USER_FAIL].string);
							SetTimer(hDlg,IDC_UPDATE_TIMER2,TIME_1S);
						}
					}
				}
			} return 0;

		case MSG_ERASEBKGND:
			if (Bmp_background != NULL)
				DrawBackground(hDlg, (HDC)wParam, (const RECT*)lParam,Bmp_background);
			return 0;

		case MSG_CLOSE:
			KillTimer(hDlg,IDC_UPDATE_TIMER1);
			KillTimer(hDlg,IDC_UPDATE_TIMER2);
			KillTimer(hDlg,IDC_UPDATE_TIMER_WTD);
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
 * @brief CreateUpdateForm 创建升级程序窗口
 *
 * @param hMainWnd
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
int CreateUpdateForm(HWND hMainWnd,
		BITMAP *Bmp_bkg,
		void *arg,
		int type)
{
	HWND Form = Screen.Find("TFrmUpdate");
	form_type = type;
	Bmp_background = Bmp_bkg;
	if(Form) {
		ShowWindow(Form,SW_SHOWNORMAL);
		return Form;
	} else {
		return CreateMyWindowIndirectParam(&DlgInitParam,hMainWnd,UpdateProc,(LPARAM)arg);
	}
}

