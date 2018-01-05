#ifndef PublicH
#define PublicH

#include "communication.h"
#include "UDPServer.h"
#define MAX_CENTERNUM	4	// 协议规定的管理中心数量
#define MAX_MKJNUM		9	// 协议规定的门口机数量8, 多留一个给户门口机

typedef struct _FORMCLASS
{
	HWND hWnd;
	char Class[16];
	struct _FORMCLASS * next;
}FormClass,*PFormClass;

typedef struct _SCREEN_FORM
{
    HWND hMainWnd;												//主窗口 
    HWND hUpdate;                                               //更新窗口
	HWND hAddressBook;											//通讯录窗口
	FILE *DrangLogfp;
	int Width;
	int Height;
	int Count;													//合计数
	PFormClass head,tail;										//窗口链表头与尾
	BOOL (*Add)(HWND hWnd,const char *Class);					//添加窗口
	BOOL (*Del)(HWND hWnd);										//删除窗口
	HWND (*Find)(const char *Class);							//查找窗口
	void (*ReturnMain)(void);										//返回主窗口
} SCREEN_FORM,*PSCREEN_FORM;
extern SCREEN_FORM Screen;

typedef struct
{
	char Name[32];
	char IP[16];
	int Type;				//类型,0门口机 1户门口机
} TServerMsg;

typedef struct _DisplayMsg
{
	int index;
	char buf[32];
} DisplayMsg;

enum {
	VEDIO_320x240,
	VEDIO_640x480,
	VEDIO_1280x720
};

typedef struct _EtcValueChar {
	const char* section;	
	const char* key;
	char *value;
	unsigned int leng;
	char *default_char;
} EtcValueChar;

typedef struct _EtcValueInt {
	const char* section;	
	const char* key;
	int *value;
	int default_int;
} EtcValueInt;

