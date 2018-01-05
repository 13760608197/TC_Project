/*
 * =====================================================================================
 *
 *       Filename:  Mylistbox.c
 *
 *    Description:  description
 *
 *        Version:  1.0
 *        Created:  2015-10-30 09:12:20
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
/* ----------------------------------------------------------------*
 * 						include head files
 *-----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"

#include "predefine.h"
#include "externfunc.h"
#include "Mylistbox.h"

/* ----------------------------------------------------------------*
 * 					extern variables declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 * 					internal functions declare
 *-----------------------------------------------------------------*/
static int MyListboxCtrlProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam);

/* ----------------------------------------------------------------*
 * 						  macro define
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 * 						variables define
 *-----------------------------------------------------------------*/




/* ----------------------------------------------------------------*/
/**
 * @brief RegisteMyListboxControl 注册控件
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
BOOL RegisteMyListboxControl (void)
{
	WNDCLASS WndClass;
	WndClass.spClassName = CTRL_MYLISTBOX;
	WndClass.dwStyle     = WS_NONE;
	WndClass.dwExStyle   = WS_EX_NONE;
	WndClass.hCursor     = GetSystemCursor (0);
	WndClass.iBkColor    = 0;
	WndClass.WinProc     = MyListboxCtrlProc;
	return RegisterWindowClass (&WndClass);
}

/* ----------------------------------------------------------------*/
/**
 * @brief MyListboxControlCleanup 取消注册
 */
/* ----------------------------------------------------------------*/
void MyListboxControlCleanup (void)
{
	UnregisterWindowClass(CTRL_MYLISTBOX);
}

/* ----------------------------------------------------------------*/
/**
 * @brief PaintMenuTitle 绘制菜单标题
 *
 * @param hwnd
 * @param hdc
 * @param pInfo
 */
