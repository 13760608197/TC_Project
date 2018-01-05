/*
 * =============================================================================
 *
 *       Filename:  FormMain.c
 *
 *    Description:  ������
 *
 *        Version:  1.0
 *        Created:  2016-02-23 15:32:24
 *       Revision:  1.0
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =============================================================================
 */
/* ---------------------------------------------------------------------------*
 *                      include head files
 *----------------------------------------------------------------------------*/

#include "externfunc.h"
#include "FormMain.h"
/* ---------------------------------------------------------------------------*
 *                  extern variables declare
 *----------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*
 *                  internal functions declare
 *----------------------------------------------------------------------------*/
static void formMainAddTextNumber(int num);
static void formMainShowNumber(int type);
static int formMainDeleteTextNumber(void);
static void formMainClearDialNumber(void);
static void formMainShowStandby(void);

static void formMainListDeletAll(void);
static void formMainListRefresh(void);
static void formMainSetKeyboardType(KeyType type);
static void formMainResetKeyPressTime(void);

/* ---------------------------------------------------------------------------*
 *                        macro define
 *----------------------------------------------------------------------------*/
#if DBG_FORM_MAIN > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

static int CurrentStatus = 1;	// 1 Ϊ����״̬  0 Ϊ��������״̬

enum {
		IDC_LB_TIME = 20,
		IDC_LB_NETWORK,
		IDC_LB_CENTER,

		IDC_LB_HELP_OP,
		IDC_LB_HELP_CALL,
		IDC_LB_HELP_CLEAR,
		IDC_LB_HELP_CENTER,
		IDC_LB_HELP_MAILLIST,

		IDC_LB_DIAL_NUMBER,
		IDC_LB_DIAL_CENTER,
		IDC_LB_DIAL_CALL,
		IDC_MAIN_MYLISTBOX,
};

#if (defined TC3000_DE) || (defined TC3000_X6) || (defined TC3000_DK)
//��ʾ����--------------------------------------
#define W_REMINDER_AREA SCR_WIDTH
#define H_REMINDER_AREA 32
#define X_REMINDER_AREA 0
#define Y_REMINDER_AREA 0
//����ʱ��ؼ�
#define W_TIME W_REMINDER_AREA / 4 * 3
#define H_TIME 18
#define X_TIME X_REMINDER_AREA + 10
#define Y_TIME (H_REMINDER_AREA - H_TIME ) / 2
//���ӹ�������ͼ��
#define W_CENTER 30
#define H_CENTER 24
#define X_CENTER X_TIME + W_TIME
#define Y_CENTER (H_REMINDER_AREA - H_CENTER ) / 2
//��������ͼ��
#define W_NETWORK W_CENTER
#define H_NETWORK H_CENTER
#define X_NETWORK X_CENTER + W_CENTER + 5
#define Y_NETWORK Y_CENTER

//��������--------------------------------------
#define W_OP_AREA SCR_WIDTH
#define H_OP_AREA SCR_HEIGHT - H_REMINDER_AREA
#define X_OP_AREA 0
#define Y_OP_AREA H_REMINDER_AREA + 1

// ��ʾ�������� (������)
#define LINE_NUM 3
// 120Ϊ�����ܸ߶� ÿ�и�40
#define _WORD_INTERVAL_Y(Hight)        \
	((120 - (LINE_NUM * Hight)) / (LINE_NUM + 1) )
#define _Y_LOCATION( AIndex,Hight) \
	(Y_OP_AREA + 58 + (_WORD_INTERVAL_Y(Hight) * (AIndex + 1)) + (Hight* (AIndex)))
//����˵��
#define W_WORD_OP W_OP_AREA
#define H_WORD_OP 40
#define X_WORD_OP 0
#define Y_WORD_OP Y_OP_AREA + (60 - H_WORD_OP) /2
//����ס�������뷿�ţ���#��
#define W_WORD_CALL W_OP_AREA
#define H_WORD_CALL H_WORD_OP
#define X_WORD_CALL 0
#define Y_WORD_CALL _Y_LOCATION(0,H_WORD_OP)
//��    ����������󣬰�*��
#define W_WORD_CLEAR W_OP_AREA
#define H_WORD_CLEAR H_WORD_OP
#define X_WORD_CLEAR 0
#define Y_WORD_CLEAR _Y_LOCATION(1,H_WORD_OP)
//�������ģ�����0000����#��
#define W_WORD_CENTER W_OP_AREA
#define H_WORD_CENTER H_WORD_OP
#define X_WORD_CENTER 0
#define Y_WORD_CENTER _Y_LOCATION(2,H_WORD_OP)

//��������--------------------------------------
// ��ʾ�������� (������)
#define DIAL_LINE_NUM 2 // 120Ϊ2���ܸ߶� ÿ�и�60
#define _DIAL_WORD_INTERVAL_Y(Hight)        \
	((120 - (DIAL_LINE_NUM * Hight)) / (DIAL_LINE_NUM + 1) )
#define _Y_DIAL_LOCATION( AIndex,Hight) \
	(Y_OP_AREA + 90 + (_DIAL_WORD_INTERVAL_Y(Hight) * (AIndex + 1)) + (Hight* (AIndex)))
