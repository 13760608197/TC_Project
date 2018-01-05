/*
 * =====================================================================================
 *
 *       Filename:  FormSetAccess.c
 *
 *    Description:  门禁设置
 *
 *        Version:  1.0
 *        Created:  2015-11-09 09:51:22
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

#include <netinet/ip.h>

#include "externfunc.h"
/* ----------------------------------------------------------------*
 *                  extern variables declare
 *-----------------------------------------------------------------*/
extern PLOGFONT font14;

/* ----------------------------------------------------------------*
 *                  internal functions declare
 *-----------------------------------------------------------------*/
static void Clear_notify(HWND hwnd, int id, int nc, DWORD add_data);

/* ----------------------------------------------------------------*
 *                        macro define
 *-----------------------------------------------------------------*/
#if DBG_SET_ACCESS > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

#if (defined TC3000_DE) || (defined TC3000_X6) || (defined TC3000_DK)
#define TEXT_FONT font14
#define BUTTON_FONT font14
#define AREA_WIDTH SCR_WIDTH - 20
#define FONT_HEIGHT 20
#define INTERVAL	5

#define W_LB_NAME	SCR_WIDTH / 10 * 2 + 10
#define H_LB_NAME	FONT_HEIGHT
#define X_LB_NAME	0
#define Y_LB_NAME	5

#define W_EDIT	SCR_WIDTH / 10 * 2 + 10
#define H_EDIT	FONT_HEIGHT
#define X_EDIT	X_LB_NAME + W_LB_NAME
#define Y_EDIT	Y_LB_NAME + H_LB_NAME + INTERVAL

#define W_BUTTON	60
#define H_BUTTON	FONT_HEIGHT
#define X_BUTTON	SCR_WIDTH / 10 * 2

#define _Y_LOCATION(x) Y_LB_NAME + ((FONT_HEIGHT+6) * x)
#define _X_BUTTON_LOCATION(x) X_BUTTON + ((W_BUTTON+3) * x)
#endif
#if (defined TC3000_18S)
#define TEXT_FONT font14
#define BUTTON_FONT font14
#define AREA_WIDTH SCR_WIDTH - 20
#define FONT_HEIGHT 22
#define INTERVAL	6

#define W_LB_NAME	SCR_WIDTH / 10 * 2 + 10
#define H_LB_NAME	FONT_HEIGHT
#define X_LB_NAME	0
#define Y_LB_NAME	5

#define W_EDIT	SCR_WIDTH / 10 * 2 + 10
#define H_EDIT	FONT_HEIGHT
#define X_EDIT	X_LB_NAME + W_LB_NAME
#define Y_EDIT	Y_LB_NAME + H_LB_NAME + INTERVAL

#define W_BUTTON	80
#define H_BUTTON	FONT_HEIGHT
#define X_BUTTON	50 + SCR_WIDTH / 10 * 2

#define _Y_LOCATION(x) Y_LB_NAME + ((FONT_HEIGHT+6) * x)
#define _X_BUTTON_LOCATION(x) X_BUTTON + ((W_BUTTON+3) * x)
#endif

#define MSG_SETACCESS_UPDATE_REMINDER		 	(MSG_USER + 1)
#define MSG_SETACCESS_UPDATE_REMINDER_LIST		(MSG_USER + 2)
#define MSG_SETACCESS_UPDATE_CARD_NUM			(MSG_USER + 3)
/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
struct listview_caption  {
	int width;
	char *title;
};

// struct listview_card {
	// char data[4][16];	
// };

static BITMAP *Bmp_background = NULL;
static HWND hListView;
static GHANDLE hitem;
static HWND hCtrl;
static HWND hDlg_ex;
static int auto_close_time;
static int listviw_num = 0;
static int Isdisplay = 0;

static struct listview_caption caption[] = {
	{70,Word[WORD_ROOM_NUM_SORT].string},
	{100,Word[WORD_CARD_NUM].string},
	{100,Word[WORD_IP].string},
	{60,Word[WORD_CARD_TYPE].string},
};
static char card_data[4][20];	

