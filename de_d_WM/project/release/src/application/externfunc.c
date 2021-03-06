/*
 * =====================================================================================
 *
 *       Filename:  externfunc.c
 *
 *    Description:  外部函数
 *
 *        Version:  1.0
 *        Created:  2015-12-11 11:56:30
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
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include <strings.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <linux/rtc.h>
#include <linux/fb.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <sys/statfs.h>
#include <sys/mman.h>

#include "externfunc.h"

/* ----------------------------------------------------------------*
 *                  extern variables declare
 *-----------------------------------------------------------------*/
extern TUDPClient *udpclient;
extern TUDPServer *UdpServer;

/* ----------------------------------------------------------------*
 *                  internal functions declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                        macro define
 *-----------------------------------------------------------------*/
#define	IOCTL_PD	0x1001
#define	IOCTL_PU	0x1002
#define	IOCTL_NM	0x1003

/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/**
 * @brief WatchDogOpen 打开并初始化看门狗
 */
/* ---------------------------------------------------------------------------*/
void WatchDogOpen(void)
{
#ifndef WATCHDOG_DEBUG
	if(Public.WatchDog_fd > 0) {
		return;
	}

	Public.WatchDog_fd = open("/dev/watchdog", O_WRONLY);
	if (Public.WatchDog_fd == -1) {
		perror("watchdog");
	} else {
		printf("Init WatchDog!!!!!!!!!!!!!!!!!!\n");
	}
#endif
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief WatchDogFeed 喂狗函数
 */
/* ---------------------------------------------------------------------------*/
void WatchDogFeed(void)
{
#ifndef WATCHDOG_DEBUG
	if(Public.WatchDog_fd <= 0) {
		return;
	}
	ioctl(Public.WatchDog_fd, WDIOC_KEEPALIVE);
#endif
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief WatchDogClose 关闭看门狗
 */
/* ---------------------------------------------------------------------------*/
void WatchDogClose(void)
{
#ifndef WATCHDOG_DEBUG
	if(Public.WatchDog_fd <= 0) {
		return;
	}
	char * closestr="V";
	write(Public.WatchDog_fd,closestr,strlen(closestr));
	close(Public.WatchDog_fd);
	Public.WatchDog_fd = -2;
#endif
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief ErrorLog 记录错误日志
 *
 * @param ecode
 * @param fmt
 * @param ...
 */
/* ---------------------------------------------------------------------------*/
void ErrorLog(int ecode,const char *fmt,...)
{
	va_list fmtargs;
	va_start(fmtargs,fmt);
	vfprintf(stderr,fmt,fmtargs);
	va_end(fmtargs);

	fprintf(stderr,"\n");
	if(ecode) {
		fprintf(stderr,"*** Error cause: %s\n",strerror(ecode));
	}
}
//---------------------------------------------------------------------------
#ifndef WIN32
//windows 的GetLocalIP代码在capture.c中实现

int GetLocalIP(char *IP,char *NetMask,char *MAC)
{
	struct ifreq ifr;
	int sock;
	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return 0;
	}
	strcpy(ifr.ifr_name, "eth0");
	if(ioctl(sock, SIOCGIFADDR, &ifr) < 0)
		goto error;

	strcpy(IP,(char*)inet_ntoa(((struct sockaddr_in *)(&ifr.ifr_addr))->sin_addr));

	if (ioctl(sock, SIOCGIFNETMASK, &ifr) < 0)
		goto error;
	strcpy(NetMask,(char*)inet_ntoa(((struct sockaddr_in *)(&ifr.ifr_addr))->sin_addr));

	//获取MAC地址
	if (!(ioctl (sock, SIOCGIFHWADDR, &ifr))) {
		sprintf(MAC,"%02X:%02X:%02X:%02X:%02X:%02X",
			(unsigned char)ifr.ifr_hwaddr.sa_data[0],
			(unsigned char)ifr.ifr_hwaddr.sa_data[1],
			(unsigned char)ifr.ifr_hwaddr.sa_data[2],
			(unsigned char)ifr.ifr_hwaddr.sa_data[3],
			(unsigned char)ifr.ifr_hwaddr.sa_data[4],
			(unsigned char)ifr.ifr_hwaddr.sa_data[5]);
	}
   close(sock);
   return 1;
error:
	close(sock);
	return 0;
}
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
int PingServer(const char *IP)
{
	int Success;
	ASKPACKET ask;
	COMMUNICATION * pComm = (COMMUNICATION *)malloc(sizeof(COMMUNICATION));
	pComm->ID = PacketID++;
	pComm->Size = sizeof(COMMUNICATION);
	pComm->Type = TP_PING;
	udpclient->Clear(udpclient);
	udpclient->SendBuffer(udpclient,IP,SQL_UDP_PORT,pComm,pComm->Size);
	udpclient->SendBuffer(udpclient,IP,SQL_UDP_PORT,pComm,pComm->Size);
	Success = udpclient->RecvBuffer(udpclient,&ask,sizeof(ASKPACKET),1000,NULL,0);
	if(Success==sizeof(ASKPACKET) && ask.ID==pComm->ID) {
		free(pComm);
		return TRUE;
	}
	else {
		free(pComm);
		return FALSE;
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief GetDate 取当前日期时间格式
 *
 * @param cBuf
 * @param Size
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
char * GetDate(char *cBuf,int Size)
{
	time_t timer;
    struct tm *tm1;
	if(Size<20) {
		if(cBuf) cBuf[0]=0;
		return cBuf;
	}
	timer = time(&timer);
	tm1 = localtime(&timer);
	sprintf(cBuf,
			"%04d-%02d-%02d %02d:%02d:%02d",
			tm1->tm_year+1900,
			tm1->tm_mon+1,
			tm1->tm_mday,
			tm1->tm_hour,
			tm1->tm_min,
			tm1->tm_sec);
	return cBuf;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief GetMs 获得当前系统毫秒数
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
unsigned long long int GetMs(void)
{
	struct  timeval    tv;
    gettimeofday(&tv,NULL);
	return ((tv.tv_usec / 1000) + tv.tv_sec  * 1000 );

}

/* ---------------------------------------------------------------------------*/
/**
 * @brief fileexists 判断文件是否存在
 *
 * @param FileName 文件名
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
int fileexists(const char *FileName)
{
	int ret = access(FileName,0);
	return ret == 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief strupper 小写转大写
 *
 * @param pdst
 * @param pstr
 * @param Size
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
char *strupper(char *pdst,const char *pstr,int Size)
{
	char *pchar = pdst;
	if(pstr==NULL)
		return NULL;
	strncpy(pdst,pstr,Size);
	while(*pchar) {
		if(*pchar>='a' && *pchar<='z')
			*pchar = *pchar - 'a' + 'A';
		pchar++;
	}
	return pdst;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief DelayMs 线程使用 延时毫秒
 *
 * @param ms
 */
/* ---------------------------------------------------------------------------*/
void DelayMs(int ms)
{
#if 0
	unsigned long long start_time;
	start_time = GetMs();
	while (!(GetMs() - start_time >= ms)) ;
#else
	usleep(ms*1000);
#endif
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief adjustdate 设置系统时间
 *
 * @param year
 * @param mon
 * @param mday
 * @param hour
 * @param min
 * @param sec
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
int adjustdate(int year,int mon,int mday,int hour,int min,int sec)
{
	//设备系统时间
	int rtc;
	time_t t;
	struct tm nowtime;
	nowtime.tm_sec=sec;			/*   Seconds.[0-60]   (1   leap   second)*/
	nowtime.tm_min=min;			/*   Minutes.[0-59]		*/
	nowtime.tm_hour=hour;		/*   Hours. [0-23]		*/
	nowtime.tm_mday=mday;		/*   Day.[1-31]			*/
	nowtime.tm_mon=mon-1;		/*   Month. [0-11]		*/
	nowtime.tm_year=year-1900;	/*   Year-   1900.		*/
	nowtime.tm_isdst=-1;		/*   DST.[-1/0/1]		*/
	t=mktime(&nowtime);
	stime(&t);

	//设置实时时钟
	rtc = open("/dev/rtc0",O_WRONLY);
	if(rtc<0) {
		rtc = open("/dev/misc/rtc",O_WRONLY);
		if(rtc<0) {
			printf("can't open /dev/misc/rtc\n");
			return -1;
		}
	}
	if (ioctl( rtc, RTC_SET_TIME, &nowtime) < 0 ) {
		printf("Could not set the RTC time\n");
		close(rtc);
		return -1;
	}
	close(rtc);
	return 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief AdjustLCDLight 调节屏幕亮度
 *
 * @param Light亮度
 * @param force_off 1强制关屏
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
int AdjustLCDLight(int Light,int force_off)
{
#define IOCTL_LCD_BRIGHTNESS    _IOW('v', 27, unsigned int) //brightness control
    int hDev = open("/dev/fb0",O_RDONLY);
    int setLight;
    int min_light;
    if(hDev==-1) {
        printf("open device %s failed", "/dev/fb0");
        return -1;
    } else {
        if (force_off == 1) {
            min_light = 0;
        } else {
            min_light = MIN_LCD_LIGHT;
        }
        setLight = ((99.0 - min_light) / 99.0) * Light + min_light;
        ioctl(hDev, IOCTL_LCD_BRIGHTNESS, &setLight);
        close(hDev);
    }
    return 0;
}

//---------------------------------------------------------------------------
//
//返回
/* ---------------------------------------------------------------------------*/
/**
 * @brief screensaverStart 启动或关闭屏保
 *
 * @param On LCD_OFF关屏 LCD_ON点亮屏
 *
 * @returns TRUE=屏保模式  FALSE=退出屏保模式
 */
/* ---------------------------------------------------------------------------*/
BOOL screensaverStart(BOOL On)
{
	if(On) {// 点亮LCD，退出屏保模式
		AdjustLCDLight(Public.LCDLight,1);
		Public.ScrSaversTimer = 0;
		return FALSE;
	} else {
		AdjustLCDLight(10,1);
		return TRUE;
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief recoverData 恢复数据
 *
 * @param file 文件名称
 * @param reboot 1需要重启 0不需要重启
 */
/* ---------------------------------------------------------------------------*/
void recoverData(char *file,int reboot)
{
	int size = strlen(file);
	char *backfile = (char *) malloc (sizeof(char) * size + 5);
	sprintf(backfile,"%s_bak",file);
	ExcuteCmd(1,"cp",backfile,file,NULL);
	sync();
	if (reboot) {
		form_main->timerStart(IDC_TIMER_RECOVER);
	}
	free(backfile);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief backData   备份数据
 *
 * @param file
 */
/* ---------------------------------------------------------------------------*/
void backData(char *file)
{
	int size = strlen(file);
	char *backfile = (char *) malloc (sizeof(char) * size + 5);
	sprintf(backfile,"%s_bak",file);
	ExcuteCmd(1,"cp",file,backfile,NULL);
	sync();
	free(backfile);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief sqlCheck 检查数据库文件是否有效
 *
 * @param sql 数据库
 * @param file 文件名
 * @param reboot 是否需要重启
 *
 * @returns 1成功 0失败
 */
/* ---------------------------------------------------------------------------*/
int sqlCheck(TSqlite **sql,char *file,int reboot)
{
    TSqlite *tmp_sql = *sql;
	int ret1,ret2,ret3;
	if (tmp_sql == NULL) {
		goto sqlCheck_fail;
	}
	ret1 = LocalQueryOpen(tmp_sql,"select ID from IDCard limit 1");
	ret2 = LocalQueryOpen(tmp_sql,"select ID from IDCardRecord limit 1");
	ret3 = LocalQueryOpen(tmp_sql,"select ID from UserInfo limit 1");
	tmp_sql->Close(tmp_sql);

    if (ret1 == 1 && ret2 ==1 && ret3 == 1) {
		return TRUE;
    }
    printf("ret1:%d,ret2:%d,ret3:%d\n", ret1,ret2,ret3);
sqlCheck_fail:
	// if (reboot) {
		// recoverData(file,reboot);
	// } else {
		(*sql)->Destroy(*sql);
		// recoverData(file,reboot);
		recoverData(file,0);
		*sql = CreateLocalQuery(file);
	// }
	return FALSE;
}


/* ---------------------------------------------------------------------------*/
/**
 * @brief sqlLoad 加载数据库文件并判断是否成功，若不成功则恢复备份文件，重新加载
 *
 * @param sql 数据库文件
 * @param file 数据库文件名
 */
/* ---------------------------------------------------------------------------*/
void sqlLoad(TSqlite **sql,char *file)
{
	*sql = CreateLocalQuery(file);
	if (*sql) {
		printf("Create %s open successfully\n",file);
	} else {
		printf("Err:%s open failed\n",file);
		char file_bak[32];
		sprintf(file_bak,"%s_bak",file);
		if (fileexists(file_bak) == 1) {
			recoverData(file,0);
			*sql = CreateLocalQuery(file);
		}
	}    
	sqlCheck(&LocalQuery,"DataBase.db",1);
    if (DownData)
        DownData->reloadSql(DownData,LocalQuery);
    if (Card)
        Card->reloadSql(Card,LocalQuery);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief etcFileCheck 检查配置文件是否正确
 *
 * @param file 配置文件名称
 *
 * @returns 1成功 0失败
 */
/* ---------------------------------------------------------------------------*/
int etcFileCheck(char *file)
{
	int ret = TRUE;
	int hFile;
	int star_num,end_num;
	hFile = LoadEtcFile(file);

	if(hFile != ETC_FILENOTFOUND) {
		if(GetIntValueFromEtc(hFile,"START_CHECK","StartCheck",&star_num)!=ETC_OK) {
			ret = FALSE;
		}
		if(GetIntValueFromEtc(hFile,"END_CHECK","EndCheck",&end_num)!=ETC_OK) {
			ret = FALSE;
		}
		if ((star_num != INI_KEY_START) || (end_num != INI_KEY_END)) {
			ret = FALSE;
		}
		if (ret == FALSE) {
			recoverData(file,1);
		}
		UnloadEtcFile(hFile);
	} else {
		recoverData(file,1);
	}
	return ret;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief ExcuteCmd 执行shell命令,以NULL结尾
 *
 * @param bWait
 * @param Cmd
 * @param ...
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
int ExcuteCmd(int bWait,char *Cmd,...)
{
	int i;
	FILE *fp;
	int ret;
	va_list argp;
	char *argv;
	char commond[512] = {0};
	strcat(commond,Cmd);
	va_start( argp, Cmd);
	for(i=1;i<20;i++) {
		argv = va_arg(argp,char *);
		if(argv == NULL)
			break;
		strcat(commond," ");
		strcat(commond,argv);
	}
	va_end(argp);
	printf("cmd :%s\n",commond);
	if ((fp = popen(commond, "r") ) == 0) {
		perror("popen");
		return -1;
	}
	if ( (ret = pclose(fp)) == -1 ) {
		printf("close popen file pointer fp error!\n");
		// printf("popen ret is :%d\n", ret);
	}
	return ret;

	// 用此函数导致产生僵尸进程
	// system(commond);
	// return 0;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief GetSendIP 判断本地IP与目标IP是否同一个网段,
 * 如果不在同一个网段将返回最小的广播域
 *
 * @param pSrcIP 本地IP地址
 * @param pDestIP 目标IP地址
 * @param pMask 本机子网掩码
 *
 * @returns 最小的广播域IP地址
 */
/* ---------------------------------------------------------------------------*/
const char * GetSendIP(const char *pSrcIP,const char *pDestIP,const char *pMask)
{
	//取广播域
	static char cIP[16];
	unsigned int SrcIP,DestIP,Mask;
	unsigned char *pIP = (unsigned char *)&DestIP;
	//转换字符形IP到整形IP地址
	SrcIP = inet_addr(pSrcIP);
	DestIP = inet_addr(pDestIP);
	Mask = inet_addr(pMask);
	if((SrcIP & Mask)!=(DestIP & Mask)) {
		DestIP = (SrcIP & Mask) | (~Mask & 0xFFFFFFFF);//DestIP = 0xFFFFFFFF;
	}
	//转换成字符形IP地址
	sprintf(cIP,"%d.%d.%d.%d",pIP[0],pIP[1],pIP[2],pIP[3]);
	return cIP;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief jugdeRecIP 判断目标IP和本机IP是否在同一网段
 *
 * @param pSrcIP 本机IP
 * @param pDestIP 目标IP
 * @param pMask 本机掩码
 *
 * @returns 0不在同一网段 1在同一网段
 */
/* ---------------------------------------------------------------------------*/
int jugdeRecIP(const char *pSrcIP,const char *pDestIP,const char *pMask)
{
	unsigned int SrcIP,DestIP,Mask;
	//转换字符形IP到整形IP地址
	SrcIP = inet_addr(pSrcIP);
	DestIP = inet_addr(pDestIP);
	Mask = inet_addr(pMask);
	if((SrcIP & Mask)!=(DestIP & Mask)) 
		return 0;

	return 1;
}


/* ---------------------------------------------------------------------------*/
/**
 * @brief SetNetMac 根据系列号设置mac地址
 *
 * @param pImei 生产序列号
 * @param MAC 设置后的mac地址
 */
/* ---------------------------------------------------------------------------*/
void SetNetMac(unsigned char *pImei,char *MAC)
{
	unsigned char LastMacAddr; // 计算得到的Mac地址最后值
	unsigned char tmpbuf[3] = {0};
	char AddressBuf[20];

//	struct timeval tpstart;

	memset(AddressBuf, 0, sizeof(AddressBuf));
//  	gettimeofday(&tpstart,NULL);
//    srand(tpstart.tv_usec);

//	LastMacAddr = (1+(int) (255.0*rand()/(RAND_MAX+1.0)));
//	sprintf(&AddressBuf[0], "%02X:", LastMacAddr);

//	LastMacAddr = (1+(int) (255.0*rand()/(RAND_MAX+1.0)));
//	sprintf(&AddressBuf[3], "%02X:", LastMacAddr);

//	LastMacAddr = (1+(int) (255.0*rand()/(RAND_MAX+1.0)));
//	sprintf(&AddressBuf[6], "%02X:", LastMacAddr);

	memcpy(AddressBuf,MAC,sizeof(AddressBuf));

	LastMacAddr = pImei[0]^pImei[3];
	sprintf(tmpbuf, "%02X", LastMacAddr);
	AddressBuf[16] = tmpbuf[1];
	AddressBuf[15] = tmpbuf[0];

	LastMacAddr = pImei[1]^pImei[4];
	sprintf(tmpbuf, "%02X:", LastMacAddr);
	AddressBuf[14] = tmpbuf[2];
	AddressBuf[13] = tmpbuf[1];
	AddressBuf[12] = tmpbuf[0];

	LastMacAddr = pImei[2]^pImei[5];
	sprintf(tmpbuf, "%02X:", LastMacAddr);
	AddressBuf[11] = tmpbuf[2];
	AddressBuf[10] = tmpbuf[1];
	AddressBuf[9] = tmpbuf[0];

	memset(MAC,0,sizeof(AddressBuf));
	memcpy(MAC,AddressBuf,sizeof(AddressBuf));
}
//---------------------------------------------------------------------------
// flag =0 临时使用IP  =1正式使用IP
void SetNetwork(int flag,const char *cIp,const char *cMask,const char *cGateWay,const char *cMac)
{
	FILE *fp;
	char shfile[16];

	memset(shfile, 0, sizeof(shfile));
	if(flag == 1) // 正式文件
		strcpy(shfile,"./route.sh");
	else
		strcpy(shfile,"./route_temp.sh");
	fp = fopen(shfile,"wb");
	if(fp)
	{
		fprintf(fp,"#!/bin/sh\n");
		fprintf(fp,"/sbin/ifconfig eth0 down\n");
		if(cIp && cMask)
			fprintf(fp,"/sbin/ifconfig eth0 %s netmask %s\n",cIp,cMask);
		if(cMac)
			fprintf(fp,"/sbin/ifconfig eth0 hw ether %s\n",cMac);
		if(cGateWay)
			fprintf(fp,"/sbin/route add default gw %s dev eth0\n",cGateWay);
		fprintf(fp,"/sbin/ifconfig eth0 up\n");
		fclose(fp);
		fp = NULL;
		ExcuteCmd(1,shfile,NULL);
		sync();
	}
	else
		printf("Can't open %s\n", shfile);
	if(fp)
		fclose(fp);
}
//---------------------------------------------------------------------------
int SetValueToEtcFile_Ext(const char* pEtcFile,
                const char* pSection, const char* pKey, char* pValue)
{
	SetValueToEtcFile(pEtcFile,pSection,pKey,pValue);
	SetValueToEtcFile("interface.ini","System","Test1",pValue);
	if (etcFileCheck("interface.ini") == TRUE) {//检查配置文件并备份
	    backData("interface.ini");
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief GetFileSize 获得文件大小
 *
 * @param file	文件名
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
int GetFileSize(char *file)
{
	struct stat stat_buf;
	stat(file, &stat_buf) ;
	return stat_buf.st_size;
}

time_t MyGetTickCount(void)
{
	return time(NULL)*1000;
}

void talkLedControl(int status)
{
	if (status == 1) {
		// Mygpio->SetValueNow(Mygpio,ENUM_GPIO_CCD_POWER,IO_ACTIVE);
		Mygpio->SetValueNow(Mygpio,ENUM_GPIO_CCD_LED,IO_ACTIVE);
	} else {
		// Mygpio->SetValueNow(Mygpio,ENUM_GPIO_CCD_POWER,IO_INACTIVE);
		Mygpio->SetValueNow(Mygpio,ENUM_GPIO_CCD_LED,IO_INACTIVE);
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief GetFlashFreeSpace 获得flash剩余大小，单位为Byte
 *
 * @returns 
 */
/* ---------------------------------------------------------------------------*/
unsigned long long GetFlashFreeSpace(void)
{
	struct statfs diskInfo;
	unsigned long long freeDisk;

    if (statfs(FLASH_DIR,&diskInfo) < 0) {
		return -1;
	}
    freeDisk = diskInfo.f_bfree * diskInfo.f_bsize;
	return freeDisk;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief GetFlashFreeSpaceKB 获得flash剩余大小，单位为KB
 *
 * @returns 
 */
/* ---------------------------------------------------------------------------*/
unsigned long long GetFlashFreeSpaceKB(void)
{
	return GetFlashFreeSpace()/(unsigned long long)1024;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief GetFlashFreeSpaceMB 获得flash剩余大小，单位为MB
 *
 * @returns 
 */
/* ---------------------------------------------------------------------------*/
unsigned long long GetFlashFreeSpaceMB(void)
{
	return GetFlashFreeSpaceKB()/(unsigned long long)1024;
}


/* ---------------------------------------------------------------------------*/
/**
 * @brief net_detect 检测网线连接状态
 *
 * @param net_name 网卡名称
 *
 * @returns 0正常 -1不正常
 */
/* ---------------------------------------------------------------------------*/
int net_detect(char* net_name)
{
	int skfd = 0;
	struct ifreq ifr;
	struct sockaddr_in *pAddr = NULL;
	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd < 0) {
		printf("%s:%d Open socket error!\n", __FILE__, __LINE__);
		return -1;

	}
	strcpy(ifr.ifr_name, net_name);
	if(ioctl(skfd, SIOCGIFFLAGS, &ifr) <0 ) {
		close(skfd);
		printf("%s:%d IOCTL error!\n", __FILE__, __LINE__);
		printf("Maybe ethernet inferface %s is not valid!", ifr.ifr_name);
		return -1;

	}
	close(skfd);
	if(ifr.ifr_flags & IFF_RUNNING) {
		// printf("%s is running :)\n", ifr.ifr_name);
		return 0;
	} else {
		return -1;
		// printf("%s is not running :(\n", ifr.ifr_name);
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief print_data 格式化打印数据
 *
 * @param data 数据内容
 * @param len 长度
 */
/* ----------------------------------------------------------------*/
void print_data(char *data,int len)
{
	int i;
	for (i = 0; i < len; ++i) {
		if (i) {
			printf("[%02d]0x%02x ",i,data[i] );
			if ((i%5) == 0)
				printf("\n");
		} else {
			printf("\n");
		}
	}
	printf("\n");
}

/* ----------------------------------------------------------------*/
/**
 * @brief GetFilesNum 获取文件夹目录文件名及返回文件数目
 *
 * @param pPathDir 文件夹目录
 * @param func 每获得一个文件处理函数
 *
 * @returns 文件夹目录下的文件数目
 */
/* ----------------------------------------------------------------*/
int GetFilesNum(char *pPathDir,void (*func)(void *))
{
	int i=0 ;
	DIR *dir = NULL;
    struct dirent *dirp = NULL;
	struct _st_dir temp_file; // 保存文件名结构体
	if((dir=opendir(pPathDir)) == NULL) {
		printf("Open File %s Error %s\n",pPathDir,strerror(errno));
		return 0;
    }

	while((dirp=readdir(dir)) != NULL) {
		if ((strcmp(".",dirp->d_name) == 0) || (strcmp("..",dirp->d_name) == 0)) {
			continue;
		}
		i++;
		sprintf(temp_file.path,"%s/%s",pPathDir,dirp->d_name);
		if (func) {
			func(&temp_file);
		}
		// printf("i:%d,name:%s\n",i,dirp->d_name);
	}
	closedir(dir);
	return i;
}

/* ----------------------------------------------------------------*/
/**
 * @brief ClearFramebuffer 清除fb0
 */
/* ----------------------------------------------------------------*/
void ClearFramebuffer(void)
{
	int fd = -1;
	unsigned int VpostWidth=0, VpostHeight=0,VpostBpp=0;
	unsigned int g_fb_vaddress=0;
	unsigned int g_u32VpostBufMapSize=0;
	struct	fb_var_screeninfo g_fb_var;
	fd = open("/dev/fb0", O_RDWR);
	if (fd < 0) {
		perror("/dev/fb0");
		return;
	}
	if (ioctl(fd, FBIOGET_VSCREENINFO, &g_fb_var) < 0) {
		perror("/dev/fb0");
		close(fd);
		return;
	}
	VpostWidth = g_fb_var.xres;
	VpostHeight = g_fb_var.yres;
	VpostBpp = g_fb_var.bits_per_pixel/8;

	g_u32VpostBufMapSize = VpostWidth*VpostHeight*VpostBpp*2;

	g_fb_vaddress = (unsigned int)mmap( NULL, g_u32VpostBufMapSize,
			PROT_READ|PROT_WRITE,
			MAP_SHARED,
			fd,
			0);

	memset ((void*)g_fb_vaddress, 0x0, g_u32VpostBufMapSize );

	if (g_fb_vaddress)
		munmap((void *)g_fb_vaddress, g_u32VpostBufMapSize);

	close(fd);

}

/* ---------------------------------------------------------------------------*/
/**
 * @brief playButtonSound 播放按键声音
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
BOOL playButtonSound(void)
{
	if(video->getStatus(video) == ST_IDLE){
		video->setSilence(video,FALSE);
		playwavfile("res/wav/clicked.wav",NULL);
		// Public.PlayVoice("clicked");
	}
	return TRUE;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief JudgeMonth 判断当月多少天
 *
 * @param year 年份
 * @param month 月份
 *
 * @returns 天数
 */
/* ---------------------------------------------------------------------------*/
int JudgeMonth(int year,int month)
{
	if ((month == 1)
		|| (month == 3)
		|| (month == 5)
		|| (month == 7)
		|| (month == 8)
		|| (month == 10)
		|| (month == 12)) {
		return 31;	//大月
	} else if ((month == 4)
		|| (month == 6)
		|| (month == 9)
		|| (month == 11)) {
		return 30;
	} else if (month == 2) {
		if ((year % 4) == 0) {
			return 28;
		} else {
			return 29;
		}
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief hexToChar  16进制字符串转成n进制字符串
 *
 * @param num 待转换数
 * @param d_str 输出字符串
 * @param radix 进制(1-52)
 */
/* ---------------------------------------------------------------------------*/
void hexToChar( unsigned long long int num, char* d_str, unsigned int radix)
{
	const char a[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	char *ptr = d_str;
	if(num == 0) {
		*ptr++ = 0;
		*ptr = '\0';
		return;

	}

	while(num) {
		*ptr++ = a[num % (unsigned long long int)radix];
		num /= (unsigned long long int)radix;

	}

	*ptr = '\0';
	ptr--;
	char *start = d_str;
	while(start < ptr) {
		char tmp = *ptr;
		*ptr = *start;
		*start = tmp;
		ptr--;
		start++;

	}
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief CheckRoomIP  根据房间号计算对应IP
 *
 * @return	result结果
 */
/* ---------------------------------------------------------------------------*/

int CheckRoomIP(c_RoomInfo * this){
	WatchDogClose();
	int dong,dy,ceng,fh,fj,ip1 = 1,ip2 = 2,EndFlag = 0;
	for (dong=1; dong<=ROOM_BUILD_MAX; dong++) {
			for (dy=1; dy<=ROOM_UNIT_MAX; dy++) {
				for (ceng=1; ceng<=ROOM_FLOOR_MAX; ceng++) {
					for (fh=1; fh<=ROOM_ROOM_MAX; fh++) {
						for (fj=0; fj<=ROOM_ROOM_EXT_MAX; fj++) {
							if (fj == 0){							
								sprintf(this->master_ip,"%d.%d.%d.%d",ROOM_SEGMENT1,ROOM_SEGMENT2,ip1,ip2);
								if(ip2 != 2){
									ip2++;
									if (ip2 > 254) {
										ip2 = ip2 - 255 + 1;
										ip1++;
										if (ip1 > 254) {
		                                	printf("err ip %d.%d.%d.%d\n",ROOM_SEGMENT1,ROOM_SEGMENT2,ip1,ip2);
											WatchDogOpen();
											return 0;
										}
									}
								}
							}
							if ( dong == this->c_DH
									&& dy == this->c_DYH
									&& ceng == this->c_CH
									&& fh == this->c_FH
                                    && fj == this->c_FJH) {
								//确认本机IP
								sprintf(this->c_ip,"%d.%d.%d.%d",ROOM_SEGMENT1,ROOM_SEGMENT2,ip1,ip2);
								//确认户门口机IP
								if (fj == 0){
									ip2 += ROOM_HDOOR_MAX + 1;
								}
								else{	
									int tmp = 0;
									tmp = ROOM_ROOM_EXT_MAX - fj;
									ip2 += tmp + 1;
								}
								if (ip2 > 254) {
									ip2 = ip2 - 255 + 1;
									ip1++;
									if (ip1 > 254) {
	                                	printf("err ip %d.%d.%d.%d\n",ROOM_SEGMENT1,ROOM_SEGMENT2,ip1,ip2);
										WatchDogOpen();
										return 0;
									}
								}
								sprintf(this->houtdoor_ip,"%d.%d.%d.%d",ROOM_SEGMENT1,ROOM_SEGMENT2,ip1,ip2);
								goto cal_end;

							}
							ip2++;
							if (ip2 > 254) {
								ip2 = ip2 - 255 + 1;
								ip1++;
								if (ip1 > 254) {
                                	printf("err ip %d.%d.%d.%d\n",ROOM_SEGMENT1,ROOM_SEGMENT2,ip1,ip2);
									WatchDogOpen();
									return 0;
								}
							}
						}
					}
				}
			}
		}
cal_end:
    printf("master:[%s],local:[%s],houtdoor_ip:[%s]\n",this->master_ip,this->c_ip,this->houtdoor_ip);
	WatchDogOpen();
	return 1;
}


