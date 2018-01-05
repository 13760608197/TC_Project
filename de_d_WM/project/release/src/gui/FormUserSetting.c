/*
 * =====================================================================================
 *
 *       Filename:  FormUserSetting.c
 *
 *    Description:  用户设置窗口
 *
 *        Version:  1.0
 *        Created:  2015-11-02 16:30:21
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
#include <netinet/ip.h>

#include "externfunc.h"
/* ----------------------------------------------------------------*
 *                  extern variables declare
 *-----------------------------------------------------------------*/
extern PLOGFONT font14;
extern PLOGFONT font18;

/* ----------------------------------------------------------------*
 *                  internal functions declare
 *-----------------------------------------------------------------*/
static void Clear_notify(HWND hwnd, int id, int nc, DWORD add_data);
/* ----------------------------------------------------------------*
 *                        macro define
 *-----------------------------------------------------------------*/
#if DBG_SETTING_USER > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

#if (defined TC3000_DE) || (defined TC3000_X6) || (defined TC3000_DK)
#define TEXT_FONT font18
#define BUTTON_FONT font14
#define SCROLLVIEW_FONT font14

#define INTERVAL	5
#define FONT_HEIGHT 20
#define BUTTON_WIDTH 60
#define BUTTON_HEIGHT FONT_HEIGHT + INTERVAL

#define W_LB_ROOMNUM 125
#define H_LB_ROOMNUM FONT_HEIGHT
#define X_LB_ROOMNUM 5
#define Y_LB_ROOMNUM 10

#define W_EDIT_ROOMNUM	W_LB_ROOMNUM
#define H_EDIT_ROOMNUM	H_LB_ROOMNUM
#define X_EDIT_ROOMNUM	X_LB_ROOMNUM
#define Y_EDIT_ROOMNUM	Y_LB_ROOMNUM + H_LB_ROOMNUM + INTERVAL

#define W_LB_IP	W_LB_ROOMNUM
#define H_LB_IP	FONT_HEIGHT
#define X_LB_IP X_LB_ROOMNUM
#define Y_LB_IP	Y_EDIT_ROOMNUM + H_EDIT_ROOMNUM + INTERVAL

#define W_EDIT_IP	W_LB_IP
#define H_EDIT_IP	H_LB_IP
#define X_EDIT_IP	X_LB_IP
#define Y_EDIT_IP	Y_LB_IP + H_LB_IP  + INTERVAL

#define W_LB_LISTCOUNT 	SCR_WIDTH - (X_LB_ROOMNUM + W_LB_ROOMNUM + INTERVAL)
#define H_LB_LISTCOUNT 	FONT_HEIGHT
#define X_LB_LISTCOUNT	X_LB_ROOMNUM + W_LB_ROOMNUM + INTERVAL
#define Y_LB_LISTCOUNT	Y_LB_ROOMNUM

#define W_LIST_ROOMNUM	W_LB_LISTCOUNT
#define H_LIST_ROOMNUM	SCR_HEIGHT - FONT_HEIGHT * 4
#define X_LIST_ROOMNUM	X_LB_LISTCOUNT
#define Y_LIST_ROOMNUM 	Y_LB_LISTCOUNT + H_LB_LISTCOUNT + INTERVAL

#define W_LB_REMINDER	SCR_WIDTH
#define H_LB_REMINDER   FONT_HEIGHT
#define X_LB_REMINDER	X_LB_ROOMNUM
#define Y_LB_REMINDER	SCR_HEIGHT - FONT_HEIGHT

#define W_LB_REMINDER_BT	W_LIST_ROOMNUM
#define H_LB_REMINDER_BT   	FONT_HEIGHT
#define X_LB_REMINDER_BT	X_LB_LISTCOUNT
#define Y_LB_REMINDER_BT	Y_LIST_ROOMNUM + H_LIST_ROOMNUM + INTERVAL

#define W_BT_ADD	BUTTON_WIDTH
#define H_BT_ADD	BUTTON_HEIGHT
#define X_BT_ADD	6
#define Y_BT_ADD	Y_EDIT_IP + BUTTON_HEIGHT + 10

#define W_BT_DEL	BUTTON_WIDTH
#define H_BT_DEL	BUTTON_HEIGHT
#define X_BT_DEL	W_BT_ADD + 10
#define Y_BT_DEL	Y_BT_ADD