static char *card_type[] = {
	Word[WORD_CARD_TYPE_OWNER].string,
	Word[WORD_CARD_TYPE_MANAGE].string,
	Word[WORD_CARD_TYPE_TEMP].string,
	Word[WORD_CARD_TYPE_INVALID].string,
};

static MY_CTRLDATA ChildCtrls [] = {
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(0), W_LB_NAME, H_LB_NAME,
			IDC_SETACCESS_LB_ROOM_NUM,
			Word[WORD_ROOM_NUM_SORT].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(SCR_WIDTH / 2, _Y_LOCATION(0), W_LB_NAME, H_LB_NAME,
			IDC_SETACCESS_LB_IP,
			Word[WORD_IP].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(1), W_LB_NAME, H_LB_NAME,
			IDC_SETACCESS_LB_CARD_NUM,
			Word[WORD_CARD_NUM].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(SCR_WIDTH / 2, _Y_LOCATION(1), W_LB_NAME, H_LB_NAME,
			IDC_SETACCESS_LB_CARD_TYPE,
			Word[WORD_CARD_TYPE].string,
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(2), SCR_WIDTH, H_LB_NAME,
			IDC_SETACCESS_LB_REMINDER,
			"",
			0,
			&TEXT_FONT),
	STATIC_LB_L(X_LB_NAME, _Y_LOCATION(4), SCR_WIDTH, H_LB_NAME,
			IDC_SETACCESS_LB_LIST,
			Word[WORD_RECORD_NUM].string,
			0,
			&TEXT_FONT),
	GROUP_EDIT_L(X_EDIT, _Y_LOCATION(0), W_EDIT, H_EDIT,
			IDC_SETACCESS_EDIT_ROOM_NUM,
			"",
			0,
			0),
	EDIT_L(X_EDIT + SCR_WIDTH / 2, _Y_LOCATION(0), W_EDIT, H_EDIT,
			IDC_SETACCESS_EDIT_IP,
			"",
			0,
			0),
	EDIT_L(X_EDIT, _Y_LOCATION(1), W_EDIT, H_EDIT,
			IDC_SETACCESS_EDIT_CARD_NUM,
			"",
			0,
			0),
	COMBOBOX(X_EDIT + SCR_WIDTH / 2, _Y_LOCATION(1), W_EDIT, H_EDIT,
			IDC_SETACCESS_MENU_CARD_TYPE,
			"",
			0,
			&BUTTON_FONT),
	BUTTON_LB(_X_BUTTON_LOCATION(0), _Y_LOCATION(3), W_BUTTON, H_BUTTON,
			IDC_SETACCESS_BT_ADD,
			Word[WORD_ADD].string,
			0,
			&BUTTON_FONT),
	BUTTON_LB(_X_BUTTON_LOCATION(1), _Y_LOCATION(3), W_BUTTON, H_BUTTON,
			IDC_SETACCESS_BT_DELETE,
			Word[WORD_DELETE].string,
			0,
			&BUTTON_FONT),
	BUTTON_LB(_X_BUTTON_LOCATION(2), _Y_LOCATION(3), W_BUTTON, H_BUTTON,
			IDC_SETACCESS_BT_CLEAR,
			Word[WORD_CLEAR_ALL].string,
			0,
			&BUTTON_FONT),
	BUTTON_LB(_X_BUTTON_LOCATION(3), _Y_LOCATION(3), W_BUTTON, H_BUTTON,
			IDC_SETACCESS_BT_BACK,
			Word[WORD_BACK].string,
			0,
			&BUTTON_FONT),
	LISTVIEW(X_LB_NAME, _Y_LOCATION(5), SCR_WIDTH, SCR_HEIGHT - _Y_LOCATION(5),
			IDC_SETACCESS_LISTVIEW,
			"",
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
	.title = Word[WORD_CONFIRM_CLEAR_ALL_CARD].string,
	.notify_func =  Clear_notify,
};

/* ----------------------------------------------------------------*/
/**
 * @brief JudgeCtrlType 判断控件类型
 *
 * @param id 控件ID号
 *
 * @returns
		CTRL_TYPE_SETACCESS_MENU,组合框
		CTRL_TYPE_SETACCESS_EDIT,编辑框
		CTRL_TYPE_SETACCESS_EDIT_IP,编辑框(只能输IP)
		CTRL_TYPE_SETACCESS_BUTTON,按钮
 */
/* ----------------------------------------------------------------*/
static int JudgeCtrlType(int id)
{
	if (id >= IDC_SETACCESS_BT_ADD) {
		return CTRL_TYPE_SETACCESS_BUTTON;
	} else if ( id >= IDC_SETACCESS_EDIT_IP){
		return CTRL_TYPE_SETACCESS_EDIT_IP;
	} else if ( id >= IDC_SETACCESS_EDIT_ROOM_NUM){
		return CTRL_TYPE_SETACCESS_EDIT;
	} else {
		return CTRL_TYPE_SETACCESS_MENU;
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief UpdateReminder 更新提示语
 *
 * @param hwnd
 * @param string
 */
/* ----------------------------------------------------------------*/
static void UpdateReminder(HWND hwnd,char *string)
{
	SendMessage(hwnd, MSG_SETACCESS_UPDATE_REMINDER,
			0,
			(LPARAM)string);
}

/* ----------------------------------------------------------------*/
/**
 * @brief UpdateListReminder 更新记录数提示数目
 */
/* ----------------------------------------------------------------*/
static void UpdateListReminder(HWND hwnd)
{
	char string[64];
	LocalQueryOpen(LocalQuery,"Select Count(*) As cnt From IDCard");
    int count = LocalQueryOfInt(LocalQuery,"cnt");
    LocalQuery->Close(LocalQuery);
	// int count = SendMessage (hListView,  LVM_GETITEMCOUNT, 0, 0) ;
	sprintf(string,"%s:%d",Word[WORD_RECORD_NUM].string,count);
	SendMessage(hwnd, MSG_SETACCESS_UPDATE_REMINDER_LIST,
			0,
			(LPARAM)string);
}

/* ----------------------------------------------------------------*/
/**
 * @brief add_item 增加行条目
 *
 * @param hlist 控件句柄
 * @param lvItem 数据
 * @param data 二维数组指针
 */
/* ----------------------------------------------------------------*/
static void add_item (HWND hlist, PLVITEM lvItem,char (*data)[20])
{
    LVSUBITEM subdata;
    int i;
	GHANDLE item = SendMessage (hlist, LVM_ADDITEM, 0, (LPARAM)lvItem);
    subdata.flags = 0;
    subdata.image = 0;
    subdata.nItem = lvItem->nItem;

	for (i = 0; i < 4; i ++) {
		subdata.subItem = i;
		subdata.pszText = *data++;
		subdata.nTextColor = PIXEL_black;
		SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief btAdd 添加按钮
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void btAdd(HWND hwnd, int id, int nc, DWORD add_data)
{
	LVITEM item;
	char edit_card_code[20],edit_ip[16],edit_room[20];
	char SqlStr[512];

	int len = SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETACCESS_EDIT_CARD_NUM),
			MSG_GETTEXT, 20, (LPARAM) edit_card_code);
	if (len == 0) {
		UpdateReminder(GetParent(hwnd),Word[WORD_PLEASE_INSERT_CARD_NUM].string);
		return;
	} else {
		sprintf(SqlStr,"Select CardCode From IDCard Where CardCode='%s'",edit_card_code);
		LocalQueryOpen(LocalQuery,SqlStr);
		len = LocalQuery->RecordCount(LocalQuery);
		if (len) {
			sprintf(SqlStr,"%s%s",Word[WORD_CARD_NUM].string,Word[WORD_EXSIT].string);
			UpdateReminder(GetParent(hwnd),SqlStr);
			LocalQuery->Close(LocalQuery);
			return;
		}
		LocalQuery->Close(LocalQuery);
	}

	SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETACCESS_EDIT_IP),
			MSG_GETTEXT, 20, (LPARAM) edit_ip);
	struct in_addr addr;
	len = inet_pton(AF_INET, edit_ip, &addr);
	if (len <= 0){
		// 无效的IP地址
		sprintf(SqlStr,"IP%s",Word[WORD_WRONG_FORMATE].string);
		UpdateReminder(GetParent(hwnd),SqlStr);
		return;
	}

	len = SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETACCESS_EDIT_ROOM_NUM),
			MSG_GETTEXT, 20, (LPARAM) edit_room);
	if (len == 0) {
		UpdateReminder(GetParent(hwnd),Word[WORD_PLEASE_INSERT_PHONE_NUM].string);
		return;
	}

	len = SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETACCESS_MENU_CARD_TYPE), CB_GETCURSEL, 0, 0);
	item.dwFlags = 0;
	item.nItemHeight = 25;
	item.nItem = listviw_num++;

	memset(card_data,0,sizeof(card_data));
	memcpy(card_data[0],edit_room,strlen(edit_room));
	memcpy(card_data[1],edit_card_code,strlen(edit_card_code));
	memcpy(card_data[2],edit_ip,strlen(edit_ip));
	memcpy(card_data[3],card_type[len],strlen(card_type[len]));
	// sprintf(SqlStr, "Insert Into IDCard(Code, CardCode, IP, Type) Values(\"%s\",\"%s\",\"%s\",%d)",
			// card_data[0], card_data[1], card_data[2], len + CARD_USER);

	sprintf(SqlStr,
			"Insert Into IDCard(Code, IP, CardCode, Type, HintAudio, MaxUseDate, UseState,LimitDate,UseDay,\
		LST1,LET1,LST2,LET2,LST3,LET3) Values(\"%s\",\"%s\",\"%s\",%d,\"%s\",\"%s\",%d,%d,%d,\
			%d,%d,%d,%d,%d,%d)",
			card_data[0],
			card_data[2],
			card_data[1],
			len + CARD_USER,
			"欢迎回家，开锁成功",
			"2100-12-31",
			1,
			0,
			127,
			0,
			86399,
			0,
			86399,
			0,
			86399);
	LocalQueryExec(LocalQuery,SqlStr);
	WatchDogClose();
	LocalQueryExec(LocalQuery, "REINDEX index_card");
	WatchDogOpen();
	if (sqlCheck(&LocalQuery,"DataBase.db",1) == TRUE) {//检查据库
	    backData("DataBase.db");
	}
	add_item (hListView, &item, card_data);
	sprintf(SqlStr,"%s%s",Word[WORD_SETTING].string,Word[WORD_SUCCESS].string);
	UpdateReminder(GetParent(hwnd),SqlStr);
	UpdateListReminder(GetParent(hwnd));
}