/* ----------------------------------------------------------------*/
static void PaintMenuTitle(HWND hwnd, HDC hdc, MYLISTBOXDATA *pInfo)
{
	RECT rc;
	int i;

	GetClientRect (hwnd, &rc);

	SetTextColor(hdc,MY_BLACK);
	SetBkMode(hdc,BM_TRANSPARENT);
	SelectFont (hdc, pInfo->title.font);

	FillBoxWithBitmap(hdc,
			rc.left,
			rc.top,
			pInfo->itemWidth,
			pInfo->itemHeight,
			pInfo->title.bmp);
	SetRect(&rc,
			rc.left,
			rc.top,
			rc.right,
			pInfo->itemHeight);

	DrawText (hdc,
			pInfo->title.text,
			-1,
			&rc,
			DT_CENTER | DT_WORDBREAK | DT_VCENTER | DT_SINGLELINE);


	for (i=0; i<pInfo->page_max; i++) {
		if (i == pInfo->title.ball.index) {
			FillBoxWithBitmap(hdc,
					pInfo->title.ball.x + pInfo->title.ball.index_old * pInfo->title.ball.interval,
					pInfo->title.ball.y,
					pInfo->title.ball.w,
					pInfo->title.ball.h,
					pInfo->title.ball.bmp_inlight);
			pInfo->title.ball.index_old = pInfo->title.ball.index;
			FillBoxWithBitmap(hdc,
					pInfo->title.ball.x + i * pInfo->title.ball.interval,
					pInfo->title.ball.y,
					pInfo->title.ball.w,
					pInfo->title.ball.h,
					pInfo->title.ball.bmp_light);
		} else {
			FillBoxWithBitmap(hdc,
					pInfo->title.ball.x + i * pInfo->title.ball.interval,
					pInfo->title.ball.y,
					pInfo->title.ball.w,
					pInfo->title.ball.h,
					pInfo->title.ball.bmp_inlight);
		}
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief IsBottom 判断是否是底部按钮
 *
 * @param pInfo
 * @param index    当前选择位置
 * @param Islight  是否是选择状态 LIGHT为选择 INLIGHT为非选择
 *
 * @return
 */
/* ----------------------------------------------------------------*/
static  BITMAP *IsBottom(MYLISTBOXDATA *pInfo,int index,int Islight)
{
	if (index == (pInfo->itemCount - 1)) {
		if (Islight == LIGHT) {
			return pInfo->item.bmp_b_light;
		} else {
			return pInfo->item.bmp_b_inlight;
		}
	} else {
		if (Islight == LIGHT) {
			return pInfo->item.bmp_light;
		} else {
			return pInfo->item.bmp_inlight;
		}
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief PaintMenuItem 绘制菜单
 *
 * @param hwnd
 * @param hdc
 * @param pInfo
 */
/* ----------------------------------------------------------------*/
static void PaintMenuItem(HWND hwnd, HDC hdc, MYLISTBOXDATA *pInfo)
{
	RECT rc;
	int i;
	char *item_name,*item_name_old;
	MYMENUITEM *menudata;
	menudata = pInfo->menulist;

	GetClientRect (hwnd, &rc);

	SetTextColor(hdc,COLOR_lightwhite);
	SetBkMode(hdc,BM_TRANSPARENT);
	SelectFont (hdc, pInfo->item.font);

	for (i=0; i<pInfo->itemCount; i++) {
		if (i + pInfo->title.ball.index * pInfo->itemCount < pInfo->listcnt) {
			item_name = (menudata + i + pInfo->title.ball.index * pInfo->itemCount)->name;
		} else {
			item_name = "";
		}

		if (pInfo->item.Hook.index_old + pInfo->title.ball.index * pInfo->itemCount < pInfo->listcnt) {
			item_name_old = (menudata + pInfo->item.Hook.index_old + pInfo->title.ball.index * pInfo->itemCount)->name;
		} else {
			item_name_old = "";
		}

		if (i == pInfo->item.Hook.index) {

			FillBoxWithBitmap(hdc,
					0,
					(pInfo->item.Hook.index_old + 1) * pInfo->itemHeight,
					pInfo->itemWidth,
					pInfo->itemHeight,
					IsBottom(pInfo, pInfo->item.Hook.index_old, INLIGHT));
			SetRect(&rc,
					0,
					(pInfo->item.Hook.index_old + 1) * pInfo->itemHeight,
					pInfo->itemWidth,
					(pInfo->item.Hook.index_old + 1) * pInfo->itemHeight + pInfo->itemHeight);
			DrawText (hdc,
					item_name_old,
					-1,
					&rc,
					DT_NOCLIP | DT_CENTER | DT_WORDBREAK | DT_VCENTER | DT_SINGLELINE);

			FillBoxWithBitmap(hdc,
					0,
					(i + 1) * pInfo->itemHeight,
					pInfo->itemWidth,
					pInfo->itemHeight,
					IsBottom(pInfo, i, LIGHT));

			FillBoxWithBitmap(hdc,
					pInfo->item.Hook.x,
					(i + 1) * pInfo->itemHeight + pInfo->item.Hook.y,
					pInfo->item.Hook.w,
					pInfo->item.Hook.h,
					pInfo->item.Hook.bmp_light);
			SetRect(&rc,
					0,
					(i + 1) * pInfo->itemHeight,
					pInfo->itemWidth,
					(i + 1) * pInfo->itemHeight + pInfo->itemHeight);
			DrawText (hdc,
					item_name,
					-1,
					&rc,
					DT_CENTER | DT_WORDBREAK | DT_VCENTER | DT_SINGLELINE);

			pInfo->item.Hook.index_old = pInfo->item.Hook.index;
		} else {
			FillBoxWithBitmap(hdc,
					0,
					(i + 1) * pInfo->itemHeight,
					pInfo->itemWidth,
					pInfo->itemHeight,
					IsBottom(pInfo,i,INLIGHT));
			SetRect(&rc,
					0,
					(i + 1) * pInfo->itemHeight,
					pInfo->itemWidth,
					(i + 1) * pInfo->itemHeight + pInfo->itemHeight);
			DrawText (hdc,
					item_name,
					-1,
					&rc,
					DT_CENTER | DT_WORDBREAK | DT_VCENTER | DT_SINGLELINE);
		}
	}

}

/* ----------------------------------------------------------------*/
/**
 * @brief update_ball 更新标题页码指示小球的状态
 *
 * @param hwnd
 * @param pInfo
 */
/* ----------------------------------------------------------------*/
static void update_ball(HWND hwnd,MYLISTBOXDATA *pInfo)
{
	int x;
	RECT rc;
	pInfo->title.ball.index = pInfo->selItem / pInfo->itemCount;

	if (pInfo->title.ball.index != pInfo->title.ball.index_old) {
		x = pInfo->title.ball.x + pInfo->title.ball.index_old * pInfo->title.ball.interval;
		SetRect(&rc,
				x,
				pInfo->title.ball.y,
				x + pInfo->title.ball.w,
				pInfo->title.ball.y + pInfo->title.ball.h);
		InvalidateRect(hwnd,&rc,FALSE);

		x = pInfo->title.ball.x + pInfo->title.ball.index * pInfo->title.ball.interval;
		SetRect(&rc,
				x,
				pInfo->title.ball.y,
				x + pInfo->title.ball.w,
				pInfo->title.ball.y + pInfo->title.ball.h);
		InvalidateRect(hwnd,&rc,FALSE);

		SetRect(&rc,
				0,
				pInfo->itemHeight,
				pInfo->itemWidth,
				pInfo->itemHeight * (pInfo->itemCount + 1));
		InvalidateRect(hwnd,&rc,TRUE);
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief update_hook 更新绿色小勾的位置
 *
 * @param hwnd
 * @param pInfo
 */
/* ----------------------------------------------------------------*/
static void update_hook(HWND hwnd,MYLISTBOXDATA *pInfo)
{
	int y;
	RECT rc;
	pInfo->item.Hook.index = pInfo->selItem % pInfo->itemCount;

	if (pInfo->item.Hook.index != pInfo->item.Hook.index_old) {
		y = (pInfo->item.Hook.index_old + 1) * pInfo->itemHeight;
		SetRect(&rc,
				0,
				y,
				pInfo->itemWidth,
				y + pInfo->itemHeight);
		InvalidateRect(hwnd,&rc,TRUE);

		y = (pInfo->item.Hook.index + 1) * pInfo->itemHeight;
		SetRect(&rc,
				0,
				y,
				pInfo->itemWidth,
				y + pInfo->itemHeight);
		InvalidateRect(hwnd,&rc,TRUE);
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief PaintCallListItem 绘制呼叫列表
 *
 * @param hwnd
 * @param hdc
 * @param pInfo
 */
/* ----------------------------------------------------------------*/
static void PaintCallListItem(HWND hwnd, HDC hdc, MYLISTBOXDATA *pInfo)
{
	RECT rc;
	int i,color;
	SetTextColor(hdc,MY_BLACK);
	SetBkMode (hdc, BM_TRANSPARENT);
	SelectFont (hdc, pInfo->call_list_item.font);
	
	for (i = 0; i < pInfo->selItem ; i++) {
		SetRect(&rc,
				0,
				pInfo->itemHeight * i,
				pInfo->itemWidth,
				pInfo->itemHeight * (i + 1));
		if (i % 2) {
			color = COLOR_darkgray;	
		} else {
			color = COLOR_lightwhite;	
		}
		myFillBox(hdc,rc,color);
		DrawText (hdc,
				pInfo->call_list_item.list[i],
				-1,
				&rc,
				DT_NOCLIP | DT_LEFT | DT_WORDBREAK | DT_VCENTER | DT_SINGLELINE);
	}
}
/* ----------------------------------------------------------------*/
/**
 * @brief PaintLixtBox 绘图函数，根据type判断绘制类型
 *
 * @param hwnd
 * @param hdc
 * @param pInfo
 */
/* ----------------------------------------------------------------*/
static void PaintLixtBox(HWND hwnd, HDC hdc, MYLISTBOXDATA *pInfo)
{
	if (pInfo->type == TYPE_MENU) {
		PaintMenuTitle(hwnd,hdc,pInfo);
		PaintMenuItem(hwnd,hdc,pInfo);
	} else if (pInfo->type == TYPE_CALL_LIST) {
		PaintCallListItem(hwnd,hdc,pInfo);
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief MyListboxCtrlProc 菜单控件主回调函数
 *
 * @param hwnd
 * @param message
 * @param wParam
 * @param lParam
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
static int MyListboxCtrlProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
	PCONTROL        pCtrl;
	MYLISTBOXDATA   *pInfo;
	DWORD           dwStyle;

	pCtrl   = Control(hwnd);
	dwStyle = GetWindowStyle (hwnd);
	pInfo = (MYLISTBOXDATA*)pCtrl->dwAddData2;
	switch (message)
	{
		case MSG_CREATE:
			{
				MYLISTBOXDATA* data = (MYLISTBOXDATA*)pCtrl->dwAddData;
				pInfo = (MYLISTBOXDATA*) calloc (1, sizeof (MYLISTBOXDATA));
				if (pInfo == NULL)
					return -1;
				memcpy(pInfo,data,sizeof(MYLISTBOXDATA));
				pCtrl->dwAddData2 = (DWORD)pInfo;
			} break;

		case MSG_DESTROY:
			{
				if (pInfo) {
					free(pInfo);
				}
			} break;

		case MSG_UP :
			{
				if (pInfo->type == TYPE_MENU) {
					if (pInfo->selItem > 0) {
						pInfo->selItem--;
						update_ball(hwnd,pInfo);
						update_hook(hwnd,pInfo);
					}
				}
			}
			break;

		case MSG_DOWN :
			{
				if (pInfo->type == TYPE_MENU) {
					if (pInfo->selItem < (pInfo->listcnt - 1)) {
						pInfo->selItem++;
						update_ball(hwnd,pInfo);
						update_hook(hwnd,pInfo);
					}
				}
			}
			break;

		case MSG_SELECT:
			{
				if (pInfo->type == TYPE_MENU) {
					if ((pInfo->menulist + pInfo->selItem)->notif_proc)
						(pInfo->menulist + pInfo->selItem)->notif_proc(hwnd,0,0,0);
				}
			}
			break;

		case MSG_ADD_ITEM:
			{
				if(pInfo->selItem < pInfo->itemCount) {
					pInfo->selItem++; 
					strcpy(pInfo->call_list_item.list[pInfo->selItem-1],(char *)wParam);
				}
			} break;

		case MSG_DELETE_ALL:
			{
				pInfo->selItem = 0; 
				int i;
				for (i = 0; i < pInfo->itemCount; ++i) {
					memset(pInfo->call_list_item.list[i],0,sizeof(char)*64);
				}
			} break;

		case MSG_RERESH:
			{
				InvalidateRect(hwnd,NULL,TRUE);
			} break;

		case MSG_PAINT:
			{
				HDC	hdc;
				hdc = BeginPaint (hwnd);
				PaintLixtBox(hwnd,hdc,pInfo);
				EndPaint (hwnd, hdc);
			}
			return 0;

		default:
			break;
	}

	return DefaultControlProc (hwnd, message, wParam, lParam);

}

/* ----------------------------------------------------------------*/
/**
 * @brief MenuInit 初始化菜单类型的数据
 *
 * @param pInfo		菜单数据类型
 * @param pData		传入数据结构
 * @param MaxItemNum 菜单每页最多显示行数
 */
/* ----------------------------------------------------------------*/
static void MenuInit(MYLISTBOXDATA *pInfo,MY_MENUTEMPLATE *pData,int MaxItemNum)
{
	pInfo->itemCount = MaxItemNum;
	pInfo->itemWidth = pData->w;
	pInfo->itemHeight = pData->h;
	pInfo->listcnt = pData->controlnr;
	pInfo->menulist	= pData->menulist;
	pInfo->selItem = 0;
	if (pData->controlnr % MaxItemNum) {
		pInfo->page_max	= pData->controlnr / MaxItemNum + 1;
	} else {
		pInfo->page_max	= pData->controlnr / MaxItemNum;
	}

	pInfo->title.text = pData->caption;
	pInfo->title.font = *(pData->fonts->title);
	pInfo->title.bmp = pData->bmps->title_background;
	pInfo->title.ball.bmp_light = pData->bmps->title_ball_light;
	pInfo->title.ball.bmp_inlight = pData->bmps->title_ball_deep;

	pInfo->item.font = *(pData->fonts->text);
	pInfo->item.bmp_light = pData->bmps->menu_deep;
	pInfo->item.bmp_inlight = pData->bmps->menu_light;
	pInfo->item.bmp_b_light = pData->bmps->menu_bottom_deep;
	pInfo->item.bmp_b_inlight = pData->bmps->menu_bottom_light;
	pInfo->item.Hook.bmp_light = pData->bmps->menu_select;

	int widget_width,start;
	widget_width = pInfo->itemWidth / 4;
	start = pInfo->itemWidth / 4 * 3;

	pInfo->title.ball.w = pInfo->title.ball.bmp_light->bmWidth;
	pInfo->title.ball.h = pInfo->title.ball.bmp_light->bmHeight;
	pInfo->title.ball.interval = (widget_width - (pInfo->page_max * pInfo->title.ball.w)) / (pInfo->page_max + 1) + pInfo->title.ball.w;
	pInfo->title.ball.x =  start + pInfo->title.ball.interval - pInfo->title.ball.w;
	pInfo->title.ball.y	= (pInfo->itemHeight - pInfo->title.ball.h) / 2;

	pInfo->title.ball.index = pInfo->title.ball.index_old = 0;

	pInfo->item.Hook.w = pInfo->item.Hook.bmp_light->bmWidth;
	pInfo->item.Hook.h = pInfo->item.Hook.bmp_light->bmHeight;
	pInfo->item.Hook.x = start + (widget_width - pInfo->item.Hook.w) / 2;
	pInfo->item.Hook.y = (pInfo->itemHeight - pInfo->item.Hook.h) / 2;
	pInfo->item.Hook.index = pInfo->item.Hook.index_old = 0;
}

/* ----------------------------------------------------------------*/
/**
 * @brief CallListInit 初始化呼叫列表类型数据
 *
 * @param pInfo		呼叫列表数据类型
 * @param pData		传入数据结构
 * @param MaxItemNum 呼叫列表每页最多显示行数
 */
/* ----------------------------------------------------------------*/
static void CallListInit(MYLISTBOXDATA *pInfo,MY_CALL_LIST_TEMPLATE *pData,int MaxItemNum)
{
	pInfo->itemCount = MaxItemNum;
	pInfo->itemWidth = pData->w;
	pInfo->itemHeight = pData->h;
	pInfo->listcnt = MaxItemNum;
	pInfo->page_max	= 1;
	pInfo->selItem = 0; 

	memset(&pInfo->call_list_item,0,sizeof(MyCallListItem));
	pInfo->call_list_item.font = *(pData->fonts->text);
}

/* ----------------------------------------------------------------*/
/**
 * @brief CreateMyListBox 创建菜单列表控件
 *
 * @param Data 		数据结构
 * @param hOwner    父窗口句柄
 * @param type      列表类型  TYPE_MENU:菜单 TYPE_CALL_LIST:呼叫列表
 * @param MaxItemNum 窗口每页最大显示行数
 */
/* ----------------------------------------------------------------*/
void CreateMyListBox(void *Data,HWND hOwner,ENUM_LISTBOX_TYPE type,int MaxItemNum)
{
	MYLISTBOXDATA pInfo;
	pInfo.type = type;

	if (type == TYPE_MENU) {
		MY_MENUTEMPLATE *pData = (MY_MENUTEMPLATE *)Data;
		MenuInit(&pInfo,(MY_MENUTEMPLATE *)pData,MaxItemNum);
		CreateWindowEx(CTRL_MYLISTBOX,pData->caption,pData->dwStyle,pData->dwExStyle,
				pData->id,pData->x,pData->y, pData->w, pData->h * (MaxItemNum + 1), hOwner,(DWORD)&pInfo);
	} else if (type == TYPE_CALL_LIST){
		MY_CALL_LIST_TEMPLATE *pData = (MY_CALL_LIST_TEMPLATE *)Data;
		CallListInit(&pInfo,(MY_CALL_LIST_TEMPLATE *)pData,MaxItemNum);
		CreateWindowEx(CTRL_MYLISTBOX,"",pData->dwStyle,pData->dwExStyle,
				pData->id,pData->x,pData->y, pData->w, pData->h * MaxItemNum, hOwner,(DWORD)&pInfo);
	}
}