typedef struct _PUBLIC_DATA
{
	//程序版本信息
	TVersion KernelVer;				//内核版本号
	TVersion SoftVer;				//软件版本号
	TVersion m_MCUVer;				//单片机版本号
	char BuilderDate[24];			//本程序编译日期

	int language;			// 语言0中文，1英文	 xb_20150330
	BOOL bTransVideo;		// 是否传输本机视频图像
	BOOL bAlarm;			// 是否报警
	BOOL bAlarm_delay_ring;	// 是否布防延时响铃
	BOOL bUnbroken;			// 是否防拆报警
	BOOL bUnbrokenUse;		// 是否启用防拆
	BOOL bDoorBellUse;		// 是否启用门铃
	int LCDLight;			// LCD背光亮度
	int MaxScrSaversTimer;	// 进入屏幕保护的时间(秒)
	int ScrSaversTimer;		// 屏幕保护计数
	BOOL bEnableScrSavers;	// 能否进入屏保状态
	int TalkVolume;			//对讲音量
	int MaxTalkVolume;		// 最大对讲音量
	int CurrentVolume;		// 平时播放音量

	char MAC[20];			//本机MAC地址.
	char cLocalIP[16];		// 本机IP地址
	char cNetMask[16];		// 子网掩码
	char cLocalID[16];		// 申请的号码
	char cRoomID[16];       // 终端编号(门牌号)
	char cRoomName[32];		// 本机终端名称

	BOOL bGetDevID;					//是否获取设备唯一ID

	//房号信息
	unsigned long long int dwDevID;		//设备唯一号
	unsigned long long int dwDevIDTemp;	//临时设备唯一号

	TServerMsg *pCenterMsg; // 中心信息表
	int CenterCnt;			// 中心机数量
	char cGateWay[16];		// 默认网关
	char cInternetIP[16];	// 基于公网的服务器IP

	//全局变量
	BOOL IsDrangTest;				//是否进行压力测试

	char MasterDeviceIP[16];		//主机IP地址
	char ViceDeviceIP[16];			//从机IP地址

	int WatchDog_fd;
	BOOL bCallCenter;				//是否呼叫管理中心
	int CallRecord;					//呼叫记录 1为呼入 2为呼出
	int CallRecordView;				//呼叫记录 1为呼入 2为呼出	用于通话界面显示
	char CallRecordName[32];		//呼叫记录 名称
	BOOL Single_Mutex;				//互斥信号量，监视门口机与中心不能同时出现
	int VideoOverDlay;				//挂机延时，挂机3S后改变状态，防止视频未清除就再次呼入

	int CenterOnlineTime;			//管理中心在线时间
	BOOL CenterOnline;				//管理中心在线标志
	BOOL SyncTime;					//是否同步管理中心时间

	int MainWndMode; // 主界面模式 0待机，1按键与通话
	BOOL m_bSetStatus; // 是否设置状态 TRUE=设置 FALSE=非设置

	int EleVatorFloor;                  //电梯联动时室内机的房号
	int ElevatorAddr;                   //电梯地址
	int ElevatorFloorCode;              //电梯联动门口机的层号
	int ElevatorLobby;             		//电梯大堂号
	int eElevator;                      //电梯联动使能
	char UnlockPassword[16];			// 开锁密码
	char SystemPassword[20];			// 本机密码
	char mCallIP[20];					// 呼叫IP

	TServerMsg HomeList[13];			// 室内机信息
	int HomeListCnt;				// 室内机数量
	int FramPeriod;					// 帧率
	int AlarmDoorTime;				// 开门报警时间
	int EnableDoorPassword;		// 启用密码开锁
	int EnableAlarmDoor;			// 使能开门报警
	int VideoType;					// 视频分辨率,0:320x240,1:640x480,2:1280x720
	int AdvertisementType;			// 播放广告类型,0:文字,1:图片,2:视频
	int UnlockMode; 			// 开锁方式，上传管理中心
	int fTransmitToCenter;				//是否传输图像到中心

	int IsAllowUnLock;			//是否允许开锁		0:允许开锁	  1:禁止开锁
	int IsAllowDisarm;			//是否允许刷卡撤防	0:刷卡不撤防  1:刷卡撤防
	int IsDownLoadUseInfo;		//是否下载用户表信息
	char DefaultDmkjIp[16];		//栋门口机默认起始IP
	int Build_Num;				//楼栋号
	int Unit_Num;				//单元号
	
	void (*UnlockTheDoor)(int mode);
	int (*SendUdpData)(const char *IP, int Port,
		int Type,
		char *Data, unsigned int DataSize);

	int (*SendUdpTData)(const char *IP, int Port,
		int Type,
		char *Data, unsigned int DataSize,
		CallBackUDP Func,
		void *CallBackData);

	void (*LoadHomeList)(void);	//  x6机型用到更新每个按键对应IP
	void (*SaveIMEIandSetMac)(void);// 保存设备号并设置IP地址和MAC地址
	void (*GetDeviceIMEI)(void);// 发送获取设备编号协议
	void (*UpdateCenterIP)(char *db,char *ip,int ip_len); // 更新数据库管理中心IP
	void (*SendDoorOpenWarn)(int WarnLevel,
			int LogType,
			int STime,
			char *Msg, int MsgSize); // 发送开门报警状态到管理中心
	void (*RegisteToMng)(void) ; //每分种登记一次在线
	void (*PlayVoice)(char *text); // 播放语音
	void (*PlayVoiceWithBack)(char *text,void (*Proc)(void)); // 带回调函数的播放语音
	void (*ElevatorSndData)(TElevator *pElevator);
	int (*BroadCastFilter)(const char *SrcIP,const char *DstIP);
	void (*SendUnlockRec)(int status);
	void (*SendCallInfoToCenter)(void);
	void (*reSyncTime)(void); // 设置时间完成后立即重新同步管理中心
	void (*syncTime)(void);   // 同步管理中心时间
	void (*syncTimeHandle60s)(void); // 未同步时间时每分钟同步一次
 
} PUBLIC_DATA;


void publicInit(void);
void publicDestroy(void);

extern BOOL __mg_ime_minimize;
extern  PUBLIC_DATA Public;
extern unsigned int PacketID;

#define MSG_REFRESHSTATUS (MSG_USER+100)

#endif