/* ----------------------------------------------------------------*/
/**
 * @brief btDelet 删除按钮
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static LVFINDINFO findInfo;
static void btDelet(HWND hwnd, int id, int nc, DWORD add_data)
{
	char *edit_code,*edit_room;
	char SqlStr[512];

	edit_code = (char *)malloc(sizeof(char)*20);
	edit_room = (char *)malloc(sizeof(char)*20);

	SendMessage (GetDlgItem(GetParent(hwnd),IDC_SETACCESS_EDIT_CARD_NUM),
			MSG_GETTEXT, 20, (LPARAM) edit_code);
	sprintf(SqlStr,"Select Code,CardCode From IDCard Where CardCode='%s'",edit_code);
	LocalQueryOpen(LocalQuery,SqlStr);
	int len = LocalQuery->RecordCount(LocalQuery);
	if (len) {
		LocalQueryOfChar(LocalQuery,"Code",edit_room,12);
		sprintf(SqlStr,"Delete From IDCard Where Code='%s'",edit_room);
		LocalQueryExec(LocalQuery,SqlStr);
		WatchDogClose();
		LocalQueryExec(LocalQuery, "REINDEX index_card");
		WatchDogOpen();
		if (sqlCheck(&LocalQuery,"DataBase.db",1) == TRUE) {//检查据库
			backData("DataBase.db");
		}
	} else {
		sprintf(SqlStr,"%s%s%s",
				Word[WORD_DELETE].string,Word[WORD_CARD_NUM].string,Word[WORD_NOT_EXSIT].string);
		UpdateReminder(GetParent(hwnd),SqlStr);
		free(edit_code);
		free(edit_room);
		return;
	}
	memset(&findInfo,0,sizeof(LVFINDINFO));
	HLVITEM hFound;
	findInfo.flags =  LVFF_TEXT;
	findInfo.iStart = 0;
	findInfo.nCols = 0;
	findInfo.pszInfo = &edit_room;	
	hFound = SendMessage (hListView, LVM_FINDITEM, 0, (LPARAM)&findInfo);
	if (hFound) {
		SendMessage (hListView, LVM_DELITEM, 0, hFound) ;
		sprintf(SqlStr,"%s%s",
				Word[WORD_DELETE].string,Word[WORD_SUCCESS].string);
		UpdateReminder(GetParent(hwnd),SqlStr);
	}
	free(edit_code);
	free(edit_room);
	UpdateListReminder(GetParent(hwnd));
}

/* ----------------------------------------------------------------*/
/**
 * @brief Clear_notify 清除数据回调函数
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void Clear_notify(HWND hwnd, int id, int nc, DWORD add_data)
{
	Isdisplay = 1;
	auto_close_time = FORM_SETTING_ONTIME;
	if (nc == FALSE) {
		return;
	}
	
	char SqlStr[512];
	WatchDogClose();
	LocalQueryExec(LocalQuery,"Delete From IDCard"); // 批量下载时不清除数据库 更改为命令方式删除所有卡记录
	LocalQueryExec(LocalQuery, "REINDEX index_card");
	WatchDogOpen();
	if (sqlCheck(&LocalQuery,"DataBase.db",1) == TRUE) {//检查据库
	    backData("DataBase.db");
	}
	SendMessage (hListView, LVM_DELALLITEM, 0, 0) ;
	sprintf(SqlStr,"%s%s",
			Word[WORD_CLEAR_ALL].string,Word[WORD_SUCCESS].string);
	UpdateReminder(hDlg_ex,SqlStr);
	UpdateListReminder(hDlg_ex);
}

/* ----------------------------------------------------------------*/
/**
 * @brief btClear 清空按钮
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void btClear(HWND hwnd, int id, int nc, DWORD add_data)
{
	Isdisplay = 0;	
	CreateTopMessageForm(GetParent(hwnd),Bmp_background,&topmsg);
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
		int ip_len = SendMessage (GetDlgItem(GetParent(hwnd),id), MSG_GETTEXT, 50,  (LPARAM)text);
		SendMessage (GetDlgItem(GetParent(hwnd),id), EM_SETCARETPOS, 0, ip_len);
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
					case CTRL_TYPE_SETACCESS_MENU:
						{
							if (ctrl_type != CTRL_TYPE_SETACCESS_MENU) {
								UpdateReminder(hDlg,Word[WORD_HELP_MENU].string);
							}
							ctrl_type = CTRL_TYPE_SETACCESS_MENU;
						} break;

					case CTRL_TYPE_SETACCESS_EDIT:
						{
							if (ctrl_type != CTRL_TYPE_SETACCESS_EDIT) {
								UpdateReminder(hDlg,Word[WORD_HELP_EDIT].string);
							}
							ctrl_type = CTRL_TYPE_SETACCESS_EDIT;
						} break;

					case CTRL_TYPE_SETACCESS_EDIT_IP:
						{
							if (ctrl_type != CTRL_TYPE_SETACCESS_EDIT_IP) {
								UpdateReminder(hDlg,Word[WORD_HELP_EDIT_IP].string);
							}
							ctrl_type = CTRL_TYPE_SETACCESS_EDIT_IP;
						} break;

					case CTRL_TYPE_SETACCESS_BUTTON:
						{
							if (ctrl_type != CTRL_TYPE_SETACCESS_BUTTON) {
								UpdateReminder(hDlg,Word[WORD_HELP_BUTTON].string);
							}
							ctrl_type = CTRL_TYPE_SETACCESS_BUTTON;
						} break;

					default:
						break;
				}
			} break;

		case KEYOK:
			{
				switch (JudgeCtrlType(GetDlgCtrlID(hCtrl)))
				{
					case CTRL_TYPE_SETACCESS_EDIT_IP:
					case CTRL_TYPE_SETACCESS_EDIT:
						{
							SendMessage(hCtrl,MSG_CHAR,'\b',0);
						} break;

					case CTRL_TYPE_SETACCESS_MENU:
						{
							SendMessage(hDlg,MSG_KEYDOWN,SCANCODE_ENTER,0);
						} break;

					case CTRL_TYPE_SETACCESS_BUTTON:
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
					case CTRL_TYPE_SETACCESS_EDIT_IP:
						{
							if (KeyVal == KEYLEAVEWORD) {
								SendMessage(hCtrl,MSG_CHAR,'.',0);
							} else {
								SendMessage(hCtrl,MSG_CHAR,'0' + KeyVal,0);
							}
						} break;

					case CTRL_TYPE_SETACCESS_EDIT:
						{
							if (KeyVal != KEYLEAVEWORD) {
								SendMessage(hCtrl,MSG_CHAR,'0' + KeyVal,0);
							}
						} break;

					case CTRL_TYPE_SETACCESS_MENU:
						{
							if (KeyVal == 0) {
								SendMessage(hDlg,MSG_KEYDOWN,SCANCODE_CURSORBLOCKDOWN,0);
							} else if (KeyVal == 8) {
								SendMessage(hDlg,MSG_KEYDOWN,SCANCODE_CURSORBLOCKUP,0);
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
 * @brief InitAccess 初始化编辑框，列表框等控件
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 */
/* ----------------------------------------------------------------*/
static void InitAccess(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	unsigned int i;
	LVCOLUMN lvcol;
	LVITEM item;
	hListView = GetDlgItem(hDlg,IDC_SETACCESS_LISTVIEW);
	for (i = 0; i < NELEMENTS(card_type); ++i) {
		SendDlgItemMessage(hDlg, IDC_SETACCESS_MENU_CARD_TYPE, CB_ADDSTRING, 0,
				(LPARAM)card_type[i]);
	}
	SendDlgItemMessage(hDlg, IDC_SETACCESS_MENU_CARD_TYPE, CB_SETCURSEL, 0, 0);

	for(i = 0; i < NELEMENTS(caption); i++) {
		lvcol.nCols = i;
		lvcol.pszHeadText = caption[i].title;
		lvcol.width = caption[i].width;
		lvcol.pfnCompare = NULL;
		lvcol.colFlags = 0;
		SendMessage (hListView, LVM_ADDCOLUMN, 0, (LPARAM) &lvcol);
	}
	listviw_num = 0;

		LocalQueryOpen(LocalQuery,"Select Code,IP,CardCode,Type From IDCard limit 0,10");
	unsigned int num = LocalQuery->RecordCount(LocalQuery);
	SendMessage (hListView, MSG_FREEZECTRL, TRUE, 0);
	item.dwFlags = 0;
	item.nItemHeight = 25;

	memset(card_data,0,sizeof(card_data));
	for (i = 0;i < num; i++) {
		item.nItem = i;
		LocalQueryOfChar(LocalQuery,"Code",card_data[0],12);
		LocalQueryOfChar(LocalQuery,"CardCode",card_data[1],12);
		LocalQueryOfChar(LocalQuery,"IP",card_data[2],20);
		int tmp = LocalQueryOfInt(LocalQuery,"Type");
		memcpy(card_data[3],card_type[tmp - CARD_USER],strlen(card_type[tmp]));
		add_item (hListView, &item, card_data);
		LocalQuery->Next(LocalQuery);
		listviw_num++;
	}
	LocalQuery->Close(LocalQuery);
	SendMessage (hListView, MSG_FREEZECTRL, FALSE, 0);
	UpdateListReminder(hDlg);
}
/* ----------------------------------------------------------------*/
/**
 * @brief SetAccessProc 窗口回调函数
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 *
 * @return
 */