#define W_BT_CLEAR	BUTTON_WIDTH
#define H_BT_CLEAR	BUTTON_HEIGHT
#define X_BT_CLEAR	X_BT_ADD
#define Y_BT_CLEAR	Y_BT_DEL + BUTTON_HEIGHT + INTERVAL

#define W_BT_FIND	BUTTON_WIDTH
#define H_BT_FIND	BUTTON_HEIGHT
#define X_BT_FIND	X_BT_DEL
#define Y_BT_FIND	Y_BT_CLEAR

#define W_BT_BACK	BUTTON_WIDTH
#define H_BT_BACK	BUTTON_HEIGHT
#define X_BT_BACK	X_BT_ADD + 28
#define Y_BT_BACK	Y_BT_FIND + BUTTON_HEIGHT + INTERVAL
#endif
#if (defined TC3000_18S)
#define TEXT_FONT font18
#define BUTTON_FONT font14
#define SCROLLVIEW_FONT font14

#define INTERVAL	10
#define FONT_HEIGHT 20
#define BUTTON_WIDTH 60
#define BUTTON_HEIGHT FONT_HEIGHT + INTERVAL

#define W_LB_ROOMNUM 125
#define H_LB_ROOMNUM FONT_HEIGHT
#define X_LB_ROOMNUM 5
#define Y_LB_ROOMNUM 10

#define W_EDIT_ROOMNUM	W_LB_ROOMNUM
#define H_EDIT_ROOMNUM	H_LB_ROOMNUM
#define X_EDIT_ROOMNUM	X_LB_ROOMNUM
#define Y_EDIT_ROOMNUM	Y_LB_ROOMNUM + H_LB_ROOMNUM + INTERVAL

#define W_LB_IP	W_LB_ROOMNUM
#define H_LB_IP	FONT_HEIGHT
#define X_LB_IP X_LB_ROOMNUM
#define Y_LB_IP	Y_EDIT_ROOMNUM + H_EDIT_ROOMNUM + INTERVAL

#define W_EDIT_IP	W_LB_IP
#define H_EDIT_IP	H_LB_IP
#define X_EDIT_IP	X_LB_IP
#define Y_EDIT_IP	Y_LB_IP + H_LB_IP  + INTERVAL

#define W_LB_LISTCOUNT 	SCR_WIDTH - (X_LB_ROOMNUM + W_LB_ROOMNUM + INTERVAL)
#define H_LB_LISTCOUNT 	FONT_HEIGHT
#define X_LB_LISTCOUNT	X_LB_ROOMNUM + W_LB_ROOMNUM + INTERVAL
#define Y_LB_LISTCOUNT	Y_LB_ROOMNUM

#define W_LIST_ROOMNUM	W_LB_LISTCOUNT
#define H_LIST_ROOMNUM	SCR_HEIGHT - FONT_HEIGHT * 4
#define X_LIST_ROOMNUM	X_LB_LISTCOUNT
#define Y_LIST_ROOMNUM 	Y_LB_LISTCOUNT + H_LB_LISTCOUNT + INTERVAL

#define W_LB_REMINDER	SCR_WIDTH
#define H_LB_REMINDER   FONT_HEIGHT
#define X_LB_REMINDER	X_LB_ROOMNUM
#define Y_LB_REMINDER	SCR_HEIGHT - FONT_HEIGHT

#define W_LB_REMINDER_BT	W_LIST_ROOMNUM
#define H_LB_REMINDER_BT   	FONT_HEIGHT
#define X_LB_REMINDER_BT	X_LB_LISTCOUNT
#define Y_LB_REMINDER_BT	Y_LIST_ROOMNUM + H_LIST_ROOMNUM + INTERVAL

#define W_BT_ADD	BUTTON_WIDTH
#define H_BT_ADD	BUTTON_HEIGHT
#define X_BT_ADD	6
#define Y_BT_ADD	Y_EDIT_IP + BUTTON_HEIGHT + 10

#define W_BT_DEL	BUTTON_WIDTH
#define H_BT_DEL	BUTTON_HEIGHT
#define X_BT_DEL	W_BT_ADD + 10
#define Y_BT_DEL	Y_BT_ADD

