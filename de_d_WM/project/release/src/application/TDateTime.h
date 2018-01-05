#ifndef TDATETIMEH
#define TDATETIMEH
#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
//		时间类,日期字符串与数值相互转互的类
//		数值 0 为 '1900-1-1 0:0:0'
//		用整数表示的Time值单位为1/300，也就是300为1秒
//////////////////////////////////////////////////////////////////////////////

#define _THIS struct _TDateTime *This
struct _TDateTimePriv;
typedef struct _TDateTime
{
    struct _TDateTimePriv *dtPriv;
    int (*Year)(_THIS);
    int (*Month)(_THIS);
    int (*Day)(_THIS);
    int (*Hour)(_THIS);
    int (*Minute)(_THIS);
    int (*Second)(_THIS);
    void (*Destroy)(_THIS);
    double (*GetValue)(_THIS);
	void (*SetValue)(_THIS,double Value);
	void (*GetDateTime)(_THIS,int *Date,int *Time);
	void (*SetDateTime)(_THIS,int Date,int Time);
    double (*SetString)(_THIS,char *pBuf);
    char* (*GetString)(_THIS,char *pBuf,int BufSize);
}TDateTime;
TDateTime *CreateDateTime(int Date,int Time);
#ifdef __cplusplus
}
#endif

#endif