//����
#define W_DIAL_WORD_NUMBER W_OP_AREA
#define H_DIAL_WORD_NUMBER 60
#define X_DIAL_WORD_NUMBER 0
#define Y_DIAL_WORD_NUMBER Y_OP_AREA + (60 - H_WORD_OP) /2
//�������ģ�����0000����#��
#define W_DIAL_WORD_CENTER W_OP_AREA
#define H_DIAL_WORD_CENTER H_WORD_OP
#define X_DIAL_WORD_CENTER 0
#define Y_DIAL_WORD_CENTER _Y_DIAL_LOCATION(0,H_WORD_OP)
//����ס�������뷿�ţ���#��
#define W_DIAL_WORD_CALL W_OP_AREA
#define H_DIAL_WORD_CALL H_WORD_OP
#define X_DIAL_WORD_CALL 0
#define Y_DIAL_WORD_CALL _Y_DIAL_LOCATION(1,H_WORD_OP)
/*
//������ѯ������8888����#��
#define W_DIAL_WORD_MAILLIST W_OP_AREA
#define H_DIAL_WORD_MAILLIST H_WORD_OP
#define X_DIAL_WORD_MAILLIST 0
#define Y_DIAL_WORD_MAILLIST _Y_DIAL_LOCATION(2,H_WORD_OP)
*/
//�����б���ʾ����--------------------------------------
#define W_CALL_LIST_WORD W_DIAL_WORD_CENTER
#define H_CALL_LIST_WORD H_DIAL_WORD_CENTER
#define X_CALL_LIST_WORD 0
#define Y_CALL_LIST_WORD Y_OP_AREA + 90
#endif

#if (defined TC3000_18S)
//��ʾ����--------------------------------------
#define W_REMINDER_AREA SCR_WIDTH
#define H_REMINDER_AREA 50
#define X_REMINDER_AREA 0
#define Y_REMINDER_AREA 0
//����ʱ��ؼ�
#define W_TIME W_REMINDER_AREA / 4 * 3
#define H_TIME 40
#define X_TIME X_REMINDER_AREA + 10
#define Y_TIME (H_REMINDER_AREA - H_TIME ) / 2
//���ӹ�������ͼ��
#define W_CENTER 30
#define H_CENTER 24
#define X_CENTER X_TIME + W_TIME
#define Y_CENTER 6
//��������ͼ��
#define W_NETWORK W_CENTER
#define H_NETWORK H_CENTER
#define X_NETWORK X_CENTER + W_CENTER + 20
#define Y_NETWORK Y_CENTER

//��������--------------------------------------
#define W_OP_AREA SCR_WIDTH
#define H_OP_AREA SCR_HEIGHT - H_REMINDER_AREA
#define X_OP_AREA 0
#define Y_OP_AREA H_REMINDER_AREA + 1

// ��ʾ�������� (������)
#define LINE_NUM 3
// 135Ϊ�����ܸ߶� ÿ�и�40
#define _WORD_INTERVAL_Y(Hight)        \
	((140 - (LINE_NUM * Hight)) / (LINE_NUM + 1) )
#define _Y_LOCATION( AIndex,Hight) \
	(Y_OP_AREA + 50 + (_WORD_INTERVAL_Y(Hight) * (AIndex + 1)) + (Hight* (AIndex)))
//����˵��
#define W_WORD_OP W_OP_AREA
#define H_WORD_OP 40
#define X_WORD_OP 0
#define Y_WORD_OP 50
//����ס�������뷿�ţ���#��
#define W_WORD_CALL W_OP_AREA
#define H_WORD_CALL H_WORD_OP
#define X_WORD_CALL 0
#define Y_WORD_CALL _Y_LOCATION(0,H_WORD_OP)
//��    ����������󣬰�*��
#define W_WORD_CLEAR W_OP_AREA
#define H_WORD_CLEAR H_WORD_OP
#define X_WORD_CLEAR 0
#define Y_WORD_CLEAR _Y_LOCATION(1,H_WORD_OP)
//�������ģ�����0000����#��
#define W_WORD_CENTER W_OP_AREA
#define H_WORD_CENTER H_WORD_OP
#define X_WORD_CENTER 0
#define Y_WORD_CENTER _Y_LOCATION(2,H_WORD_OP)

//��������--------------------------------------
// ��ʾ�������� (������)
#define DIAL_LINE_NUM 2 // 120Ϊ2���ܸ߶� ÿ�и�60
#define _DIAL_WORD_INTERVAL_Y(Hight)        \
	((120 - (DIAL_LINE_NUM * Hight)) / (DIAL_LINE_NUM + 1) )
#define _Y_DIAL_LOCATION( AIndex,Hight) \
	(Y_OP_AREA + 90 + (_DIAL_WORD_INTERVAL_Y(Hight) * (AIndex + 1)) + (Hight* (AIndex)))
//����
#define W_DIAL_WORD_NUMBER W_OP_AREA
#define H_DIAL_WORD_NUMBER 60
#define X_DIAL_WORD_NUMBER 0
#define Y_DIAL_WORD_NUMBER Y_OP_AREA + (60 - H_WORD_OP) /2
//�������ģ�����0000����#��
#define W_DIAL_WORD_CENTER W_OP_AREA
#define H_DIAL_WORD_CENTER H_WORD_OP
#define X_DIAL_WORD_CENTER 0
#define Y_DIAL_WORD_CENTER _Y_DIAL_LOCATION(0,H_WORD_OP)
//����ס�������뷿�ţ���#��
#define W_DIAL_WORD_CALL W_OP_AREA
#define H_DIAL_WORD_CALL H_WORD_OP
#define X_DIAL_WORD_CALL 0
#define Y_DIAL_WORD_CALL _Y_DIAL_LOCATION(1,H_WORD_OP)

//�����б���ʾ����--------------------------------------
#define W_CALL_LIST_WORD W_DIAL_WORD_CENTER
#define H_CALL_LIST_WORD 35
#define X_CALL_LIST_WORD 0
#define Y_CALL_LIST_WORD Y_OP_AREA + 80
#endif
typedef struct _BmpLocation {
	BITMAP 	*bmp;
	char 	*location;
}BmpLocation;

typedef struct _FontLocation {
	PLOGFONT *font;
	int size;
	int first_type;
}FontLocation;
/* ---------------------------------------------------------------------------*
 *                      variables define
 *----------------------------------------------------------------------------*/
PLOGFONT font14;
PLOGFONT font18;
PLOGFONT font20;
PLOGFONT font24;
PLOGFONT font32;
static PLOGFONT font50;

