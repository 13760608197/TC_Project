/*
 * =====================================================================================
 *
 *       Filename:  FormUserSearch.c
 *
 *    Description:  户主查询
 *
 *        Version:  1.0
 *        Created:  2017-08-07 17:15:15
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
extern PLOGFONT font24;
extern PLOGFONT font32;
extern PLOGFONT font18;

/* ----------------------------------------------------------------*
 *                  internal functions declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                        macro define
 *-----------------------------------------------------------------*/
#if DBG_PASSWORD > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

#define USERSEARCH_TITLE_FONT font32
#define USERSEARCH_HELP_FONT font18
#define USERSEARCH_TREEVIEW_FONT font18

#define W_TITLE SCR_WIDTH
#define H_TITLE 40
#define X_TITLE 0
#define Y_TITLE 5

#define W_TREEVIEW SCR_WIDTH
#define H_TREEVIEW 160
#define X_TREEVIEW 0
#define Y_TREEVIEW 40

#define W_HELP SCR_WIDTH
#define H_HELP 40
#define X_HELP 0
#define Y_HELP 210

static TVITEMINFO RootInfo;		//���ڸ��ڵ�������������

/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/
static BITMAP *Bmp_background = NULL; 
static int auto_close_time;

static int Enter_Allow_time;		//��ֹ��Ƶ���������ж���ɳ���crash
static int Enter_Allow_Flag = 1;

static MY_CTRLDATA ChildCtrls [] = {

	STATIC_LB(X_TITLE, Y_TITLE, W_TITLE, H_TITLE,
			IDC_USERSEARCH_TITLE,
			"",
			0,
			&USERSEARCH_TITLE_FONT),

	TREEVIEW(X_TREEVIEW,Y_TREEVIEW,W_TREEVIEW,H_TREEVIEW,
			IDC_USERSEARCH_TREEVIEW,
			"",
			(DWORD)&RootInfo,
			&USERSEARCH_TREEVIEW_FONT),

	STATIC_LB(X_HELP, Y_HELP, W_HELP, H_HELP,
			IDC_USERSEARCH_HELP,
			"",
			0,
			&USERSEARCH_HELP_FONT)
};

static MY_DLGTEMPLATE DlgInitParam =
{
    TVS_NOTIFY,
//   WS_EX_AUTOSECONDARYDC,
    WS_EX_NONE,
    0,0,SCR_WIDTH,SCR_HEIGHT,
    "??????",
    0, 0,       //menu and icon is null
    sizeof(ChildCtrls)/sizeof(MY_CTRLDATA),
    ChildCtrls, //pointer to control array
    0           //additional data,must be zero
};

/* ---------------------------------------------------------------------------*/
/**
 * @brief EnterCalling ȷ�Ϻ���
 */
