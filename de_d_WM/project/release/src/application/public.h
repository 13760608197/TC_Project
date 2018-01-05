#ifndef PublicH
#define PublicH

#include "communication.h"
#include "UDPServer.h"
#define MAX_CENTERNUM	4	// Э��涨�Ĺ�����������
#define MAX_MKJNUM		9	// Э��涨���ſڻ�����8, ����һ�������ſڻ�

typedef struct _FORMCLASS
{
	HWND hWnd;
	char Class[16];
	struct _FORMCLASS * next;
}FormClass,*PFormClass;

typedef struct _SCREEN_FORM
{
    HWND hMainWnd;												//������ 
    HWND hUpdate;                                               //���´���
	HWND hAddressBook;											//ͨѶ¼����
	FILE *DrangLogfp;
	int Width;
	int Height;
	int Count;													//�ϼ���
	PFormClass head,tail;										//��������ͷ��β
	BOOL (*Add)(HWND hWnd,const char *Class);					//��Ӵ���
	BOOL (*Del)(HWND hWnd);										//ɾ������
	HWND (*Find)(const char *Class);							//���Ҵ���
	void (*ReturnMain)(void);										//����������
} SCREEN_FORM,*PSCREEN_FORM;
extern SCREEN_FORM Screen;

typedef struct
{
	char Name[32];
	char IP[16];
	int Type;				//����,0�ſڻ� 1���ſڻ�
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
	//����汾��Ϣ
	TVersion KernelVer;				//�ں˰汾��
	TVersion SoftVer;				//����汾��
	TVersion m_MCUVer;				//��Ƭ���汾��
	char BuilderDate[24];			//�������������

	int language;			// ����0���ģ�1Ӣ��	 xb_20150330
	BOOL bTransVideo;		// �Ƿ��䱾����Ƶͼ��
	BOOL bAlarm;			// �Ƿ񱨾�
	BOOL bAlarm_delay_ring;	// �Ƿ񲼷���ʱ����
	BOOL bUnbroken;			// �Ƿ���𱨾�
	BOOL bUnbrokenUse;		// �Ƿ����÷���
	BOOL bDoorBellUse;		// �Ƿ���������
	int LCDLight;			// LCD��������
	int MaxScrSaversTimer;	// ������Ļ������ʱ��(��)
	int ScrSaversTimer;		// ��Ļ��������
	BOOL bEnableScrSavers;	// �ܷ��������״̬
	int TalkVolume;			//�Խ�����
	int MaxTalkVolume;		// ���Խ�����
	int CurrentVolume;		// ƽʱ��������

	char MAC[20];			//����MAC��ַ.
	char cLocalIP[16];		// ����IP��ַ
	char cNetMask[16];		// ��������
	char cLocalID[16];		// ����ĺ���
	char cRoomID[16];       // �ն˱��(���ƺ�)
	char cRoomName[32];		// �����ն�����

	BOOL bGetDevID;					//�Ƿ��ȡ�豸ΨһID

	//������Ϣ
	unsigned long long int dwDevID;		//�豸Ψһ��
	unsigned long long int dwDevIDTemp;	//��ʱ�豸Ψһ��

	TServerMsg *pCenterMsg; // ������Ϣ��
	int CenterCnt;			// ���Ļ�����
	char cGateWay[16];		// Ĭ������
	char cInternetIP[16];	// ���ڹ����ķ�����IP

	//ȫ�ֱ���
	BOOL IsDrangTest;				//�Ƿ����ѹ������

	char MasterDeviceIP[16];		//����IP��ַ
	char ViceDeviceIP[16];			//�ӻ�IP��ַ

	int WatchDog_fd;
	BOOL bCallCenter;				//�Ƿ���й�������
	int CallRecord;					//���м�¼ 1Ϊ���� 2Ϊ����
	int CallRecordView;				//���м�¼ 1Ϊ���� 2Ϊ����	����ͨ��������ʾ
	char CallRecordName[32];		//���м�¼ ����
	BOOL Single_Mutex;				//�����ź����������ſڻ������Ĳ���ͬʱ����
	int VideoOverDlay;				//�һ���ʱ���һ�3S��ı�״̬����ֹ��Ƶδ������ٴκ���

	int CenterOnlineTime;			//������������ʱ��
	BOOL CenterOnline;				//�����������߱�־
	BOOL SyncTime;					//�Ƿ�ͬ����������ʱ��

	int MainWndMode; // ������ģʽ 0������1������ͨ��
	BOOL m_bSetStatus; // �Ƿ�����״̬ TRUE=���� FALSE=������

	int EleVatorFloor;                  //��������ʱ���ڻ��ķ���
	int ElevatorAddr;                   //���ݵ�ַ
	int ElevatorFloorCode;              //���������ſڻ��Ĳ��
	int ElevatorLobby;             		//���ݴ��ú�
	int eElevator;                      //��������ʹ��
	char UnlockPassword[16];			// ��������
	char SystemPassword[20];			// ��������
	char mCallIP[20];					// ����IP

	TServerMsg HomeList[13];			// ���ڻ���Ϣ
	int HomeListCnt;				// ���ڻ�����
	int FramPeriod;					// ֡��
	int AlarmDoorTime;				// ���ű���ʱ��
	int EnableDoorPassword;		// �������뿪��
	int EnableAlarmDoor;			// ʹ�ܿ��ű���
	int VideoType;					// ��Ƶ�ֱ���,0:320x240,1:640x480,2:1280x720
	int AdvertisementType;			// ���Ź������,0:����,1:ͼƬ,2:��Ƶ
	int UnlockMode; 			// ������ʽ���ϴ���������
	int fTransmitToCenter;				//�Ƿ���ͼ������

	int IsAllowUnLock;			//�Ƿ�������		0:������	  1:��ֹ����
	int IsAllowDisarm;			//�Ƿ�����ˢ������	0:ˢ��������  1:ˢ������
	int IsDownLoadUseInfo;		//�Ƿ������û�����Ϣ
	char DefaultDmkjIp[16];		//���ſڻ�Ĭ����ʼIP
	int Build_Num;				//¥����
	int Unit_Num;				//��Ԫ��
	
	void (*UnlockTheDoor)(int mode);
	int (*SendUdpData)(const char *IP, int Port,
		int Type,
		char *Data, unsigned int DataSize);

	int (*SendUdpTData)(const char *IP, int Port,
		int Type,
		char *Data, unsigned int DataSize,
		CallBackUDP Func,
		void *CallBackData);

	void (*LoadHomeList)(void);	//  x6�����õ�����ÿ��������ӦIP
	void (*SaveIMEIandSetMac)(void);// �����豸�Ų�����IP��ַ��MAC��ַ
	void (*GetDeviceIMEI)(void);// ���ͻ�ȡ�豸���Э��
	void (*UpdateCenterIP)(char *db,char *ip,int ip_len); // �������ݿ��������IP
	void (*SendDoorOpenWarn)(int WarnLevel,
			int LogType,
			int STime,
			char *Msg, int MsgSize); // ���Ϳ��ű���״̬����������
	void (*RegisteToMng)(void) ; //ÿ���ֵǼ�һ������
	void (*PlayVoice)(char *text); // ��������
	void (*PlayVoiceWithBack)(char *text,void (*Proc)(void)); // ���ص������Ĳ�������
	void (*ElevatorSndData)(TElevator *pElevator);
	int (*BroadCastFilter)(const char *SrcIP,const char *DstIP);
	void (*SendUnlockRec)(int status);
	void (*SendCallInfoToCenter)(void);
	void (*reSyncTime)(void); // ����ʱ����ɺ���������ͬ����������
	void (*syncTime)(void);   // ͬ����������ʱ��
	void (*syncTimeHandle60s)(void); // δͬ��ʱ��ʱÿ����ͬ��һ��
 
} PUBLIC_DATA;


void publicInit(void);
void publicDestroy(void);

extern BOOL __mg_ime_minimize;
extern  PUBLIC_DATA Public;
extern unsigned int PacketID;

#define MSG_REFRESHSTATUS (MSG_USER+100)

#endif