#if (defined TC3000_DE) || (defined TC3000_X6) || (defined TC3000_DK)
#define TIME_FONT 	font18
#define TITLE_FONT 	font24
#define WORD_FONT 	font24
#define LIST_FONT 	font24
#define DIAL_FONT 	font20
#endif
#if (defined TC3000_18S)
#define TIME_FONT 	font24
#define TITLE_FONT 	font32
#define WORD_FONT 	font32
#define LIST_FONT 	font24
#define DIAL_FONT 	font32
#endif

static BITMAP Bmp_bkg_standby; // ����ģʽ
static BITMAP Bmp_bkg_dial; 	// ����ģʽ
static BITMAP Bmp_icon_network_ok; // ����������
static BITMAP Bmp_icon_network_fail; // ��������ʧ��
static BITMAP Bmp_icon_center_ok; // �����������ӳɹ�
static BITMAP Bmp_icon_center_fail; // ������������ʧ��
BITMAP Bmp_bkg_setting; // ����ģʽ
BITMAP Bmp_bkg_addressbook; // �鿴ͨѶ¼

static BmpLocation bmp_load[] = {
	&Bmp_bkg_standby, 			"res/image/backgrd/background1.bmp",
	&Bmp_bkg_dial, 				"res/image/backgrd/background2.bmp",
	&Bmp_bkg_setting, 			"res/image/backgrd/background3.bmp",
	&Bmp_bkg_addressbook, 		"res/image/backgrd/background4.bmp",
	&Bmp_icon_network_ok, 		"res/image/icon/net_ok.bmp",
	&Bmp_icon_network_fail, 	"res/image/icon/net_error.bmp",
	&Bmp_icon_center_ok, 		"res/image/icon/center_ok.bmp",
	&Bmp_icon_center_fail, 		"res/image/icon/center_error.bmp",
};

/* ---------------------------------------------------------------------------*/
/**
 * @brief FONT_WEIGHT_DEMIBOLD�Ӵ�
 * @brief FONT_WEIGHT_BOOK	���Ӵ�
 */
/* ---------------------------------------------------------------------------*/
static FontLocation font_cn_load[] = {
	{&font14, 14,FONT_WEIGHT_DEMIBOLD},
	{&font18, 18,FONT_WEIGHT_DEMIBOLD},
	{&font20, 20,FONT_WEIGHT_DEMIBOLD},
	{&font24, 24,FONT_WEIGHT_DEMIBOLD},
	{&font32, 32,FONT_WEIGHT_DEMIBOLD},
	{&font50, 50,FONT_WEIGHT_DEMIBOLD},
};

static FontLocation font_en_load[] = {
	{&font14, 14,FONT_WEIGHT_DEMIBOLD},
	{&font18, 14,FONT_WEIGHT_DEMIBOLD},
	{&font20, 16,FONT_WEIGHT_DEMIBOLD},
	{&font24, 16,FONT_WEIGHT_DEMIBOLD},
	{&font32, 18,FONT_WEIGHT_DEMIBOLD},
	{&font50, 50,FONT_WEIGHT_DEMIBOLD},
};

static FontLocation font_ru_load[] = {
	{&font14, 12,FONT_WEIGHT_DEMIBOLD},
	{&font18, 12,FONT_WEIGHT_DEMIBOLD},
	{&font20, 14,FONT_WEIGHT_DEMIBOLD},
	{&font24, 14,FONT_WEIGHT_DEMIBOLD},
	{&font32, 16,FONT_WEIGHT_DEMIBOLD},
	{&font50, 50,FONT_WEIGHT_DEMIBOLD},
};


static MY_CALL_LIST_FONTS CallListFont = {
	&LIST_FONT
};

static MY_CALL_LIST_TEMPLATE CallListInitParam = {
    WS_CHILD|WS_VISIBLE,
    WS_EX_TRANSPARENT,
    X_CALL_LIST_WORD,Y_CALL_LIST_WORD,W_CALL_LIST_WORD,H_CALL_LIST_WORD,
    IDC_MAIN_MYLISTBOX,
    &CallListFont,
    NULL,
};

char font_type[10] = {0},font_family[10] = {0},font_charset[10] = {0};
static HWND mainHwnd = HWND_INVALID;
static FormMainTimers *timers_tbl;
static DisplayMsg disp_msg; // ��ʾ���Ż���
static int key_press_time = KEY_PRESS_TIME; // ������ʱ
static int (*mainAppProc)(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);
static void (*keyHandle)(unsigned int key_value,int ret);
/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainKeyboardNotify С���̰����ص�����
 *
 * @param hwnd
 * @param id
 * @param nc
 * @param add_data ��ֵ 0-12
 */
/* ---------------------------------------------------------------------------*/
static void formMainKeyboardNotify(DWORD add_data)
{
	int ret = 1;
	if (video->getStatus(video) == ST_IDLE) {
		formMainResetKeyPressTime();
		if (add_data <= 9) {
			formMainSetKeyboardType(KEY_TYPE_STANDY);
			formMainAddTextNumber(add_data);
		} else if (add_data == 11) {
			if(CurrentStatus == 0){
				ret = formMainDeleteTextNumber();
			}else if(CurrentStatus == 1){
				ret = 3; 		// 3Ϊ��������� ������ת��ͨѶ¼����
			}
			
		}
	} 
	
	keyHandle(add_data,ret);
	
}

