/*
 * =====================================================================================
 *
 *       Filename:  FormMain.h
 *
 *    Description:  主窗口
 *
 *        Version:  1.0
 *        Created:  2016-02-23 15:34:38 
 *       Revision:  1.0
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _FORM_MAIN_H
#define _FORM_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MSG_MAIN_TIMER_START	    (MSG_USER + 1)
#define MSG_MAIN_TIMER_STOP	        (MSG_USER + 2)

	typedef enum _FormType {
		FORM_UNBROKEN,
		FORM_RECOVER,
		FORM_UPDATE,
		FORM_UNLOCK,
		FORM_SETTING,
		FORM_UPDATE_IC,
		FORM_UPDATE_USER,
		FORM_USERSEARCH,
	} FormType;

	typedef enum _KeyType{
		KEY_TYPE_STANDY, // 拨号键盘，底部为"CE"和"呼叫"
		KEY_TYPE_CALLING, // 通话键盘，底部为"留言"和"挂机"
	}KeyType;

	enum {
		IDC_TIMER_1S ,	// 1s定时器
		IDC_TIMER_60S,	// 60s定时器
		IDC_TIMER_CALL_DISP_200ms,	// 200ms定时器
		IDC_TIMER_UNLOCK,	// 开锁延时 500ms定时器
		IDC_TIMER_RECOVER,	// 数据恢复时显示窗口延时定时器
		IDC_TIMER_CALL_OUT, 			// 呼出超时定时器
		IDC_TIMER_SHAKEHANS_OUT, 	// 握手超时定时器
		IDC_TIMER_UNLOCK_FAIL_10S, 	// 门锁打开超时超时定时器
		IDC_TIMER_UPDATE_TIME_200MS, 	// 更新时间定时器
		IDC_TIMER_NUM,
	};

	typedef struct _FormMainTimers {
		void (*proc)(void);
		int time;
	}FormMainTimers;

	typedef struct _FormMain {
		int status;			
		FormMainTimers *timer;
		int (*loop)(void);

		int (*timerProc1s)(void);
		void (*timerStart)(int idc_timer);
		void (*timerStop)(int idc_timer);
		int (*timerGetState)(int idc_timer);
		void (*timerStopAllCalling)(void);

		void (*setNetWorkState)(int state);
		void (*setCenterOnlineState)(int state);

		void (*setTextOpration)(char *string);
		void (*setTextDial)(char *string);

		unsigned int (*getTextNumber)(char *text_num);

		void (*showCalling)(void);
		void (*clearDialNumber)(void);

		int (*updateClock)(void);
		void (*keyNotify)(int val);

		void (*listDeletAll)(void);
		void (*listAddItem)(int item);
		void (*listRefresh)(void);

		void (*resetKeyPressTime)(void);

		void (*setKeyboardType)(KeyType type);

		void (*createForm)(FormType type,void *para);
	} FormMain;

	extern PLOGFONT font14;
	extern PLOGFONT font18;
	extern PLOGFONT font24;

	extern BITMAP Bmp_bkg_setting_main; // 设置主界面背景
	extern BITMAP Bmp_bkg_setting_func; // 设置模式背景
	extern BITMAP Bmp_bkg_top_message; // 消息框背景
	extern BITMAP Bmp_icon_button_setting; // 设置界面按键图标
	extern BITMAP Bmp_icon_button_dial; // 设置界面数字按钮图标
	extern BITMAP Bmp_icon_button_func; // 设置界面功能按钮图标
	extern BITMAP Bmp_icon_button_back; // 设置界面返回按钮
	extern BITMAP Bmp_icon_button_check; // 选中
	extern BITMAP Bmp_icon_button_uncheck; // 非选中
	extern BITMAP Bmp_icon_button_plus; // +
	extern BITMAP Bmp_icon_button_sub; // -

	extern char font_type[10],font_family[10],font_charset[10];
	FormMain * formMainCreate(
			int (*AppProc)(HWND,int,WPARAM,LPARAM),
			void (*keyboardHandle)(unsigned int,int),
			FormMainTimers *);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
