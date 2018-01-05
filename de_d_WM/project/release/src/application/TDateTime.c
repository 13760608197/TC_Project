#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "TDateTime.h"
#include "predefine.h"

//////////////////////////////////////////////////////////////////////////////
//		ʱ����,�����ַ�������ֵ�໥ת������
//		��ֵ 0 Ϊ '1900-1-1 0:0:0'
//		��������ʾ��Timeֵ��λΪ1/300��Ҳ����300Ϊ1��
//////////////////////////////////////////////////////////////////////////////

typedef struct _TDateTimePriv
{
    int Date;
	int Time;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
}TDateTimePriv;
static int MonthDays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
//---------------------------------------------------------------------------
static int IsLeapYear(int Year)
{
    if(Year%400==0 || (Year%4==0 && Year%100!=0))
        return 1;
    else
        return 0;
}
//---------------------------------------------------------------------------
static int TDateTime_GetYear(_THIS)
{
    return This->dtPriv->year;
}
//---------------------------------------------------------------------------
static int TDateTime_GetMonth(_THIS)
{
    return This->dtPriv->month;
}
//---------------------------------------------------------------------------
static int TDateTime_GetDay(_THIS)
{
    return This->dtPriv->day;
}
//---------------------------------------------------------------------------
static int TDateTime_GetHour(_THIS)
{
    return This->dtPriv->hour;
}
//---------------------------------------------------------------------------
static int TDateTime_GetMinute(_THIS)
{
    return This->dtPriv->minute;
}
//---------------------------------------------------------------------------
static int TDateTime_GetSecond(_THIS)
{
    return This->dtPriv->second;
}
//---------------------------------------------------------------------------
static double TDateTime_SetString(_THIS,char *pBuf)
{
    int y,M,d,h,m,s;
    int i,leap,days;
    char *pos;
    char Buf[20];

	if(pBuf==NULL) {
        return 0.0;
	}
    memset(Buf,0,sizeof(Buf));
    strncpy(Buf,pBuf,19);
    pBuf = Buf;
    //ȡ��
	if(pBuf[4]!='-') {
        return 0.0;
	}
    pBuf[4] = 0;
    y = atoi(pBuf);
	if(y==0) {
		return 0.0;
	}
    y = y - 1900;
    pBuf = &pBuf[5];
    //ȡ��
    pos = strchr(pBuf,'-');
	if(pos==NULL) {
        return 0.0;
	}
    *pos = 0;
    M = atoi(pBuf);
	if(M==0) {
		return 0.0;
	}
    pBuf = pos+1;
    //ȡ��
    pos = strchr(pBuf,' ');
	if(pos==NULL) {
        return 0.0;
	}
    *pos = 0;
    d = atoi(pBuf);
	if(d==0) {
		return 0.0;
	}
    pBuf = pos+1;
    //ȡʱ
    pos = strchr(pBuf,':');
	if(pos==NULL) {
        return 0.0;
	}
    *pos = 0;
    h = atoi(pBuf);
    pBuf = pos+1;
    //ȡ��
    pos = strchr(pBuf,':');
	if(pos==NULL) {
        return 0.0;
	}
    *pos = 0;
    m = atoi(pBuf);
    pBuf = pos+1;
    //ȡ��
    s = atoi(pBuf);

    //��������
    leap = y/4;
    leap-=(y/100);
    leap+=((y+300)/400);
    if(IsLeapYear(y)) {
        MonthDays[1] = 29;
        leap--;
    }
    days = y *365+leap;
	for(i=1;i<=M;i++) {
        days = days+MonthDays[M];
	}
    days+=d-1;

    //����ʱ��
    This->dtPriv->Date = days;
	This->dtPriv->Time = (((h*60)+m)*60+s)*300;
    This->dtPriv->year = 1900+y;
    This->dtPriv->month = M;
    This->dtPriv->day = d;
    This->dtPriv->hour = h;
    This->dtPriv->minute = m;
    This->dtPriv->second = s;
    return days+This->dtPriv->Time/25920000.0;
}
//---------------------------------------------------------------------------
static void TDateTime_Destroy(_THIS)
{
    if(This) {
        if(This->dtPriv)
            free(This->dtPriv);
        free(This);
    }
}
//---------------------------------------------------------------------------
static double TDateTime_GetValue(_THIS)
{
    return (double)This->dtPriv->Date+(This->dtPriv->Time/25920000.0);
}
//---------------------------------------------------------------------------
static void TDateTime_GetDateTime(_THIS,int *Date,int *Time)
{
	*Date = This->dtPriv->Date;
	*Time = This->dtPriv->Time;
}
//---------------------------------------------------------------------------
static void TDateTime_SetDateTime(_THIS,int Date,int Time)
{
    int y,M,d,h,m,s;
    int leap,l;
    int days = Date;
	int T;
    y = days / 365;
    leap = y/4;
    leap-=(y/100);
    leap+=((y+300)/400);
    if(IsLeapYear(1900+y)) {
        leap--;
        MonthDays[1] = 29;
    }
    if(days % 365<leap) y--;
    l = days-(y*365+leap);
	d = 0;
    for(M = 0;M<12;M++) {
        if(l<MonthDays[M]) {
            d = l;
            break;
        }
        l = l- MonthDays[M];
    }
    h = Time/1080000;
    T = Time % 1080000;
    m = T / 18000;
    T = T % 18000;
    s = T / 300;

	This->dtPriv->Date = Date;
	This->dtPriv->Time = Time;
    This->dtPriv->year = 1900+y;
    This->dtPriv->month = M+1;
    This->dtPriv->day = d+1;
    This->dtPriv->hour = h;
    This->dtPriv->minute = m;
    This->dtPriv->second = s;
}
//---------------------------------------------------------------------------
static void TDateTime_SetValue(_THIS,double Value)
{
	int Date = (int)Value;
	int Time = (int)((Value-Date)*25920000);
	TDateTime_SetDateTime(This,Date,Time);
}
//---------------------------------------------------------------------------
static char* TDateTime_GetString(_THIS,char *pBuf,int BufSize)
{
    if(BufSize<20)
        return NULL;
    sprintf(pBuf,"%04d-%02d-%02d %02d:%02d:%02d",This->dtPriv->year,This->dtPriv->month,This->dtPriv->day,
        This->dtPriv->hour,This->dtPriv->minute,This->dtPriv->second);
    return pBuf;
}
//---------------------------------------------------------------------------
TDateTime *CreateDateTime(int Date,int Time)
{
    TDateTime *This;
    This = (TDateTime *)malloc(sizeof(TDateTime));
    memset(This,0,sizeof(TDateTime));
    This->dtPriv = (TDateTimePriv*)malloc(sizeof(TDateTimePriv));
    memset(This->dtPriv,0,sizeof(TDateTimePriv));
    TDateTime_SetDateTime(This,Date,Time);
	This->Destroy = TDateTime_Destroy;
    This->Year = TDateTime_GetYear;
    This->Month = TDateTime_GetMonth;
    This->Day = TDateTime_GetDay;
    This->Hour = TDateTime_GetHour;
    This->Minute = TDateTime_GetMinute;
    This->Second = TDateTime_GetSecond;
    This->GetValue = TDateTime_GetValue;
	This->SetValue = TDateTime_SetValue;
    This->GetDateTime = TDateTime_GetDateTime;
	This->SetDateTime = TDateTime_SetDateTime;
    This->SetString = TDateTime_SetString;
    This->GetString = TDateTime_GetString;
    return This;
}
//---------------------------------------------------------------------------