static void formMainKey(int val)
{
	if (Screen.tail) {
		SendMessage(Screen.tail->hWnd,MSG_KEYBOARD,(WPARAM)val,0);
	}
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainBmpLoad װ��ͼƬ��Դ
 *
 * @param bmp ͼƬ�ṹ
 * @param num ͼƬ��Ŀ
 */
/* ---------------------------------------------------------------------------*/
static void formMainBmpLoad(BmpLocation *bmp,int num)
{
	int i;
	for (i=0; i<num; i++) {
		if (LoadBitmap (HDC_SCREEN, bmp[i].bmp, bmp[i].location)){
			printf ("LoadBitmap(%s)fail.\n",bmp[i].location);
		}
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainBmpRelease �ͷ�ͼƬ��Դ
 *
 * @param bmp ͼƬ�ṹ
 * @param num ͼƬ��Ŀ
 */
/* ---------------------------------------------------------------------------*/
static void formMainBmpRelease(BmpLocation *bmp,int num)
{
	int i;
	for (i=0; i<num; i++) {
		UnloadBitmap(bmp[i].bmp);
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainFontLoad װ������
 *
 * @param font ����ṹ
 * @param num ������Ŀ
 */
/* ---------------------------------------------------------------------------*/
static void formMainFontLoad(FontLocation *font,int num)
{
	int i;
	for (i=0; i<num; i++) {
		*font[i].font = CreateLogFont(font_type,
				font_family, font_charset,
				font[i].first_type,
				FONT_SLANT_ROMAN,
				FONT_OTHER_NIL,
				FONT_OTHER_LCDPORTRAIT,
				FONT_UNDERLINE_NONE,
				FONT_STRUCKOUT_NONE,
				font[i].size, 0);
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainFontRelease �ͷ�������Դ
 *
 * @param font ����ṹ
 * @param num ������Ŀ
 */
/* ---------------------------------------------------------------------------*/
static void formMainFontRelease(FontLocation *font,int num)
{
	int i;
	for (i=0; i<num; i++) {
		DestroyLogFont(*font[i].font);
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainTimerStart ������ʱ�� ��λ100ms
 *
 * @param idc_timer ��ʱ��id�ţ�ͬʱҲ�Ǳ��
 */
/* ---------------------------------------------------------------------------*/
static void formMainTimerStart(int idc_timer)
{
	SendMessage(mainHwnd, MSG_MAIN_TIMER_START, (WPARAM)idc_timer, 0);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainTimerStop �رն�ʱ��
 *
 * @param idc_timer ��ʱ��id��
 */
/* ---------------------------------------------------------------------------*/
static void formMainTimerStop(int idc_timer)
{
	SendMessage(mainHwnd, MSG_MAIN_TIMER_STOP, (WPARAM)idc_timer, 0);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainTimerGetState ���ض�ʱ����ǰ�Ƿ񼤻�
 *
 * @param idc_timer
 *
 * @returns 1���� 0δ����
 */
/* ---------------------------------------------------------------------------*/
static int formMainTimerGetState(int idc_timer)
{
	return	IsTimerInstalled(mainHwnd,idc_timer);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainTimerStopAllCalling �ر�����ͨ��ʱ�Ķ�ʱ��
 */
/* ---------------------------------------------------------------------------*/
static void formMainTimerStopAllCalling(void)
{
	int i;
	for (i=IDC_TIMER_CALL_OUT; i<=IDC_TIMER_SHAKEHANS_OUT; i++) {
		formMainTimerStop(i);
	}
}

static void formMainSetClockText(char *string)
{
	SetWindowText(GetDlgItem(mainHwnd,IDC_LB_TIME),string);
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainUpdateClock ����������ʱ��
 *
 * @returns 1��Ҫ���������ͬ��ʱ�䣬0����Ҫ
 */
/* ---------------------------------------------------------------------------*/
static int formMainUpdateClock(void)
{
	char buff[512];
	int ret = 0;
	time_t t;
	struct tm *tm;
	static int old_hour = 0,old_sec = 0;
	time(&t);
	tm = localtime(&t);

	if( (old_hour != tm->tm_hour) || (old_sec != tm->tm_sec) ) {
		old_sec = tm->tm_sec;
		if (Public.MainWndMode == 0) {
			sprintf(buff,"%d%s%d%s%d%s  %02d:%02d:%02d",
					tm->tm_year+1900,
					Word[WORD_YEAR].string,
					tm->tm_mon+1,
					Word[WORD_MOUNTH].string,
					tm->tm_mday,
					Word[WORD_DATE].string,
					tm->tm_hour,
					tm->tm_min,
					tm->tm_sec);
			formMainSetClockText(buff);
		}
		if (old_hour != tm->tm_hour) {
			old_hour = tm->tm_hour;
			ret = 1;
		}
	}

	return ret;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainSetNetWorkState �ı�����״̬ͼ��
 *
 * @param state 0δ���� 1����
 */
/* ---------------------------------------------------------------------------*/
static void formMainSetNetWorkState(int state)
{
	if (state) {
		InvalidateRect (GetDlgItem(mainHwnd, IDC_LB_NETWORK), NULL, TRUE);
		SendMessage(GetDlgItem(mainHwnd, IDC_LB_NETWORK),
				STM_SETIMAGE, (WPARAM)&Bmp_icon_network_ok, 0);
	} else {
		SendMessage(GetDlgItem(mainHwnd, IDC_LB_NETWORK),
				STM_SETIMAGE, (WPARAM)&Bmp_icon_network_fail, 0);
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainSetCenterOnlineState �ı������������ͼ��
 *
 * @param state 0���� 1����
 */
/* ---------------------------------------------------------------------------*/
static void formMainSetCenterOnlineState(int state)
{
	if (state) {
		InvalidateRect (GetDlgItem(mainHwnd, IDC_LB_CENTER), NULL, TRUE);
		SendMessage(GetDlgItem(mainHwnd,IDC_LB_CENTER),
				STM_SETIMAGE,(WPARAM)&Bmp_icon_center_ok,0);
	} else {
		SendMessage(GetDlgItem(mainHwnd,IDC_LB_CENTER),
				STM_SETIMAGE,(WPARAM)&Bmp_icon_center_fail,0);
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainSetTextOpration ���ð���������
 *
 * @param string
 */
/* ---------------------------------------------------------------------------*/
static void	formMainSetTextOpration(char *string)
{
	SetWindowText(GetDlgItem(mainHwnd,IDC_LB_TIME),string);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainSetTextDial ����ͨ����ʾʱ������
 *
 * @param string
 */
/* ---------------------------------------------------------------------------*/
static void formMainSetTextDial(char *string)
{
	SetWindowText(GetDlgItem(mainHwnd,IDC_LB_DIAL_NUMBER),string);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainSetTextNumber ���ò���ʱ�Ĵ�����
 *
 * @param string
 */
/* ---------------------------------------------------------------------------*/
static void formMainSetTextNumber(char *string)
{
	SetWindowText(GetDlgItem(mainHwnd,IDC_LB_DIAL_NUMBER),string);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainAddTextNumber ���Ӳ���ʱ�����֣�������
 *
 * @param num
 */
/* ---------------------------------------------------------------------------*/
static void formMainAddTextNumber(int num)
{
	if (disp_msg.index < MAX_DIAL_NUM) {
		disp_msg.buf[disp_msg.index++] = '0' + num;
	} else {
		disp_msg.buf[disp_msg.index] = '\0';
	}
	formMainShowNumber(1);
	formMainSetTextNumber(disp_msg.buf);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainDeleteTextNumber ɾ������ʱ�����֣�������
 *
 * @returns 1ɾ���ɹ���0ɾ��ʧ�ܣ������Ѿ�Ϊ�� 
 */
/* ---------------------------------------------------------------------------*/
static int formMainDeleteTextNumber(void)
{
	if (disp_msg.index <= 0) {
		return 0;
	}
	disp_msg.index--;
	if (disp_msg.index == 0) {
		formMainClearDialNumber();
		formMainSetTextNumber(Word[WORD_PLEASE_INSERT_PHONE_NUM].string);
	} else {
		disp_msg.buf[disp_msg.index] = '\0';
		formMainSetTextNumber(disp_msg.buf);
	}
	return 1;
	
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainSetKeyboardType ���ü�������
 *
 * @param type KEY_TYPE_STANDY ���ż��̣�
 * 			   KEY_TYPE_CALLING ͨ�����̣�
 *
 */
/* ---------------------------------------------------------------------------*/
static void formMainSetKeyboardType(KeyType type)
{
	formMainSetTextOpration("");
	if (type == KEY_TYPE_STANDY) {
		
		SetWindowText(GetDlgItem(mainHwnd,IDC_LB_DIAL_CENTER),
				Word[WORD_HELP_CENTER].string);
		SetWindowText(GetDlgItem(mainHwnd, IDC_LB_DIAL_CALL),
				Word[WORD_HELP_CALL].string);
	} else if (type == KEY_TYPE_CALLING) {
		formMainListDeletAll();
		formMainListRefresh();
		SetWindowText(GetDlgItem(mainHwnd, IDC_LB_DIAL_CENTER),
				Word[WORD_HELP_LEAVE_WORD].string);
		SetWindowText(GetDlgItem(mainHwnd, IDC_LB_DIAL_CALL),
				Word[WORD_HELP_HAND_UP].string);
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainGetTextNumber ��ȡ���ŵ�ǰ����
 *
 * @param ret_num ���ص�ǰ����
 *
 * @returns �������ֳ���
 */
/* ---------------------------------------------------------------------------*/
static unsigned int formMainGetTextNumber(char *ret_num)
{
	strcpy(ret_num,disp_msg.buf);
	return disp_msg.index;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainShowNumber �Ƿ���ʾ�����ֲ���
 *
 * @param type 1 ��ʾ 0����
 */
/* ---------------------------------------------------------------------------*/
static void formMainShowNumber(int type)
{
	if (Public.MainWndMode == 1)
		return;
	Public.MainWndMode = 1;
	int i;
	for (i=0; i<(IDC_LB_HELP_CENTER - IDC_LB_HELP_OP + 1); i++) {
		ShowWindow(GetDlgItem(mainHwnd,IDC_LB_HELP_OP + i),SW_HIDE);
	}
	
	for (i=0; i<(IDC_LB_DIAL_CALL - IDC_LB_DIAL_NUMBER + 1); i++) {
		ShowWindow(GetDlgItem(mainHwnd,IDC_LB_DIAL_NUMBER + i),SW_SHOWNORMAL);
	}
	ShowWindow(GetDlgItem(mainHwnd,IDC_MAIN_MYLISTBOX),SW_SHOWNORMAL);
	InvalidateRect (mainHwnd, NULL, TRUE);
	CurrentStatus = 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainClearDialNumber ������Ž�������
 */
/* ---------------------------------------------------------------------------*/
static void formMainClearDialNumber(void)
{
	memset(&disp_msg,0,sizeof(disp_msg));
	formMainSetTextNumber("");

	// formMainShowNumber(0);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainShowCalling ��ʾͨ������
 */
/* ---------------------------------------------------------------------------*/
static void formMainShowCalling(void)
{
	screensaverStart(LCD_ON);
	formMainShowNumber(0);
	ShowWindow(GetDlgItem(mainHwnd,IDC_MAIN_MYLISTBOX),SW_HIDE);
	InvalidateRect (mainHwnd, NULL, TRUE);
	SetWindowText(GetDlgItem(mainHwnd, IDC_LB_DIAL_CENTER),
			Word[WORD_HELP_LEAVE_WORD].string);
	SetWindowText(GetDlgItem(mainHwnd, IDC_LB_DIAL_CALL),
			Word[WORD_HELP_HAND_UP].string);
	
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainShowStandby ��ʾ��������
 *
 * @param hWnd
 */
/* ---------------------------------------------------------------------------*/
static void formMainShowStandby(void)
{
	int i;
	if (Public.MainWndMode == 0)
		return;
	Public.MainWndMode = 0;

	CurrentStatus = 1;

	//��λ����IP
	memset(Public.mCallIP,0,sizeof(Public.mCallIP));
	
	formMainClearDialNumber();

	formMainTimerStopAllCalling();

	for (i=0; i<(IDC_LB_HELP_CENTER - IDC_LB_HELP_OP + 1); i++) {
		ShowWindow(GetDlgItem(mainHwnd,IDC_LB_HELP_OP + i),SW_SHOWNORMAL);

	}

	for (i=0; i<(IDC_LB_DIAL_CALL - IDC_LB_DIAL_NUMBER + 1); i++) {
		ShowWindow(GetDlgItem(mainHwnd,IDC_LB_DIAL_NUMBER + i),SW_HIDE);
	}
	ShowWindow(GetDlgItem(mainHwnd,IDC_MAIN_MYLISTBOX),SW_HIDE);
	InvalidateRect (mainHwnd, NULL, TRUE);

}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainTimerProc1s ������ض�ʱ���� 
 *
 * @returns 1������ʱ�˳� 0δ��ʱ�˳�
 */
/* ---------------------------------------------------------------------------*/
static int formMainTimerProc1s(void)
{
	if (video->getStatus(video) != ST_IDLE) {
		return 0;
	}

	// ������ʱ
	if(Public.bEnableScrSavers) {
		if(Public.ScrSaversTimer < Public.MaxScrSaversTimer) {
			Public.ScrSaversTimer++;
			if(Public.ScrSaversTimer >= Public.MaxScrSaversTimer) {
				screensaverStart(LCD_OFF);
			}
		}
	}

	if (key_press_time < KEY_PRESS_TIME) {
		DBG_P("[%s]key:%d\n", __FUNCTION__,key_press_time);
		if (++key_press_time == KEY_PRESS_TIME) {
			char buf[16];
			int len = formMainGetTextNumber(buf);
			formMainShowStandby();
			if (Public.m_bSetStatus == FALSE) {
				if (len) {
					return 1;
				} else {
					return 0;
				}
			}
		}
	}
	return 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainListDeletAll �����б����
 */
/* ---------------------------------------------------------------------------*/
static void formMainListDeletAll(void)
{
	SendMessage(GetDlgItem(mainHwnd, IDC_MAIN_MYLISTBOX),
			MSG_DELETE_ALL, 0, 0);
}
static void formMainListAddItem(int item)
{
	SendMessage(GetDlgItem(mainHwnd, IDC_MAIN_MYLISTBOX),
			MSG_ADD_ITEM,
			(WPARAM)item, 0);
}
static void formMainListRefresh(void)
{
	SendMessage(GetDlgItem(mainHwnd, IDC_MAIN_MYLISTBOX),
			MSG_RERESH, 0, 0);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainResetKeyPressTime ���ð���ʱ��
 */
/* ---------------------------------------------------------------------------*/
static void formMainResetKeyPressTime(void)
{
	key_press_time = 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainCreateForm ��������
 *
 * @param type
 * @param para
 */
/* ---------------------------------------------------------------------------*/
static void formMainCreateForm(FormType type,void *para)
{
	switch (type) 
	{
		case FORM_UNBROKEN :
			CreateUpdateForm(mainHwnd,
					&Bmp_bkg_setting,
					NULL,
					UPDATE_TYPE_UNBROKEN);
			break;
		case FORM_RECOVER:
			CreateUpdateForm(mainHwnd,
					&Bmp_bkg_setting,
					NULL,
					UPDATE_TYPE_RECOVER);
			break;
		case FORM_UPDATE:
			CreateUpdateForm(mainHwnd,
					&Bmp_bkg_setting,
					(void *)para,
					UPDATE_TYPE_PROCESS);
			break;
		case FORM_UPDATE_IC:
			CreateUpdateForm(mainHwnd,
					&Bmp_bkg_setting,
					(void *)para,
					UPDATE_TYPE_IC_CARD);
			break;
		case FORM_UPDATE_USER:
			CreateUpdateForm(mainHwnd,
					&Bmp_bkg_setting,
					(void *)para,
					UPDATE_TYPE_USER);
			break;
		case FORM_UNLOCK:
			CreatePasswordForm(Screen.hMainWnd,
					TYPE_PASSWORD_UNLOCK,
					&Bmp_bkg_setting);
			break;
		case FORM_SETTING:
			CreatePasswordForm(Screen.hMainWnd,
					TYPE_PASSWORD_SETTING,
					&Bmp_bkg_setting);
			break;
		//查询户主信息列表
		case FORM_USERSEARCH:
			CreateUserSeachForm(mainHwnd,
					&Bmp_bkg_addressbook);
			break;
		default:
			break;
	}	
}
	
/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainCreateControl ���������ڻ����ؼ�
 *
 * @param hWnd
 */
/* ---------------------------------------------------------------------------*/
#define CEATE_STATIC_IMAGE(x,y,w,h,id,dwAddData) \
	CreateWindowEx("static",\
			"",\
			WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTERIMAGE,\
			WS_EX_TRANSPARENT,\
			id,\
			x, y, w, h,\
			hWnd,\
			dwAddData)

#define CEATE_STATIC_LABER(caption,x,y,w,h,id) \
    CreateWindowEx("static",\
			caption,\
			WS_CHILD|WS_VISIBLE|SS_CENTER,\
			WS_EX_TRANSPARENT,\
			id,\
			x, y, w, h,\
			hWnd,\
			0)

#define CEATE_STATIC_LABER_DISVISIBLE(caption,x,y,w,h,id) \
CreateWindowEx("static",\
		caption,\
		WS_CHILD|SS_CENTER,\
		WS_EX_TRANSPARENT,\
		id,\
		x, y, w, h,\
		hWnd,\
		0)

#define CEATE_STATIC_LABER_L(caption,x,y,w,h,id) \
    CreateWindowEx("static",\
			caption,\
			WS_CHILD|WS_VISIBLE|SS_LEFT,\
			WS_EX_TRANSPARENT,\
			id,\
			x, y, w, h,\
			hWnd,\
			0)

#define CEATE_STATIC_LABER_L_DISVISIBLE(caption,x,y,w,h,id) \
    CreateWindowEx("static",\
			caption,\
			WS_CHILD|SS_LEFT,\
			WS_EX_TRANSPARENT,\
			id,\
			x, y, w, h,\
			hWnd,\
			0)
/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainCreateControl �����ؼ�
 *
 * @param hWnd
 */
/* ---------------------------------------------------------------------------*/
static void formMainCreateControl(HWND hWnd)
{
	HWND hCtrl;
	// --------------------------״̬��--------------------------
	//����ʱ��ؼ�
	hCtrl = CEATE_STATIC_LABER_L("",
			X_TIME, Y_TIME, W_TIME, H_TIME,
		   	IDC_LB_TIME);
	// SetWindowBkColor(hCtrl,MY_WHIGHT);
	SetWindowElementAttr(hCtrl, WE_FGC_WINDOW, MY_WHIGHT);
	SetWindowFont(hCtrl,TIME_FONT);
	//��������ͼ��
	CEATE_STATIC_IMAGE(X_NETWORK, Y_NETWORK, W_NETWORK, H_NETWORK,
		   	IDC_LB_NETWORK,
		   	(DWORD)&Bmp_icon_network_fail);
	//���ӹ�������ͼ��
	CEATE_STATIC_IMAGE(X_CENTER, Y_CENTER, W_CENTER, H_CENTER,
		   	IDC_LB_CENTER,
		   	(DWORD)&Bmp_icon_center_fail);

	// --------------------------����״̬--------------------------
	//����˵��
	hCtrl = CEATE_STATIC_LABER(Word[WORD_HELP_OP].string,
			X_WORD_OP,
			Y_WORD_OP,
			W_WORD_OP,
			H_WORD_OP,
			IDC_LB_HELP_OP);
	SetWindowFont(hCtrl,TITLE_FONT);
	//����ס�������뷿�ţ���#��
	hCtrl = CEATE_STATIC_LABER_L(Word[WORD_HELP_CALL].string,
			X_WORD_CALL,
			Y_WORD_CALL,
			W_WORD_CALL,
			H_WORD_CALL,
			IDC_LB_HELP_CALL);
	SetWindowFont(hCtrl,WORD_FONT);
	//��    ����������󣬰�*��
	hCtrl = CEATE_STATIC_LABER_L(Word[WORD_HELP_CLEAR].string,
			X_WORD_CLEAR,
			Y_WORD_CLEAR,
			W_WORD_CLEAR,
			H_WORD_CLEAR,
			IDC_LB_HELP_CLEAR);
	SetWindowFont(hCtrl,WORD_FONT);
	//�������ģ�����0000����#��
	hCtrl = CEATE_STATIC_LABER_L(Word[WORD_HELP_CENTER].string,
			X_WORD_CENTER,
			Y_WORD_CENTER,
			W_WORD_CENTER,
			H_WORD_CENTER,
			IDC_LB_HELP_CENTER);
	SetWindowFont(hCtrl,WORD_FONT);
	
	// --------------------------���źͺ���״̬--------------------------
	//����
	hCtrl = CEATE_STATIC_LABER_DISVISIBLE(Word[WORD_PLEASE_INSERT_PHONE_NUM].string,
			X_DIAL_WORD_NUMBER,
			Y_DIAL_WORD_NUMBER,
			W_DIAL_WORD_NUMBER,
			H_DIAL_WORD_NUMBER,
			IDC_LB_DIAL_NUMBER);
	SetWindowElementAttr(hCtrl, WE_FGC_WINDOW, MY_WHIGHT);
	SetWindowFont(hCtrl,font50);
	//�������ģ�����0000����#��
	hCtrl = CEATE_STATIC_LABER_L_DISVISIBLE("",
			X_DIAL_WORD_CENTER,
			Y_DIAL_WORD_CENTER,
			W_DIAL_WORD_CENTER,
			H_DIAL_WORD_CENTER,
			IDC_LB_DIAL_CENTER);
	SetWindowElementAttr(hCtrl, WE_FGC_WINDOW, MY_BLACK_TOP); //��ɫ
	SetWindowFont(hCtrl,DIAL_FONT);
	//����ס�������뷿�ţ���#��
	hCtrl = CEATE_STATIC_LABER_L_DISVISIBLE("",
			X_DIAL_WORD_CALL,
			Y_DIAL_WORD_CALL,
			W_DIAL_WORD_CALL,
			H_DIAL_WORD_CALL,
			IDC_LB_DIAL_CALL);
	SetWindowElementAttr(hCtrl, WE_FGC_WINDOW, MY_BLACK_TOP); //��ɫ
	SetWindowFont(hCtrl,DIAL_FONT);
/*
	//������ѯ������8888����#��
	hCtrl = CEATE_STATIC_LABER_L_DISVISIBLE("",
			X_DIAL_WORD_MAILLIST,
			Y_DIAL_WORD_MAILLIST,
			W_DIAL_WORD_MAILLIST,
			H_DIAL_WORD_MAILLIST,
			IDC_LB_DIAL_MAILLIST);
	SetWindowElementAttr(hCtrl, WE_FGC_WINDOW, MY_BLACK_TOP); //��ɫ
	SetWindowFont(hCtrl,DIAL_FONT);
*/
	// ChangeDialDisp(hWnd,0);
	CreateMyListBox((void *)&CallListInitParam,hWnd,TYPE_CALL_LIST,4);
}


/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainProc ���ڻص�����
 *
 * @param hDlg
 * @param message
 * @param wParam
 * @param lParam
 *
 * @return
 */
/* ---------------------------------------------------------------------------*/
static int formMainProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	// ִ�л���ҵ���߼�
	if (mainAppProc(hWnd, message, wParam, lParam) == 0) {
		return DefaultMainWinProc(hWnd, message, wParam, lParam);
	}

	switch (message)
	{
		case MSG_CREATE:
			{
				Screen.Add(hWnd,"TFrmMain");
				mainHwnd = Screen.hMainWnd = hWnd;
				// ���԰���ʼ��
				languageLoad();
				ClearFramebuffer();
				// װ������
				if (Public.language == CHINEASE || Public.language == TAIWAN) {
					formMainFontLoad(font_cn_load,sizeof(font_cn_load) / sizeof(FontLocation));
				} else if(Public.language == ENGLISH || Public.language == TURKISH) {
					formMainFontLoad(font_en_load,sizeof(font_en_load) / sizeof(FontLocation));
				} else{
					formMainFontLoad(font_ru_load,sizeof(font_ru_load) / sizeof(FontLocation));
				}
				// װ��ͼƬ
				formMainBmpLoad(bmp_load,sizeof(bmp_load) / sizeof(BmpLocation));
				// ���������ڿؼ�
				formMainCreateControl(hWnd);

				formMainTimerStart(IDC_TIMER_1S);
				formMainTimerStart(IDC_TIMER_60S);
				formMainTimerStart(IDC_TIMER_UPDATE_TIME_200MS);
				formMainShowStandby();
				screensaverStart(LCD_ON);
			} break;

		case MSG_ERASEBKGND:
			{
				BITMAP *Bmp_bkg;
				if (Public.MainWndMode == 0) {
					Bmp_bkg = &Bmp_bkg_standby;
				} else {
					Bmp_bkg = &Bmp_bkg_dial;
				}
				DrawBackground(hWnd, (HDC)wParam, (const RECT*)lParam,Bmp_bkg);
			} return 0;

		case MSG_KEYBOARD:
			{
				formMainKeyboardNotify(wParam);
			} return 0;

		case MSG_MAIN_TIMER_START:
			{
				SetTimer(hWnd,
						wParam,
						(timers_tbl + wParam)->time * TIME_100MS);
			} return 0;

		case MSG_MAIN_TIMER_STOP:
			{
				if (IsTimerInstalled(mainHwnd,wParam) == TRUE) {
					KillTimer (mainHwnd,wParam);
				}
			} return 0;

		case MSG_TIMER:
			{
				if ((wParam >= IDC_TIMER_1S) && (wParam < IDC_TIMER_NUM)) {
					(timers_tbl + wParam)->proc();
				}
			} return 0;

		case MSG_LBUTTONDOWN:
			{
				screensaverStart(LCD_ON);
			} break;

		case MSG_DESTROY:
			{
				Screen.Del(hWnd);
				DestroyAllControls (hWnd);
			} return 0;

		case MSG_CLOSE:
			{
				int i;
				for (i=IDC_TIMER_1S; i<IDC_TIMER_NUM; i++) {
					formMainTimerStop(i);
				}
				DestroyMainWindow (hWnd);
				PostQuitMessage (hWnd);
			} return 0;

		default:
			break;
	}
	return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainLoop ��������Ϣѭ��
 *
 * @returns 
 */
/* ---------------------------------------------------------------------------*/
static int formMainLoop(void)
{
    MSG Msg;
	while (GetMessage(&Msg, Screen.hMainWnd)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	MyListboxControlCleanup();
    MainWindowThreadCleanup (Screen.hMainWnd);
	mainHwnd = Screen.hMainWnd = 0;
	if (Public.language == CHINEASE || Public.language != TAIWAN) {
		formMainFontRelease(font_cn_load,sizeof(font_cn_load) / sizeof(FontLocation));
	} else {
		formMainFontRelease(font_en_load,sizeof(font_en_load) / sizeof(FontLocation));
	}
	formMainBmpRelease(bmp_load,sizeof(bmp_load) / sizeof(BmpLocation));
	return 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief formMainCreate ���������� 
 *
 * @param AppProc
 *
 * @returns 
 */
/* ---------------------------------------------------------------------------*/
FormMain * formMainCreate(
		int (*AppProc)(HWND,int,WPARAM,LPARAM),
		void (*keyboardHandle)(unsigned int,int),
		FormMainTimers *timers_table)
{
    MAINWINCREATE CreateInfo;
	keyHandle = keyboardHandle;
	mainAppProc = AppProc;
	timers_tbl = timers_table;
	RegisteMyListboxControl();

    CreateInfo.dwStyle = WS_NONE;
	CreateInfo.dwExStyle = WS_EX_AUTOSECONDARYDC;
	// CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "TC3000";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(IDC_ARROW);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = formMainProc;
    CreateInfo.lx = 0;
    CreateInfo.ty = 0;
    CreateInfo.rx = SCR_WIDTH;
    CreateInfo.by = SCR_HEIGHT;
    CreateInfo.iBkColor = GetWindowElementColor(WE_MAINC_THREED_BODY);
    CreateInfo.dwAddData = 0;
    CreateInfo.dwReserved = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

	CreateMainWindow (&CreateInfo);
	if (mainHwnd == HWND_INVALID)
		return NULL;
	ShowWindow(mainHwnd, SW_SHOWNORMAL);

	FormMain * this = (FormMain *) calloc (1,sizeof(FormMain));
	this->timer = timers_table;
	this->loop = formMainLoop;
	this->timerStart = formMainTimerStart;
	this->timerStop = formMainTimerStop;
	this->updateClock = formMainUpdateClock;
	this->timerGetState = formMainTimerGetState;
	this->timerStopAllCalling = formMainTimerStopAllCalling;
	this->setNetWorkState = formMainSetNetWorkState;
	this->setCenterOnlineState = formMainSetCenterOnlineState;
	this->setTextOpration = formMainSetTextOpration;
	this->setTextDial = formMainSetTextDial;
	this->getTextNumber = formMainGetTextNumber;
	this->setKeyboardType = formMainSetKeyboardType;
	this->showCalling = formMainShowCalling;
	this->clearDialNumber = formMainClearDialNumber;
	this->timerProc1s = formMainTimerProc1s;
	this->keyNotify = formMainKey;
	this->listAddItem = formMainListAddItem;
	this->listDeletAll = formMainListDeletAll;
	this->listRefresh = formMainListRefresh;
	this->resetKeyPressTime = formMainResetKeyPressTime;
	this->createForm = formMainCreateForm;

	return this;
}

