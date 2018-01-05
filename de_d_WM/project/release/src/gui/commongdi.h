/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  公共自定义画图函数
 *
 *        Version:  1.0
 *        Created:  2015-11-11 11:50:08 
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _MY_COMMON_H
#define _MY_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


	void SetAlarmForm(HWND hWnd);
	BOOL ClearAlarmForm(HWND hWnd);
	void DrawBackground(HWND hWnd, HDC hdc, const RECT* pClipRect,BITMAP *Image);
	void DrawWhiteFrame(HWND hWnd, HDC hdc, const RECT* pClipRect,int Left,int Top,int Width,int Height);
	void wndEraseBackground(HWND hWnd,HDC hdc, const RECT* pClipRect,BITMAP *pImage,int Left,int Top,int Width,int Height);
	void LoadAdverImage(HWND hWnd,int ID,BITMAP *Image,char *AdverType);
	void DrawXpFrame(HWND hWnd,HDC hdc, const RECT* pClipRect,const char *Caption,
			int left,int top,int right,int bottom);
	void GetPartFromBmp(const BITMAP *bmp,BITMAP *DstBitmap,int Left,int Top,int Width,int Height);
	void myFillBox(HDC hdc, RECT rc, int color);
	void TranslateIconPart(HDC hdc,int x,int y,int w,int h,BITMAP *FgBmp,int LineIconCnt,int IconIdx,int t,
			BOOL Translate);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
