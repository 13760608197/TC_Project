#ifndef PredefineH
#define PredefineH

#if (defined TC3000_DE)
#define DEVICE_MODE "TC-3000D-J"
#define SCR_WIDTH 320           /*LCD Width     */
#define SCR_HEIGHT 240          /*LCD Height    */
#define LOGFONTSIZE 20
#endif
#if (defined TC3000_X6)
#define DEVICE_MODE "TC3000-X6"
#define SCR_WIDTH 320           /*LCD Width     */
#define SCR_HEIGHT 240          /*LCD Height    */
#define LOGFONTSIZE 20
#endif
#if (defined TC3000_DK)
#define DEVICE_MODE "TC3000-DK"
#define SCR_WIDTH 320           /*LCD Width     */
#define SCR_HEIGHT 240          /*LCD Height    */
#define LOGFONTSIZE 20
#endif
#if (defined TC3000_18S)
#define DEVICE_MODE "TC3000-D18S"
#define SCR_WIDTH 480           /*LCD Width     */
#define SCR_HEIGHT 272          /*LCD Height    */
#define LOGFONTSIZE 20
#endif

#if (defined TC3000_BZ)
#define PROTOCOL_MODE "BZ"
#endif

#if (defined TC3000_FL)
#define PROTOCOL_MODE "FL"
#endif

#if (defined TC3000_WM)
#define PROTOCOL_MODE "WM"
#endif
//-----------------------------------------------------------------------

#define MAX_VOLUME_EX   96 //最大音量百分比

#define INI_KEY_START   55 //配置文件标志
#define INI_KEY_END     99 //
//------------按键定义------------
#define KEYLEAVEWORD	10 // 留言键
#define KEYBACK			11 // "*"
#define KEYOK			12 // "#"

// 最大显示数值个数
#define MAX_DIAL_NUM	10

#define  MAX_HOMELIST_NUM     	13

#define FLASH_DIR	"/mnt/nand1-2"

#define FIRST_UPDATE_TIME     60 // 首次获取管理中心时间间隔 60s
#define MY_BLACK    0x01010101
#define MY_BLACK_TOP    0x11111111
#define MY_WHIGHT    0XFFFFFF
#define MY_RED    0X0000FF
#define MIN_LCD_LIGHT 10
#define LCD_OFF     FALSE
#define LCD_ON      TRUE

#define FORM_SETTING_ONTIME      60 // 设置界面60秒无操作，则关闭窗口
#define DISP_STANDY_ONTIME      10  // 清空提示栏的时间 10s
#define KEY_PRESS_TIME      	5  // 按键超时时间

#define MSG_KEYBOARD		 	(MSG_USER + 500)
#define MSG_SERIALPORT		 	(MSG_USER + 501)

#define STATIC_LB(x,y,w,h,id,caption,dwAddData,font)    \
{CTRL_STATIC,WS_CHILD|WS_VISIBLE|SS_CENTER,\
	x,y,w,h,id,caption,dwAddData,WS_EX_TRANSPARENT,NULL,NULL,font}

#define STATIC_IMAGE(x,y,w,h,id,dwAddData)    \
{CTRL_STATIC,WS_CHILD|WS_VISIBLE|SS_BITMAP|SS_CENTER|SS_NOTIFY,\
	x,y,w,h,id,"",dwAddData,WS_EX_TRANSPARENT,NULL,NULL,NULL}

#define STATIC_LB_DISVISIBLE(x,y,w,h,id,caption,dwAddData,font)    \
{CTRL_STATIC,WS_CHILD|SS_CENTER,\
	x,y,w,h,id,caption,dwAddData,WS_EX_TRANSPARENT,NULL,NULL,font}

#define STATIC_LB_L_DISVISIBLE(x,y,w,h,id,caption,dwAddData,font)    \
{CTRL_STATIC,WS_CHILD|SS_LEFT,\
	x,y,w,h,id,caption,dwAddData,WS_EX_TRANSPARENT,NULL,NULL,font}

#define STATIC_LB_L(x,y,w,h,id,caption,dwAddData,font)  \
{CTRL_STATIC,WS_CHILD|WS_VISIBLE|SS_LEFT,\
	x,y,w,h,id,caption,dwAddData,WS_EX_TRANSPARENT,NULL,NULL,font}

#define STATIC_LB_L_NOTIFY(x,y,w,h,id,caption,dwAddData,font)  \
{CTRL_STATIC,WS_CHILD|WS_VISIBLE|SS_LEFT|SS_NOTIFY,\
	x,y,w,h,id,caption,dwAddData,WS_EX_TRANSPARENT,NULL,NULL,font}
// GROUP 为一组控件的开头
#define GROUP_BUTTON_LB(x,y,w,h,id,caption,dwAddData,font)    \
{CTRL_BUTTON,WS_CHILD|WS_VISIBLE |  BS_CHECKED | WS_TABSTOP | WS_GROUP,\
	x,y,w,h,id,caption,dwAddData,0,NULL,NULL,font}

#define BUTTON_LB(x,y,w,h,id,caption,dwAddData,font)    \
{CTRL_BUTTON,WS_CHILD|WS_VISIBLE | WS_TABSTOP,\
	x,y,w,h,id,caption,dwAddData,0,NULL,NULL,font}

#define BUTTON_CHECKVOX(x,y,w,h,id,caption,dwAddData,font)    \
{CTRL_BUTTON,WS_CHILD|WS_VISIBLE |BS_AUTOCHECKBOX | WS_TABSTOP,\
	x,y,w,h,id,caption,dwAddData,WS_EX_TRANSPARENT,NULL,NULL,font}