/* ---------------------------------------------------------------------------*/
static void EnterCalling(HWND hwnd,char * buff)
{
	int len = strlen(Public.mCallIP);
	if(len) {
		if(strcmp(Public.mCallIP,Public.cLocalIP)==0) { 		//���б���IP������
			return ;
		}
		if(strcmp(Public.pCenterMsg->IP,Public.mCallIP)==0) {	//���й������ģ�����
			Public.PlayVoice("CallCenter");
		} else {
			Public.PlayVoice("CallUser");
		}
#if (defined TC3000_BZ)
		video->ST_machine->MsgPost(video->ST_machine,EVENT_SHAKEHANDS,NULL);
#endif
		video->ST_machine->MsgPost(video->ST_machine,EVENT_DIAL,NULL);

		form_main->clearDialNumber();
		form_main->setTextDial(buff);
		usleep(2000);
		SendNotifyMessage(hwnd,MSG_CLOSE,0,0);
	}
	
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief EnterHandsUp �һ�
 */
/* ---------------------------------------------------------------------------*/
static void EnterHandsUp(void)
{
	if(video->getStatus(video) != ST_IDLE) {
		video->ST_machine->MsgPost(video->ST_machine,EVENT_HANDS_UP,NULL);
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief btCancelClick ȷ��
 *
 * @param hWnd
 */
/* ----------------------------------------------------------------*/
static void btCancelClick(HWND hWnd)
{	
	SendNotifyMessage(hWnd,MSG_CLOSE,0,0);
}

/* ----------------------------------------------------------------*/
/**
 * @brief EnterCallingToHouseholder ���л���
 * @param buff
 *
 */
/* ----------------------------------------------------------------*/
static void EnterCallingToHouseholder(HWND hwnd,char * buff){
	//Enter_Allow_time = 3;		//3s���ٴκ�������Ӧ
	//Enter_Allow_Flag = 0;
	char Code[16] = {0};
	char SqlStr[512] = {0};
	char CallIP[16] = {0};
	int i;

	if(buff == NULL){
		return;
	}
/*	
	for(i=0;i<strlen(buff);i++){
		if('0' <= buff[i] && buff[i] <= '9'){
			Code[i] = buff[i];
		}else{
			break;
		}
	}
	DBG_P("Check Code Value : [%s] \n",Code);
*/	
	//��ѯ
	sprintf(SqlStr,"select IP from UserInfo where Roomname='%s'",buff);
	int ret = LocalQueryOpen(LocalQuery,SqlStr);	//��ѯ
	if(!ret){
		DBG_P("Can Not Find Homer \n");
		LocalQuery->Close(LocalQuery);
		return;
	}
	LocalQueryOfChar(LocalQuery,"IP",CallIP,sizeof(CallIP));
	DBG_P("Select IP By DataBase -------> : [%s] \n",CallIP);
	memset(Public.mCallIP,0,sizeof(Public.mCallIP));
	strncpy(Public.mCallIP,CallIP,sizeof(Public.mCallIP));
	if(video->getStatus(video) == ST_IDLE) {
		EnterCalling(hwnd,buff);
	} else {
		EnterHandsUp();
	}
	LocalQuery->Close(LocalQuery);

}

/* ----------------------------------------------------------------*/
/**
 * @brief ProUserKeyBoard ��������������
 *
 * @param hWnd	���ھ��
 * @param KeyVal ����ֵ
/* ----------------------------------------------------------------*/
static void ProUserKeyBoard(HWND hWnd,int KeyVal)
{
	auto_close_time = KEY_PRESS_TIME;
	HWND TV_hWnd = GetDlgItem(hWnd,IDC_USERSEARCH_TREEVIEW);
	//��ȡ��ǰ�ڵ�����
	GHANDLE ITEM;
	ITEM = SendMessage(TV_hWnd,TVM_GETSELITEM,0,0);
	DBG_P("TVM_GETSELITEM  -----------------> : [%d] \n",ITEM);
	switch(KeyVal)
	{
		//����
		case KEYBACK:
			{
				btCancelClick(hWnd);
			} break;
		//ȷ�Ϯ�
		case KEYOK:
			{
				GHANDLE RELITEM;
				TVITEMINFO * TVINFO;
				TVINFO = (TVITEMINFO *)malloc(sizeof(TVITEMINFO));
				SendMessage(TV_hWnd,TVM_GETITEMINFO,(WPARAM)ITEM,(LPARAM)TVINFO);
				DBG_P("TVM_GETITEMINFO --------------------------> [%s] \n",TVINFO->text);
				
				//�жϵ�ǰ�ڵ����Ƿ����ӽڵ�
				RELITEM = SendMessage(TV_hWnd,TVM_GETRELATEDITEM,TVIR_FIRSTCHILD,(LPARAM)ITEM);
				if(RELITEM == 0){
					//��ǰ�ڵ���û���ӽڵ㣬���к��д���
					int VideoState = video->getStatus(video);
					if(VideoState == ST_IDLE ){ //&& Enter_Allow_Flag){
						EnterCallingToHouseholder(hWnd,TVINFO->text);
					}
					free(TVINFO);
					break;
				}
				if(TVINFO->dwFlags == (TVIF_ROOT | TVIF_SELECTED) || TVINFO->dwFlags == TVIF_SELECTED){
					TVINFO->dwFlags = TVIF_FOLD;			//�رսڵ�
				}else if(TVINFO->dwFlags == (TVIF_FOLD | TVIF_SELECTED)){
					TVINFO->dwFlags = TVIF_SELECTED;	  	//�򿪽ڵ�
				}else{
					TVINFO->dwFlags = TVIF_SELECTED;		//�򿪽ڵ�
				}
				SendMessage(TV_hWnd,TVM_SETITEMINFO,(WPARAM)ITEM,(LPARAM)TVINFO);	
				free(TVINFO);
			} break;
		//�Ϸ�
		case 8:{
				GHANDLE RELITEM;
				//��ȡ��ǰ�ڵ������һ���ֵܽڵ�					
				RELITEM = SendMessage(TV_hWnd,TVM_GETRELATEDITEM,TVIR_PREVSIBLING,(LPARAM)ITEM);
				if(RELITEM == 0){	//û����һ���ֵܽڵ���,��ȡ���ĸ��ڵ�
					RELITEM = SendMessage(TV_hWnd,TVM_GETRELATEDITEM,TVIR_PARENT,(LPARAM)ITEM);
					if(RELITEM == 0){ //û�и��ڵ㣬��˽ڵ�Ϊ���ڵ���
						
						break;
					}
				}
				SendMessage(TV_hWnd,TVM_SETSELITEM,(WPARAM)RELITEM,0);
			}break;
		//�·�
		case 0:{
				GHANDLE RELITEM;
				//�жϵ�ǰ�ڵ����״̬�Ƿ�չ��
				TVITEMINFO * TV_INFO;			
				TV_INFO = (TVITEMINFO *)malloc(sizeof(TVITEMINFO));
				SendMessage(TV_hWnd,TVM_GETITEMINFO,(WPARAM)ITEM,(LPARAM)TV_INFO);
				if(TV_INFO->dwFlags == (TVIF_ROOT | TVIF_SELECTED) || TV_INFO->dwFlags == TVIF_SELECTED){
					//��չ������ȡ��ǰ�ڵ���ĵ�һ���ӽڵ���
					RELITEM = SendMessage(TV_hWnd,TVM_GETRELATEDITEM,TVIR_FIRSTCHILD,(LPARAM)ITEM);
				}else{
					//δչ������ȡ��һ���ֵܽڵ���
					RELITEM = SendMessage(TV_hWnd,TVM_GETRELATEDITEM,TVIR_NEXTSIBLING,(LPARAM)ITEM);
					if(RELITEM == 0){	//û����һ���ֵܽڵ㣬���ȡ��ǰ�ڵ���ĵ�һ���ӽڵ�
						RELITEM = SendMessage(TV_hWnd,TVM_GETRELATEDITEM,TVIR_FIRSTCHILD,(LPARAM)ITEM);
						if(RELITEM == 0){	//û����һ�ֵܽڵ㣬�򷵻ظ��ڵ㣬����ȡ���ڵ����һ���ֵܽڵ�
							RELITEM = SendMessage(TV_hWnd,TVM_GETRELATEDITEM,TVIR_PARENT,(LPARAM)ITEM);
							RELITEM = SendMessage(TV_hWnd,TVM_GETRELATEDITEM,TVIR_NEXTSIBLING,(LPARAM)RELITEM);
						}
					}
				}
				SendMessage(TV_hWnd,TVM_SETSELITEM,(WPARAM)RELITEM,0);
				free(TV_INFO);
			}break;
	}
}
/* ----------------------------------------------------------------*/
/**
 * @brief CreateTmpTable �����ݿ����½�һ����ʱ�����ڼӿ����ݲ�ѯ��	
 * @void					
 * return Ret   1 Ok  0 fail
 */
/* ----------------------------------------------------------------*/
static int CreateTmpTable(void){
	int ret = 0;
	char * KeyFrom1 = "A";
	char * KeyTo1 = "Z";
	char * KeyFrom2 = "a";
	char * KeyTo2 = "z";
	char SqlStr[512] = {0};
	sprintf(SqlStr,"create table TmpTable as " 
				   "select Roomname from UserInfo where Roomname >= '%s' and Roomname <= '%s'"
				   "union "
				   "select Roomname from UserInfo where Roomname >= '%s' and Roomname <= '%s'",
				   KeyFrom1,KeyTo1,KeyFrom2,KeyTo2);
	ret = LocalQueryExec(LocalQuery , SqlStr);

	return ret;
}
/* ----------------------------------------------------------------*/
/**
 * @brief CreateTmpTable_INDEX ������ʱ�����������ѯ��������	
 * @void					
 * return Ret   1 Ok  0 fail
 */
/* ----------------------------------------------------------------*/
static int CreateTmpTable_INDEX(void){
	int ret = 0;
	ret = LocalQueryExec(LocalQuery,
						"CREATE INDEX index_TmpTable on TmpTable (Roomname)");
	return ret;
}

/* ----------------------------------------------------------------*/
/**
 * @brief DeleteTmpTable ɾ����ʱ��	
 * @void					
 * return Ret   1 Ok  0 fail
 */
/* ----------------------------------------------------------------*/
static int DeleteTmpTable(void){
	int ret = 0;
	char * SqlStr = "DROP table TmpTable";
	ret = LocalQueryExec(LocalQuery , SqlStr);

	return ret;
}

/* ----------------------------------------------------------------*/
/**
 * @brief UpdateDataToTreeView ��ѯ���ݿ⣬��TREEVIEW�ؼ�ע������		
 * @param hWnd					
 *
 */
/* ----------------------------------------------------------------*/

static int UpdateDataToTreeView(HWND hWnd){
	GHANDLE ITEM;
	TVITEMINFO tvInfo;
	char SelData1[4] = {0};
	char SelData2[4] = {0};
	char SelData1_Compare[4] = {0};
	char SelData2_Compare[4] = {0};
	char Householder[16] = {0};
	int i,j;
	for(i=0;i<26;i++){
		char tmp = 'A' + i;
		sprintf(SelData1,"%c",tmp);
		sprintf(SelData1_Compare,"%c",tmp + 1);		//���ڽ�likeģ����ѯתΪ�ԱȲ�ѯ����ı���
		sprintf(SelData2,"%c",tmp + 32);	//��СдASCII���ʮ����32		
		sprintf(SelData2_Compare,"%c",tmp + 33); 	//���ڽ�likeģ����ѯתΪ�ԱȲ�ѯ����ı���

		tvInfo.text = SelData1;		
		tvInfo.dwFlags = TVIF_FOLD;
		ITEM = SendMessage(GetDlgItem(hWnd,IDC_USERSEARCH_TREEVIEW),TVM_ADDITEM,0,(LPARAM)&tvInfo);
		char SqlStr[512];
		//ģ����ѯ���ݻ�������
		sprintf(SqlStr,"select Roomname from TmpTable where Roomname >= '%s' and Roomname < '%s' "
						"union "
						"select Roomname from TmpTable where Roomname >= '%s' and Roomname < '%s' ",
						SelData1,SelData1_Compare,SelData2,SelData2_Compare);
		LocalQueryOpen(LocalQuery,SqlStr);	//��ѯ
		int Ret = LocalQuery->RecordCount(LocalQuery); //��ȡ��ѯ�������
		for(j=0;j<Ret;j++){
			LocalQueryOfChar(LocalQuery,"Roomname",Householder,sizeof(Householder));
			tvInfo.text = Householder;
			tvInfo.dwFlags = TVIF_FOLD;
			SendMessage(GetDlgItem(hWnd,IDC_USERSEARCH_TREEVIEW),TVM_ADDITEM,ITEM,(LPARAM)&tvInfo);
			LocalQuery->Next(LocalQuery);
		}
	}
	LocalQuery->Close(LocalQuery);
	return 0;
}
/* ----------------------------------------------------------------*/
/**
 * @brief SearchUserInfoProc ����ִ�к���
 * @param hWnd
 * @param message
 * @param wParam
 * @param lParam
 *
 * @return
 */
/* ----------------------------------------------------------------*/
static int SearchUserInfoProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case MSG_INITDIALOG:
			{	
				Screen.hAddressBook = hWnd;
				Screen.Add(hWnd,"TFrmSearchUserInfo");				
				auto_close_time = FORM_SETTING_ONTIME;
				
				//������ʾ
				SetWindowText(GetDlgItem(hWnd,IDC_USERSEARCH_TITLE),Word[WORD_USERSEARCH_TITLE].string);
				SetWindowElementAttr(GetDlgItem(hWnd,IDC_USERSEARCH_TITLE), WE_FGC_WINDOW, MY_BLACK);
				ShowWindow(GetDlgItem(hWnd,IDC_USERSEARCH_TITLE),SW_SHOWNORMAL);
				//������ʱ���ݱ�
				if(CreateTmpTable()){
					CreateTmpTable_INDEX();
					//��ؼ�ע������
					//���Ϳؼ���ʾ
					UpdateDataToTreeView(hWnd);
					ShowWindow(GetDlgItem(hWnd,IDC_USERSEARCH_TREEVIEW),SW_SHOWNORMAL);
				}
				
				//������Ϣ��ʾ
				SetWindowText(GetDlgItem(hWnd,IDC_USERSEARCH_HELP),Word[WORD_USERSEARCH_HELP].string);
				SetWindowElementAttr(GetDlgItem(hWnd,IDC_USERSEARCH_HELP), WE_FGC_WINDOW, MY_BLACK);
				ShowWindow(GetDlgItem(hWnd,IDC_USERSEARCH_HELP),SW_SHOWNORMAL);

				SetTimer(Screen.hAddressBook,IDC_USERSEARCH_TIMER,TIME_1S);
			}
			break;
		
		case MSG_KEYBOARD:
				ProUserKeyBoard(hWnd,wParam);
				break;
		
		case MSG_ERASEBKGND:
			if (Bmp_background != NULL)
				DrawBackground(hWnd, (HDC)wParam, (const RECT*)lParam,Bmp_background);
			return 0;
		
		case MSG_TIMER:
			{
				if (wParam == IDC_USERSEARCH_TIMER){
					if ((auto_close_time > 0)) {
						if (--auto_close_time == 0) {
							SendNotifyMessage(Screen.hAddressBook,MSG_CLOSE,0,0);
						}
					}
					/*
					if(Enter_Allow_time > 0){
						if (--Enter_Allow_time == 0) {
							Enter_Allow_Flag = 1;
						}
					}
					*/
				}
			} break;

		case MSG_CLOSE:
			//ɾ����ʱ��
			DeleteTmpTable();
			DestroyMainWindow (Screen.hAddressBook);
			MainWindowThreadCleanup (Screen.hAddressBook);
		
			form_main->resetKeyPressTime();		//���ڰ�����ʱ�����в�����*/#�ţ������ڶ�δ���*/#��ʱ��
												//���߳��а�����ʱ���Ѿ���Ч�������UIˢ���쳣���ڵ�ǰUI���˳�ʱ������ˢ�¼�ʱ��
			return 0;

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
 * @brief CreatePasswordForm ��������
 *
 * @param hMainWnd
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
int CreateUserSeachForm(HWND hMainWnd,BITMAP *Bmp_bkg)
{
	HWND Form = Screen.Find("TFrmSearchUserInfo");
	Bmp_background = Bmp_bkg;
	if(Form) {
		ShowWindow(Form,SW_SHOWNORMAL);
		return Form;
	} else {
		RootInfo.text = (char *)Word[WORD_CONTACT_LIST].string;		//�������Ϳؼ����ڵ�������
		return CreateMyWindowIndirectParam(&DlgInitParam,hMainWnd,SearchUserInfoProc,0);
	}
}