/* ----------------------------------------------------------------*/
static int SetAccessProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_INITDIALOG:
			{
				Screen.Add(hDlg,"TFrmSetAc");
				hDlg_ex = hDlg;
				hCtrl = 0;
				Isdisplay = 1;
				auto_close_time = FORM_SETTING_ONTIME;
				int i;
				for (i=IDC_SETACCESS_LB_ROOM_NUM;i<=IDC_SETACCESS_LB_LIST; i++){
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW,MY_WHIGHT);
				}
				for (i=IDC_SETACCESS_EDIT_ROOM_NUM;i<=IDC_SETACCESS_EDIT_CARD_NUM; i++){
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW,MY_BLACK);
					SendDlgItemMessage(hDlg,i, EM_LIMITTEXT, 20, 0L);
					SetNotificationCallback (GetDlgItem(hDlg,i), edit_notify);
				}
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_SETACCESS_LB_REMINDER),
						WE_FGC_WINDOW,MY_RED);

				InitAccess(hDlg, message, wParam, lParam);
				UpdateReminder(hDlg,Word[WORD_HELP_BUTTON].string);

				SetNotificationCallback (GetDlgItem(hDlg,IDC_SETACCESS_BT_ADD), btAdd);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_SETACCESS_BT_DELETE), btDelet);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_SETACCESS_BT_CLEAR), btClear);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_SETACCESS_BT_BACK), btBack);
				SetTimer(hDlg,IDC_SETACCESS_TIMER,TIME_1S);
			} break;

		case MSG_SETACCESS_UPDATE_REMINDER :
			SetWindowText(GetDlgItem(hDlg,IDC_SETACCESS_LB_REMINDER),(char *)lParam);
			break;

		case MSG_SETACCESS_UPDATE_REMINDER_LIST :
			SetWindowText(GetDlgItem(hDlg,IDC_SETACCESS_LB_LIST),(char *)lParam);
			break;

		case MSG_SETACCESS_UPDATE_CARD_NUM :
			SetWindowText(GetDlgItem(hDlg,IDC_SETACCESS_EDIT_CARD_NUM),(char *)lParam);
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
				if (wParam == IDC_SETACCESS_TIMER){
					if ((auto_close_time > 0) && Isdisplay) {
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
				KillTimer (hDlg,IDC_SETACCESS_TIMER);
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
 * @brief CreateSetAccessForm 创建门禁设置窗口
 *
 * @param hMainWnd
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
int CreateSetAccessForm(HWND hMainWnd,BITMAP *Bmp_bkg)
{
	HWND Form = Screen.Find("TFrmSetAc");
	Bmp_background = Bmp_bkg;
	if(Form) {
		ShowWindow(Form,SW_SHOWNORMAL);
		return Form;
	} else {
		return CreateMyWindowIndirectParam(&DlgInitParam,hMainWnd,SetAccessProc,0);
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief updateFormSetAccessCardNum 更新卡号
 *
 * @param hDlg
 * @param card_num 卡号
 */
/* ----------------------------------------------------------------*/
void updateFormSetAccessCardNum(HWND hDlg, char *card_num)
{
	SendMessage(hDlg, MSG_SETACCESS_UPDATE_CARD_NUM, 0, (LPARAM)card_num);
}