#define GROUP_BUTTON_CHECKVOX(x,y,w,h,id,caption,dwAddData,font)    \
{CTRL_BUTTON,WS_CHILD|WS_VISIBLE |BS_AUTOCHECKBOX | WS_TABSTOP| WS_GROUP,\
	x,y,w,h,id,caption,dwAddData,WS_EX_TRANSPARENT,NULL,NULL,font}

#define EDIT(x,y,w,h,id,caption,dwAddData,font)  \
{CTRL_SLEDIT,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_CENTER| WS_TABSTOP,\
	x,y,w,h,id,caption,dwAddData,WS_EX_TRANSPARENT,NULL,NULL,font}

#define EDIT_L(x,y,w,h,id,caption,dwAddData,font)  \
{CTRL_SLEDIT,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_LEFT| WS_TABSTOP,\
	x,y,w,h,id,caption,dwAddData,0,NULL,NULL,font}

#define GROUP_EDIT_L(x,y,w,h,id,caption,dwAddData,font)  \
{CTRL_SLEDIT,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_LEFT| WS_TABSTOP | WS_GROUP,\
	x,y,w,h,id,caption,dwAddData,0,NULL,NULL,font}

#define EDIT_PSD_L(x,y,w,h,id,caption,dwAddData,font)  \
{CTRL_SLEDIT,WS_CHILD|ES_PASSWORD|WS_VISIBLE|WS_BORDER|ES_LEFT| WS_TABSTOP,\
	x,y,w,h,id,caption,dwAddData,0,NULL,NULL,font}

#define GROUP_EDIT_PSD_L(x,y,w,h,id,caption,dwAddData,font)  \
{CTRL_SLEDIT,WS_CHILD|ES_PASSWORD|WS_VISIBLE|WS_BORDER|ES_LEFT| WS_TABSTOP | WS_GROUP,\
	x,y,w,h,id,caption,dwAddData,0,NULL,NULL,font}

#define SCROLLVIEW(x,y,w,h,id,caption,dwAddData,font)  \
{CTRL_SCROLLVIEW,WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | SVS_AUTOSORT ,\
	x,y,w,h,id,caption,dwAddData,0,NULL,NULL,font}

#define LISTBOX(x,y,w,h,id,caption,dwAddData,font)  \
{CTRL_LISTBOX,WS_VISIBLE | WS_VSCROLL | WS_BORDER,\
	x,y,w,h,id,caption,dwAddData,0,NULL,NULL,font}

#define GROUP_COMBOBOX(x,y,w,h,id,caption,dwAddData,font)  \
{CTRL_COMBOBOX,WS_CHILD|WS_VISIBLE| WS_GROUP |\
	CBS_DROPDOWNLIST |CBS_READONLY | WS_TABSTOP,\
	x,y,w,h,id,caption,dwAddData,0,NULL,NULL,font}

#define COMBOBOX(x,y,w,h,id,caption,dwAddData,font)  \
{CTRL_COMBOBOX,WS_CHILD|WS_VISIBLE| \
	CBS_DROPDOWNLIST|CBS_READONLY | WS_TABSTOP ,\
	x,y,w,h,id,caption,dwAddData,0,NULL,NULL,font}

#define LISTVIEW(x,y,w,h,id,caption,dwAddData,font)  \
{CTRL_LISTVIEW,WS_CHILD|WS_VISIBLE| \
  WS_VSCROLL | WS_HSCROLL,\
	x,y,w,h,id,caption,dwAddData,0,NULL,NULL,font}

#define TREEVIEW(x,y,w,h,id,caption,dwAddData,font)  \
{CTRL_TREEVIEW,WS_BORDER|WS_CHILD|WS_VISIBLE| \
  WS_VSCROLL | WS_HSCROLL,\
	x,y,w,h,id,caption,dwAddData,0,NULL,NULL,font}

#define NELEMENTS(array)        /* number of elements in an array */ \
	(sizeof (array) / sizeof ((array) [0]))

#define TIME_1S (10 * 5)
#define TIME_100MS (TIME_1S / 10)

#define DBG_RC(x) printf("l:%d,t:%d,r:%d,b:%d\n",x.left,x.top,x.right,x.bottom)
#define DBG_STR(x)  printf("flag------->[%s]\n",x)
#define DBG_F(x) printf("flag[%s]------->[%d]\n",__FUNCTION__,x)

//#define ENTERFUNCTION printf("[%s]\n",__FUNCTION__);
//#define EXITFUNCTION printf("[%s]EXIT\n",__FUNCTION__);
#define ENTERFUNCTION
#define EXITFUNCTION
#define PRINTF( x... )  printf( x )
#define DBG_MAIN 				0
#define DBG_CALL_VIEW 			0
#define DBG_ICON_DLG			0
#define DBG_GET_PWD				0
#define DBG_PASSWORD			0
#define DBG_MENU				0
#define DBG_PUBLIC				0
#define DBG_INFO				0
#define DBG_MYMIXER				0
#define DBG_UPDATE				0
#define	DBG_MESSAGE				0
#define	DBG_MPEG4				0
#define	DBG_DOWN				0
#define DBG_SOCKETREAD 			0
#define DBG_SETTING 			0
#define DBG_SETTING_USER		0
#define DBG_NETSETTIN			0
#define DBG_SYS_PARA			0
#define DBG_UART_DEAL			0
#define DBG_FORM_MAIN			0
#define DBG_MACHINE				0
#define DBG_VIDEO_TRANS			0
#define DBG_TOPMSG				0

//-----------------------------------------------------------------------
#endif