#define W_BT_CLEAR	BUTTON_WIDTH
#define H_BT_CLEAR	BUTTON_HEIGHT
#define X_BT_CLEAR	X_BT_ADD
#define Y_BT_CLEAR	Y_BT_DEL + BUTTON_HEIGHT + INTERVAL

#define W_BT_FIND	BUTTON_WIDTH
#define H_BT_FIND	BUTTON_HEIGHT
#define X_BT_FIND	X_BT_DEL
#define Y_BT_FIND	Y_BT_CLEAR

#define W_BT_BACK	BUTTON_WIDTH
#define H_BT_BACK	BUTTON_HEIGHT
#define X_BT_BACK	X_BT_ADD + 28
#define Y_BT_BACK	Y_BT_FIND + BUTTON_HEIGHT + INTERVAL
#endif

/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
static BITMAP *Bmp_background = NULL;
static HWND hListBox;
static HWND hCtrl;
static HWND hDlg_ex;
static List *pList;
static ListData listData,*pData = NULL;
static int auto_close_time;
static int Isdisplay = 0;

static MY_CTRLDATA ChildCtrls [] = {
	STATIC_LB_L(X_LB_ROOMNUM, Y_LB_ROOMNUM, W_LB_ROOMNUM, H_LB_ROOMNUM,
			IDC_USERSET_LB_ROOMNUM,
			Word[WORD_ROOM_NUM].string,
			0,
			&TEXT_FONT),

	STATIC_LB_L(X_LB_IP, Y_LB_IP, W_LB_IP, H_LB_IP,
			IDC_USERSET_LB_IP,
			Word[WORD_IP].string,
			0,
			&TEXT_FONT),

	STATIC_LB_L(X_LB_LISTCOUNT, Y_LB_LISTCOUNT, W_LB_LISTCOUNT, H_LB_LISTCOUNT,
			IDC_USERSET_LB_LISTCOUNT,
			Word[WORD_RECORD_NUM].string,
			0,
			&TEXT_FONT),

	STATIC_LB(X_LB_REMINDER, Y_LB_REMINDER, W_LB_REMINDER, H_LB_REMINDER,
			IDC_USERSET_LB_REMINDER,
			Word[WORD_HELP_EDIT_IP].string,
			0,
			&TEXT_FONT),

	STATIC_LB(X_LB_REMINDER_BT, Y_LB_REMINDER_BT, W_LB_REMINDER_BT, H_LB_REMINDER_BT,
			IDC_USERSET_LB_REMINDER_BT,
			"",
			0,
			&TEXT_FONT),

	LISTBOX(X_LIST_ROOMNUM, Y_LIST_ROOMNUM, W_LIST_ROOMNUM, H_LIST_ROOMNUM,
			IDC_USERSET_LIST_ROOMNUM,
			"",
			0,
			&SCROLLVIEW_FONT),

	GROUP_EDIT_L(X_EDIT_ROOMNUM, Y_EDIT_ROOMNUM, W_EDIT_ROOMNUM, H_EDIT_ROOMNUM,
			IDC_USERSET_EDIT_ROOMNUM,
			"",
			0,
			0),

	EDIT_L(X_EDIT_IP, Y_EDIT_IP, W_EDIT_IP, H_EDIT_IP,
			IDC_USERSET_EDIT_IP,
			"",
			0,
			0),


	BUTTON_LB(X_BT_ADD,Y_BT_ADD, W_BT_ADD, H_BT_ADD,
			IDC_USERSET_BT_ADD,
			Word[WORD_ADD].string,
			0,
			&BUTTON_FONT),

	BUTTON_LB(X_BT_DEL, Y_BT_DEL, W_BT_DEL, H_BT_DEL,
			IDC_USERSET_BT_DEL,
			Word[WORD_DELETE].string,
			0,
			&BUTTON_FONT),

	BUTTON_LB(X_BT_CLEAR, Y_BT_CLEAR, W_BT_CLEAR, H_BT_CLEAR,
			IDC_USERSET_BT_CLEAR,
			Word[WORD_CLEAR_ALL].string,
			0,
			&BUTTON_FONT),

	BUTTON_LB(X_BT_FIND, Y_BT_FIND, W_BT_FIND, H_BT_FIND,
			IDC_USERSET_BT_FIND,
			Word[WORD_FIND].string,
			0,
			&BUTTON_FONT),

	BUTTON_LB(X_BT_BACK, Y_BT_BACK, W_BT_BACK, H_BT_BACK,
			IDC_USERSET_BT_BACK,
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
	.title = Word[WORD_CONFIRM_CLEAR_ALL_IP].string,
	.notify_func =  Clear_notify,
};
/* ----------------------------------------------------------------*/
/**
 * @brief IsButton 判断是否是按钮控件
 *
 * @param id 控件ID号
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
static BOOL IsButton(int id)
{
	if ((id >= IDC_USERSET_BT_ADD) && (id <= IDC_USERSET_BT_BACK)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief ListBox_add Scrollview添加元素
 *
 * @param data	数据内容
 */
/* ----------------------------------------------------------------*/
static void ListBox_add(ListData data)
{
	pList->ListAppend(pList,data);
	pData = (ListData *)pList->GetElemTail(pList);
	SendMessage (hListBox, LB_ADDSTRING, 0, (LPARAM)(pData->data));
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
	SendMessage(hwnd, MSG_USERSETTING_UPDATE_REMINDER_BT,
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
	SendMessage(hwnd, MSG_USERSETTING_UPDATE_REMINDER,
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
	//int count = SendMessage (hListBox, LB_GETCOUNT, 0, 0) ;
	LocalQueryOpen(LocalQuery,"Select Count(*) As cnt From UserInfo");
    int count = LocalQueryOfInt(LocalQuery,"cnt");
    LocalQuery->Close(LocalQuery);
	sprintf(string,"%s:%d",Word[WORD_RECORD_NUM].string,count - 1);
	SendMessage(hwnd, MSG_USERSETTING_UPDATE_REMINDER_LIST,
			0,
			(LPARAM)string);
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
			//TAB键
		case KEYBACK:
			{
				hCtrl = GetNextDlgTabItem (hDlg,hCtrl,FALSE);
				SendMessage(hDlg,MSG_KEYDOWN,SCANCODE_TAB,0);

				int id = GetDlgCtrlID(hCtrl);
				if (IsButton(id)) {
					if (ctrl_type == 0) {
						UpdateReminder(hDlg,Word[WORD_HELP_BUTTON].string);
					}
					ctrl_type = 1;
				} else {
					if (ctrl_type == 1) {
						UpdateReminder(hDlg,Word[WORD_HELP_EDIT_IP].string);
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
	char edit_room_text[50],edit_ip_text[50];
	char SqlStr[512];
	int count = 0;
	int room_len = SendMessage (GetDlgItem(GetParent(hwnd),IDC_USERSET_EDIT_ROOMNUM),
			MSG_GETTEXT, 50, (LPARAM) edit_room_text);
	int ip_len = SendMessage (GetDlgItem(GetParent(hwnd),IDC_USERSET_EDIT_IP),
			MSG_GETTEXT, 50,  (LPARAM)edit_ip_text);

	if (room_len == 0) { //检查房号是否为空
		UpdateBtReminder(GetParent(hwnd),Word[WORD_PLEASE_INSERT_PHONE_NUM].string);
	} else if (ip_len == 0){ // 检查IP是否为空
		UpdateBtReminder(GetParent(hwnd),Word[WORD_PLEASE_INSERT_IP].string);
	} else {
		struct in_addr addr;
		int ret = inet_pton(AF_INET, edit_ip_text, &addr);
		if (ret > 0) {
			sprintf(SqlStr,"Select Code From UserInfo Where Code='%s'",edit_room_text);
			LocalQueryOpen(LocalQuery,SqlStr);
			count = LocalQuery->RecordCount(LocalQuery);
			if (count) { // 检查是否存在该房号
				sprintf(SqlStr,"%s%s",Word[WORD_ROOM_NUM_SORT].string,Word[WORD_EXSIT].string);
				UpdateBtReminder(GetParent(hwnd),SqlStr);
				LocalQuery->Close(LocalQuery);
			} else {
				sprintf(SqlStr,"Select IP From UserInfo Where IP='%s'",edit_ip_text);
				LocalQueryOpen(LocalQuery,SqlStr);
				count = LocalQuery->RecordCount(LocalQuery);
				if (count) { // 检查是否存在该IP
					sprintf(SqlStr,"%s%s",Word[WORD_IP].string,Word[WORD_EXSIT].string);
					UpdateBtReminder(GetParent(hwnd),SqlStr);
					LocalQuery->Close(LocalQuery);
				} else {
					sprintf(listData.data,"%s %s",edit_room_text,edit_ip_text);
					ListBox_add(listData);
					sprintf(SqlStr,"Insert Into UserInfo(Code,IP) Values(\"%s\",\"%s\") ",
							edit_room_text,edit_ip_text);
					LocalQueryExec(LocalQuery,SqlStr);
					LocalQuery->Close(LocalQuery);
					WatchDogClose();
					LocalQueryExec(LocalQuery, "REINDEX index_user");
					WatchDogOpen();
					if (sqlCheck(&LocalQuery,"DataBase.db",1) == TRUE) {//检查据库
						backData("DataBase.db");
					}
					UpdateBtReminder(GetParent(hwnd),Word[WORD_ADD_SUCSEES].string);
					UpdateListReminder(GetParent(hwnd));
				}
			}
		} else {
			// 无效的IP地址
			sprintf(SqlStr,"IP%s",Word[WORD_WRONG_FORMATE].string);
			UpdateBtReminder(GetParent(hwnd),SqlStr);
		}
	}
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
static void btDelet(HWND hwnd, int id, int nc, DWORD add_data)
{
	char edit_room_text[50];
	char SqlStr[512];
	int room_len = SendMessage (GetDlgItem(GetParent(hwnd),
						IDC_USERSET_EDIT_ROOMNUM),
						MSG_GETTEXT,
						50,
						(LPARAM)edit_room_text);
	if (room_len) {
		int index = SendMessage (hListBox, LB_FINDSTRING, 0,(LPARAM)edit_room_text) ;
		if (index > 0) {
			if (strcmp(edit_room_text,"0000") == 0) {
				UpdateBtReminder(GetParent(hwnd),Word[WORD_CAN_NOT_DELETE_CENTER].string);
			} else {
				SendMessage (hListBox, LB_DELETESTRING, index, 0) ;
				sprintf(SqlStr,"Delete From UserInfo Where Code='%s'",edit_room_text);
				LocalQueryExec(LocalQuery,SqlStr);
				WatchDogClose();
				LocalQueryExec(LocalQuery, "REINDEX index_user");
				WatchDogOpen();
				if (sqlCheck(&LocalQuery,"DataBase.db",1) == TRUE) {//检查据库
					backData("DataBase.db");
				}
				sprintf(SqlStr,"%s%s",
						Word[WORD_DELETE].string,Word[WORD_SUCCESS].string);
				UpdateBtReminder(GetParent(hwnd),SqlStr);
				UpdateListReminder(GetParent(hwnd));
			}
		} else {
			sprintf(SqlStr,"%s%s%s",
					Word[WORD_DELETE].string,Word[WORD_ROOM_NUM_SORT].string,Word[WORD_NOT_EXSIT].string);
			UpdateBtReminder(GetParent(hwnd),SqlStr);
		}
	} else {
		UpdateBtReminder(GetParent(hwnd),Word[WORD_PLEASE_INSERT_PHONE_NUM].string);
	}
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
	char ip[16],code[12];
	char SqlStr[512];
	LocalQueryExec(LocalQuery,"Delete From UserInfo where Code<>\"0000\"");

	WatchDogClose();
	LocalQueryExec(LocalQuery, "REINDEX index_user");
	WatchDogOpen();
	if (sqlCheck(&LocalQuery,"DataBase.db",1) == TRUE) {//检查据库
		backData("DataBase.db");
	}
	SendMessage (hListBox, LB_RESETCONTENT, 0,0);
	pList->ClearList(pList);

	// 重新添加标题和管理中心IP地址
	LocalQueryOpen(LocalQuery,"Select Code,IP From UserInfo Where Code=\"0000\"");
	LocalQueryOfChar(LocalQuery,"Code",code,12);
	LocalQueryOfChar(LocalQuery,"IP",ip,16);
	LocalQuery->Close(LocalQuery);

	sprintf(listData.data,"%s %s",code,ip);
	ListBox_add(listData);

	sprintf(listData.data,"%s %s",Word[WORD_ROOM_NUM_SORT].string,Word[WORD_IP].string);
	ListBox_add(listData);

	sprintf(SqlStr,"%s%s",
			Word[WORD_CLEAR_ALL].string,Word[WORD_SUCCESS].string);
	UpdateBtReminder(hDlg_ex,SqlStr);
	UpdateListReminder(hDlg_ex);

	//设备被标识为未下载过房号的默认操作模式
	char buf[64] = {0};
	Public.IsDownLoadUseInfo = 0;
	sprintf(buf,"%d",Public.IsDownLoadUseInfo);
	SetValueToEtcFile_Ext("interface.ini","PUBLIC","IsDownLoadUseInfo",buf);

	//复位栋号,单元号
	Public.Build_Num = 1;
	Public.Unit_Num = 0;
	char Build_buf[4];
	sprintf(Build_buf,"%d",Public.Build_Num);
	char Unit_buf[4];
	sprintf(Unit_buf,"%d",Public.Unit_Num);
	SetValueToEtcFile_Ext("interface.ini","PUBLIC","Build_Num",Build_buf);
	SetValueToEtcFile_Ext("interface.ini","PUBLIC","Unit_Num",Unit_buf);
	
	//将本机IP设置为默认操作模式IP
	//设置本机IP
	memset(Public.cLocalIP,0,sizeof(Public.cLocalIP));
	memcpy(Public.cLocalIP,Public.DefaultDmkjIp,15);
	//设置子网掩码
	memset(Public.cNetMask,0,sizeof(Public.cNetMask));
	memcpy(Public.cNetMask,"255.255.0.0",strlen("255.255.0.0"));
	//设置网关地址
	memset(Public.cGateWay,0,sizeof(Public.cGateWay));
	memcpy(Public.cGateWay,"172.16.1.1",strlen("172.16.1.1"));
	//重置网卡
	SetNetwork(1,Public.cLocalIP,Public.cNetMask,Public.cGateWay,Public.MAC);
	
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
 * @brief btCheck 查询按钮
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data
 */
/* ----------------------------------------------------------------*/
static void btCheck(HWND hwnd, int id, int nc, DWORD add_data)
{
	char edit_room_text[50];
	char SqlStr[512];
	SendMessage (GetDlgItem(GetParent(hwnd),IDC_USERSET_EDIT_ROOMNUM), MSG_GETTEXT, 50, (LPARAM) edit_room_text);
	sprintf(SqlStr,"Select Code,IP From UserInfo Where Code='%s'",edit_room_text);
	LocalQueryOpen(LocalQuery,SqlStr);
	int count = LocalQuery->RecordCount(LocalQuery);
	if (count) {
		LocalQueryOfChar(LocalQuery,"IP",listData.data,16);
		SetWindowText(GetDlgItem(GetParent(hwnd),IDC_USERSET_EDIT_IP),listData.data);
	} else {
		sprintf(SqlStr,"%s%s",Word[WORD_ROOM_NUM_SORT].string,Word[WORD_NOT_EXSIT].string);
		UpdateBtReminder(GetParent(hwnd),SqlStr);
	}
	LocalQuery->Close(LocalQuery);
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
 * @brief InitListBoxCtrl 初始化ScrollView控件
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 */
/* ----------------------------------------------------------------*/
static void InitListBoxCtrl(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	char code[12],ip[16];
	hListBox = GetDlgItem(hDlg,IDC_USERSET_LIST_ROOMNUM);

	SendMessage (hListBox, LB_RESETCONTENT, 0, (LPARAM)0);
	pList = CreateList(sizeof(ListData));
	sprintf(listData.data,"%s %s",Word[WORD_ROOM_NUM_SORT].string,Word[WORD_IP].string);
	ListBox_add(listData);

	LocalQueryOpen(LocalQuery,"Select Code,IP,Roomname From UserInfo limit 0,20");
	int num = LocalQuery->RecordCount(LocalQuery);
	while(num--) {
		LocalQueryOfChar(LocalQuery,"Code",code,12);
		LocalQueryOfChar(LocalQuery,"IP",ip,16);
		sprintf(listData.data,"%s %s",code,ip);
		ListBox_add(listData);
		LocalQuery->Next(LocalQuery);
	}
	LocalQuery->Close(LocalQuery);
	UpdateListReminder(hDlg);
}

/* ----------------------------------------------------------------*/
/**
 * @brief UserSettingProc 窗口回调函数
 *
 * @param hWnd
 * @param message
 * @param wParam
 * @param lParam
 *
 * @return
 */
/* ----------------------------------------------------------------*/
static int UserSettingProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_INITDIALOG:
			{
				Screen.Add(hDlg,"TFrmUsr");
				hDlg_ex = hDlg;
				hCtrl = 0;
				Isdisplay = 1;
				auto_close_time = FORM_SETTING_ONTIME;
				int i;
				for (i=IDC_USERSET_LB_ROOMNUM;i<=IDC_USERSET_LB_LISTCOUNT;i++){
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW, MY_WHIGHT);
				}
				for (i=IDC_USERSET_EDIT_ROOMNUM;i<=IDC_USERSET_LIST_ROOMNUM;i++){
					SetWindowElementAttr(GetDlgItem(hDlg,i), WE_FGC_WINDOW, MY_BLACK);
				}
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_USERSET_LB_REMINDER), WE_FGC_WINDOW,MY_RED);
				SetWindowElementAttr(GetDlgItem(hDlg,IDC_USERSET_LB_REMINDER_BT), WE_FGC_WINDOW,MY_RED);

				InitListBoxCtrl(hDlg,message,wParam,lParam);
				SendDlgItemMessage(hDlg,IDC_USERSET_EDIT_ROOMNUM, EM_LIMITTEXT, 20, 0L);
				SendDlgItemMessage(hDlg,IDC_USERSET_EDIT_IP, EM_LIMITTEXT, 20, 0L);

				SetNotificationCallback (GetDlgItem(hDlg,IDC_USERSET_EDIT_IP), edit_notify);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_USERSET_EDIT_ROOMNUM), edit_notify);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_USERSET_BT_ADD), btAdd);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_USERSET_BT_DEL), btDelet);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_USERSET_BT_CLEAR), btClear);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_USERSET_BT_FIND), btCheck);
				SetNotificationCallback (GetDlgItem(hDlg,IDC_USERSET_BT_BACK), btBack);
				SetTimer(hDlg,IDC_USERSET_TIMER,TIME_1S);
			} break;

		case MSG_KEYBOARD :
			ProUserKeyBoard(hDlg,wParam);
			break;

		case MSG_USERSETTING_UPDATE_REMINDER :
			SetWindowText(GetDlgItem(hDlg,IDC_USERSET_LB_REMINDER),(char *)lParam);
			break;

		case MSG_USERSETTING_UPDATE_REMINDER_BT :
			SetWindowText(GetDlgItem(hDlg,IDC_USERSET_LB_REMINDER_BT),(char *)lParam);
			break;

		case MSG_USERSETTING_UPDATE_REMINDER_LIST :
			SetWindowText(GetDlgItem(hDlg,IDC_USERSET_LB_LISTCOUNT),(char *)lParam);
			break;

		case MSG_TIMER:
			{
				if (wParam == IDC_USERSET_TIMER){
						DBG_P("[%s]time:%d\n", __FILE__,auto_close_time);
					if ((auto_close_time > 0) && Isdisplay) {
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
				SendNotifyMessage(Form,MSG_MANUAL_SET_DEVECE_TYPE,0,0);
				KillTimer (hDlg,IDC_USERSET_TIMER);
				DestroyMainWindow (hDlg);
				MainWindowThreadCleanup (hDlg);
			} return 0;

		case MSG_DESTROY:
			Screen.Del(hDlg);
			if (pList) {
				pList->DestoryList(pList);
				pList = NULL;
			}
			DestroyAllControls (hDlg);
			return 0;

		default:
			break;
	}
    return DefaultDialogProc(hDlg, message, wParam, lParam);
}

/* ----------------------------------------------------------------*/
/**
 * @brief CreateUserSettingForm 创建用户设置窗口
 *
 * @param hMainWnd
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
int CreateUserSettingForm(HWND hMainWnd,BITMAP *Bmp_bkg)
{
	HWND Form = Screen.Find("TFrmUsr");
	Bmp_background = Bmp_bkg;
	if(Form) {
		ShowWindow(Form,SW_SHOWNORMAL);
		return Form;
	} else {
		return CreateMyWindowIndirectParam(&DlgInitParam,hMainWnd,UserSettingProc,0);
	}
}

