#ifndef ServicesDefH
#define ServicesDefH

#include <stddef.h>

#ifndef UINT32
#define UINT32 unsigned int
#endif
//����UDP�������ݰ���ʱֵ
#define UDPTIMEOUT 1000
//����UDP���͡���������ʱ���İ���,��������4
#define UDPMAXLENGTH 1000
//����UDP���û���յ�Ӧ���һ�����͵Ĵ���
#define TRYSENDTIMES 3

//UDPÿ����ĳ���
#define UDP_PACKET_MAX 1000		//UDP����󳤶�

#define UDPSERVER_PORT 7800		//Զ��UDP����˿ں�
#define UDPLOCAL_PORT  7800		//����UDP����˿ں�
#define	SQLSERVER_PORT 7890		//���ݷ�������SQL_SERVER����˿ں�(TCP)
#define RTPLISTEN_PORT 7891		//����
#define TCPSERVER_PORT 7892		//���ݷ�������TCP�ļ�����˿ں�
#define SQL_UDP_PORT   7896		//���ݷ����������Ӳ��Զ˿ں�
#define WAPTCP_PORT    7898		//WAP����������ͼ��˿�
#define TC_DNS_PORT    9888		//̫��DNS�������˿ں�
#define TC_WEB_PORT	   7895		//̫����վ��̨�������˿ں�

#define PROTOCOL_MAJOR   1
#define PROTOCOL_MINOR	 0
#define PROTOCOL_RELEASE 1
#define VERSION(a,b,c) (((a)<<16) | ((b)<<8) | (c))
/*---------------------------------------------------------------------------*/
enum {
	TP_CALL,			//���жԽ�
	TP_MAIL,			//�ʼ�֪ͨ
	TP_SIP,				//����
	TP_WAP,				//WAPͨѶ������
	TP_ALARM,			//��������
	TP_GPRS,			//GRPS���Ͷ���
	TP_PING,			//���Ӳ��ԣ�����
	TP_IPDOWN,			//IP��ַ���أ��ſڻ������ſڻ���
	TP_PICTURE,			//ͼƬ���أ��ſڻ���
	TP_AFFICHE,			//�ſڻ�������
	TP_GUARD,			//�ſڻ�������
	TP_GUARDCARD,		//�ſڻ�Ѳ������Ϣ
	TP_USERCARD,		//�ſڻ��û�ˢ��������Ϣ
	TP_DOWNMUSIC,		//����������������֪ͨ�ſڻ�
	TP_IMAGE,			//������������ͼƬ֪ͨ�ſڻ�
	TP_USERREFRESH,		//����
	TP_GETUSERIP,		//��ȡ�û�IP��ַ����TC-DNSͨѶ
	TP_VERIFYPWD,		//MD5����У��
	TP_MONITOR,			//Զ�̼����п���������
	TP_MONITOROVER,		//Զ�̼����п�������������
	TP_ELECTRIC,		//Զ�̵�����������
	TP_BURROW,			//Զ�̺��жԽ�ǰ·������͸����
	TP_CALLUSER,		//����
	TP_SETSYSTIME,		//�������ķ���ͬ��ʱ������  0x17(23d)
	TP_IDCARDDOWN,		//ID����Ϣ��������
	TP_READIP,			//�����ſڻ�IP��ַ����
	TP_USERIDRECORD,	//�ſڻ�ID����¼����
	TP_PUBLICMAIL,		//�ſڻ�����������Ϣ֪ͨ����
	TP_MEDIA,                //���������
	TP_DOWNANSWER,               //������ϢӦ��
	TP_COMPARELIST,               //�����ſڻ����Ŷ��ձ�  0x1e(30d)
	TP_MP3TIMECTRL,                  //����MP3�Ŀ���ʱ�䣬�Զ��رմ�ʱ��
	TP_SETSYSTEMAUDIO,  //��������  0x20(32d)
	TP_LOGMSG = 0x101,  //����������־ 20130709
	TP_TESTROUTE=0x200,	//·�ɻ��ز������������PING���������
	TP_NATBURROW,		//NAT·������͸����
	TP_RTPBURROW,		//RTP����Ƶ��͸����
	TP_DEVCHECK,		//�豸����
	TP_UPDATEPROC,		//�������֪ͨ
	TP_UPDATEEND,		//�������ݷ������ĸ�����ɵ���Ϣ
	TP_GETAPPVERSION,   //ȡ����汾��
	TP_ELEVATOR,        //��������
	TP_UPDATEMSG,		//������IP������Ϣ,�������ñ������Ϣ
	TP_GETREMOTEPWD,	//��ȡ���÷�����Զ������
	TP_SETREMOTEPWD,	//��ȡ���÷�����Զ������
	TP_SENDMACTOSRV,	//����MAC��ַ��IP��ַ��������Ϣ��������
	TP_CHANGEROOMID,	//���ķ���
	TP_TRANCMDBYSRV,	//ͨ��������ת����
	TP_TRANSRTP,		//����RTP��
	TP_TRANSELECTRIC,	//ת���͵������ư�
	TP_SENDSOFTVER,					//���ͱ��豸�汾��Ϣ
	TP_RETUPDATEMSG,				//����������Ϣ  0x211
	TP_BUTTONALARM,		//������ť����
	TP_LOCALDEVID,		//��ȡ����Ψһ���
	TP_ADVERTISEMENT,    //�пػ�����Զ�̹�淢���� Ϊ����U9�пػ����ݣ��Ÿ���ֵ
	TP_SYS_CONFIG=0x500,	//
	TP_ELEVATOR_3000=0x550,	//

	TP_VIDEO_CONFIG=0x600, 		//��Ƶ����ͷ��Ϣ
	TP_UNLOCK_INFO = 0x601, // ������Ϣ
	TP_GETDEVADDR=0x8000, 			//���豸��ַ��Ϣ
	TP_RESPDEVADDR,  				//�����豸��ַ��Ϣ
	TP_GETCONFIGADDR,				//��ȡ���õ�ַ
	TP_RESCONFIGADDR,				//�������õ�ַ
	TP_WORKREG,						//�豸ÿһ�����ڹ������ĵǼ�һ�ι���״̬
	TP_ONLINE,          			//���������Ļ��ſڻ����汾�豸����
	TP_DEVCTRLCOM,					//�����豸 ������������ػ� xiehb 20120613
	TP_WORKREG2,					//�豸ÿһ�����ڹ������ĵǼ�һ�ι���״̬(�°汾)
	TP_DOOROTHERPARA=0x8070,		//�����ͻ���������Э�飬ʵ�ֹ������Ķ��ſڻ��İ����������������ȹ��ܵĲ���
	TP_REQUESTVIDEO_OPEN = 0x20000,		//������Ƶ����Э��
	TP_REQUESTVIDEO_CLOSE = 0x20001,	//ֹͣ��Ƶ��������
	
	TP_NONE				//���������
};

//����ת���������
enum {
	CMD_SYS_SET_REQ,CMD_SYS_SET_REQ_REP,
	CMD_SYS_SET_DONE,CMD_SYS_SET_DONE_REP,
	CMD_SYS_SET_CANCEL,CMD_SYS_SET_CANCEL_REP,
	CMD_PWD_SET,CMD_PWD_SET_REP,
	CMD_IP_SET,CMD_IP_SET_REP,
	CMD_IP_GET,CMD_IP_GET_REP,
	CMD_DDNS_SET,CMD_DDNS_SET_REP,
	CMD_DDNS_GET,CMD_DDNS_GET_REP,
	CMD_DEV_SET,CMD_DEV_SET_REP,
	CMD_DEV_GET,CMD_DEV_GET_REP,
	CMD_IMEI_SET,CMD_IMEI_SET_REP,
	CMD_IMEI_GET,CMD_IMEI_GET_REP,
	CMD_ROOM_WRITE,CMD_ROOM_WRITE_REP,
	CMD_DEV_WRITE,CMD_DEV_WRITE_REP,
	CMD_RECORD_GET,CMD_RECORD_GET_REP,
	CMD_RF_FREQ_CHG, CMD_RF_FREQ_CHG_REP,
	CMD_RF_FREQ_GET, CMD_RF_FREQ_GET_REP,
	CMD_RF_DATA_TRAN, CMD_RF_DATA_TRAN_REP
};
/*---------------------------------------------------------------------------*/
/* ��ͷ */
typedef struct
{
    unsigned int ID;
    unsigned int Size;
    unsigned int Type;
}COMMUNICATION;
/*---------------------------------------------------------------------------*/
/* Ӧ��� */
typedef struct
{
    unsigned int ID;
}ASKPACKET;
/*---------------------------------------------------------------------------*/
/* �������� */

enum {CMD_CALL,		/* CMD_CALL ���� */
	CMD_ANSWER,		/* CMD_ANSWER ����������ʱ���� */
	CMD_TALK,		/* CMD_TALK ժ���������з����͸���Ϣ */
	CMD_OVER,		/* CMD_OVER �һ� */
	CMD_UNLOCK,		/* CMD_UNLOCK ���� */
	CMD_LEAVEWORD,	/* ���� */
	CMD_FORTIFY,	//
	CMD_REPEAL,		//
	CMD_PICTURE,	//
	CMD_TEXT,		//
	CMD_CLOSEAD,	//
	CMD_MUSICPLAY,	//
	CMD_MUSICSTOP,	//
	CMD_CLOSEMEDIA,
	CMD_SHAKEHANDS=0x80,		// ����ǰ��������
	ASW_OK=0x100,	//���лظ��ɹ�
	ASW_FAIL,		//���лظ�ʧ��
	MNT_OK,			//���ӳɹ�
	MNT_BUSY,		//����æ
	MNT_VERFAIL,	//���Ӽ���MD5ʧ��
	MNT_REFUSE,		//
	CMD_SENDPORT,	//
	CMD_TRANSCALL,	//ת����������
	CMD_TRANSOVER,	//ת�����н�������
	CMD_SHAKEHANDS_ASW=0x300,	// ����ǰ���ְ�Ӧ������
	CMD_REPLY_OK = 0x500,
	CMD_REPLY_FAIL = 0x501,
	CMD_REPLY_HUNGUP = 0x502,
};

/* call type */

enum {TYPE_CENTER,			//��������
	TYPE_DOOR,				//�ſڻ�
	TYPE_XC,				//����
	TYPE_USER,				//�п�����
	TYPE_REMOTE,			//����PC�ϵĺ��г�������վ�ĶԽ�����
	TYPE_XCDOOR,			//����
	TYPE_FDOOR,				//���ſڻ�
	TYPE_ZNBOX,				//������
	TYPE_NETRF,				//����RFģ��
	TYPE_DEVID_SRV=0x100,	//ȡ�豸ID������
	TYPE_ALL=0xFFFFFFFF		//�����豸
};
typedef struct
{
	unsigned char IP[16];			//����IP		
	unsigned int Port;				//�����˿�
	unsigned int Cmd;				//�ظ����Command
	unsigned short VideoWidth;			//��Ƶ����  0 320X240 1 352X288 2 640X480 3 720X576
	unsigned short VideoHeight;			//��Ƶ����  0 320X240 1 352X288 2 640X480 3 720X576
} REPLYVIDEODATAREQUEST;

typedef struct _COMMUNICATION_CALL
{
	unsigned int Cmd;					//Command
	char CallID[16];					//���з����
	char CallName[32];					//���з�����
	unsigned int CallType;				//Type
	unsigned int VideoType;			//��Ƶ���ͣ�0 H264, 1 Mpeg4
	unsigned short VideoWidth;			//��Ƶ����  0 320X240 1 352X288 2 640X480 3 720X576
	unsigned short VideoHeight;			//��Ƶ����  0 320X240 1 352X288 2 640X480 3 720X576
} COMMUNICATION_CALL;

typedef struct _OLD_COMMUNICATION_CALL
{
	unsigned int Cmd;					//Command
	char CallID[16];					//���з����
	char CallName[32];					//���з�����
	unsigned int CallType;				//Type
}OLD_COMMUNICATION_CALL;

typedef struct _COMMUNICATION_TRANSCALL
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int Cmd;					//Command
	char CallID[16];					//���з����
	char CallName[32];					//���з�����
	unsigned int CallType;				//Type
	char CallIP[16];					//���з�IP��ַ
	unsigned int CallPort;				//���з��˿ں�
}COMMUNICATION_TRANSCALL;
/*---------------------------------------------------------------------------*/
/* ����Ӧ�� */
typedef struct
{
	unsigned int Answer;
}ANSWER_CALL;
/*---------------------------------------------------------------------------*/
enum {WAP_GETIMAGE,WAP_GETSCENE,WAP_MACCONTROL,WAP_UNLOCK};
typedef struct
{
	unsigned int Cmd;					//Command
	unsigned int Type;					//Type
}COMMUNICATION_WAP;
//---------------------------------------------------------------------------
// Զ�̵�������
//---------------------------------------------------------------------------
enum {ELEC_GETDEVICE,		//���豸��Ϣ
	ELEC_RETDEVICE,			//�����豸��Ϣ
	ELEC_GETSTATUS,			//���豸״̬
	ELEC_RETSTATUS,			//�����豸״̬
	ELEC_CTRLDEVICE,		//�����豸
	ELEC_RETROOM,			//���ط�����Ϣ
	ELEC_GUARDMSG,			//������Ϣ
	ELEC_GUARDSTATE,		//�龰(������)״̬
};

//��ȡ�����豸��ϢЭ����ṹ
typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int NetID;
	unsigned char MD5Pwd[36];
	int Cmd;
	unsigned int GetType;			//��ȡ��Ϣ����,0���ͷ���ṹ���豸�ṹ,1���ͷ���ṹ,2�����豸�ṹ
	unsigned int PackIdx;			//0���а��ϴ�,N �ش��ڼ�����
	unsigned int Operate;			//���Ը��ֶ�
} TGetDeviceMsg;

//Զ�̵�����������Э���
typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int NetID;
	unsigned char MD5Pwd[36];
	int Cmd;
	unsigned int DevAddr;
	int DevUnit;
	unsigned int Operate;
} TRemoteCtrlDevice;

//Զ�̼�������Э���
typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned char MD5Pwd[36];
	int Way;
} TRemoteMonitor;

//�п��������ط�������Э���(�ɰ�)
typedef struct
{
	int RoomID;			//�����
	char RoomName[16];	//������
} TRemoRoomStruct;

typedef struct {
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int UserID;		//����ID���
	int Cmd;					//����
	unsigned short PacketCnt;	//�ְ�����
	unsigned short PacketIdx;	//��ǰ�ְ�����
	int RoomCnt;				//��ǰ���ں�������,-1MD5�������,-2�������������
	TRemoRoomStruct Rooms[1];		//�������ݣ���̬����
} TPacketRoomStruct;

//�п����������豸��Ϣ�ṹ��
typedef struct  {
	unsigned int DevID;			//�豸ID
	int DevUnit;				//�豸��Ԫ��(10101 1λ¥��,4λ��Ԫ��)
	int DevType;				//�豸����
	char DevName[16];			//�豸����
	unsigned short RoomID;		//����������
	unsigned short Status;		//״̬
}TRemoDeviceStatus;

//�п����������豸��ϢЭ���
typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int UserID;		//����ID���
	int Cmd;					//����
	unsigned short PacketCnt;	//�ܰ���
	unsigned short PacketIdx;	//������
	int DevCnt;					//���ں��豸����
	TRemoDeviceStatus Devs[1];
} TPacketDeviceStatus;

typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int NetID;
	int Cmd;
	unsigned int DevAddr;
	int DevUnit;
	int Status;
} TRetDevicePacket;

typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int NetID;
	int Cmd;                    //����,������ELEC_GUARDMSG
	char UserCode[16];          //�û����ⷿ��
	int TerminateType;          //�ն�����
	unsigned int DevID;         //�豸���
	unsigned int DevAddr;       //�豸��Ԫ��
	int DevUnit;                //�豸¥���
	char RoomName[24];          //������
	char DevName[36];           //�豸����
	unsigned int DevType;       //�豸����
	char cDevType[16];          //�豸����
	unsigned int Operate;       //����
	unsigned int Success;       //�����Ƿ�ɹ�
} TELECGUARDMSG;

typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int NetID;
	int Cmd;                    //����,������ELEC_GUARDMSG
	char UserCode[16];          //�û����ⷿ��
	int TerminateType;          //�ն�����
	int AlarmState;				//����״̬
	char AlarmContent[16];		//����������
} TELECGUARDSTATE;

//����Э����д�͸ʱ�İ��ṹ
typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int IP;
	unsigned int Port;
} TBURROW;

//RTP����Э����д�͸ʱ�İ��ṹ
typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int UserID;
	unsigned int IP;
	unsigned int Port;
	unsigned int Times;
} TRTPBURROW;

//�����豸��Ϣ������
enum {
	CMD_GETSTATUS,		//���豸��Ϣ
	CMD_RETSTATUS			//�����豸��Ϣ
};
typedef struct
{
	unsigned int ID;		//��ͷID��
	unsigned int Size;		//����С
	unsigned int Type;		//�����ͣ�����ΪTP_DEVCHECK
	unsigned int Cmd;		//����,����ΪCMD_GETSTATUS
	unsigned int DevType;	//�豸����
	char Addition[20];		//������Ϣ
} TGETDEVICEMSG;

//�����豸��Ϣ������
typedef struct
{
	unsigned int ID;		//��ͷID��
	unsigned int Size;		//����С
	unsigned int Type;		//�����ͣ�����ΪTP_DEVCHECK
	unsigned int Cmd;		//����,����ΪCMD_RETSTATUS
	unsigned int DevType;	//�豸����(����ֵ��ʽ����)
	char Code[16];			//�豸������
	char Name[32];			//�豸����
	char IP[16];			//�豸IP��ַ
	char dType[20];			//�豸����(���ַ���ʽ����)
	char IMEI[24];			//�豸�����
	char Addition[20];		//������Ϣ
} TRETDEVICEMSG;

//TC-DNS��ѯָ������ĵ�ַ
typedef struct {
	int ID;
	int Size;
	unsigned int Type;
	unsigned int UserID;
	unsigned char IMEI[24];
} TDNSQueryIP;
//TC-DNS���ز�ѯ��IP��ַ
typedef struct {
	int ID;
	int Size;
	unsigned int Type;
	unsigned int UserID;
	unsigned char IP[4];
	unsigned int Port;
	unsigned int Status;
} TDNSReturnIP;
//���ſڻ���д������Ϣ
typedef enum {
	IP_USER,
	IP_USERONE,           //�û�1IP
	IP_USERTWO,           //�û�2IP
	IP_USERTHREE,          //�û�3IP
	IP_USERFOUR,          //�û�4IP
	IP_DATACENTER,          //�û���������IP
	IP_MANAGECENTER,         //�û���������IP
	IP_DEVICE,           //�豸����IP
	IP_GATEWAY,           //����
	IP_SUBNETMASK,		//������������
	IP_NETCARMAC        //���������ַ
} HMKCODETYPE;
typedef enum {
	HMK_READWRITE=IP_USER,		//�������ſڻ�
	DMK_READWRITE=IP_DEVICE		//�������ſڻ�
} HMKCMDTYPE;


//����������
typedef struct {
	int ID;
	int Size;
	unsigned int Type;
	unsigned short Major;		//Э�����汾��
	unsigned char Minor;		//Э�鸱�汾��
	unsigned char Release;		//Э���޶���
	unsigned short PromptUpdate;	//�Ƿ���ʾ����
	unsigned short ForceUpdate;		//ǿ������
	unsigned short PromptReboot;	//�Ƿ���ʾ����
	unsigned char ForceReboot;		//ǿ������
	unsigned char RebootType;		//������ʽ 0Ӧ�ó���,1����ϵͳ
	char UpdateMsg[128];			//��������ʱ��ʾ������
} TUPDATEPACK;
//����������֪ͨ��
typedef struct {
	int ID;
	int Size;
	unsigned int Type;
	unsigned short Major;			//Э�����汾��
	unsigned char Minor;			//Э�鸱�汾��
	unsigned char Release;			//Э���޶���
	unsigned short AppMajor;		//�������汾��
	unsigned char AppMinor;			//���򸱰汾��
	unsigned char AppRelease;		//�����޸İ汾��
	char IMEI[20];					//����IMEI����
	char Code[16];					//�������
	char Name[32];					//��������
	char BuilderDate[32];			//��������ʱ��
} TUPDATEENDPACK;


//add by Jack
typedef struct _TVersion
{
	int Major;		//���汾
	int Minor;		//�Ӱ汾
	int Release;		//�ͷŰ汾
	int Reserve;		//����������
} TVersion;



//ȡ����汾�Ű�
typedef struct {
	int ID;
	int Size;
	unsigned int Type;
	unsigned short Major;			//Э�����汾��
	unsigned char Minor;			//Э�鸱�汾��
	unsigned char Release;			//Э���޶���
	unsigned short AppMajor;		//�������汾��
	unsigned char AppMinor;			//���򸱰汾��
	unsigned char AppRelease;		//�����޸İ汾��
} TGETAPPVERSION;

//���ݿ���Э��
#pragma pack(1)
typedef struct _ELEVATOR
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned char  head;
	unsigned char  AddrH;
	unsigned char  AddrL;
	unsigned char  Function;
	unsigned char  CellCode;
	unsigned char  FloorCode;
	unsigned char  Reserved;
	unsigned char  RoomNumber;
	unsigned char  Lrc;
	unsigned char  Cr;
	unsigned char  Lf;
}TElevator;
#pragma pack ()

typedef struct _TUpdateMsg
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int UpdateType;		//��������,1������IP����,2����������Ϣ�����
}TUpdateMsg;

typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int Cmd;		//0��ȡ����
}TGetRemotePwd;

typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int LocalID;
	unsigned int DevType;		//�豸����
}TSetRemotePwd;

typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int LocalID;
	unsigned int DevType;		//�豸����
	char Password[16];			//����
}TSetRemotePwdEx;

typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	char ESSID[32];
	unsigned char IP[16];
	unsigned char MAC[6];
	unsigned char QH;		//�ں�
	unsigned char DH;		//����
	unsigned char DRH;		//��Ԫ��
	unsigned char CH;		//���
	unsigned char FH;		//����
	unsigned char FJH;		//�ֻ���
} TSendMACToSrv;

//�����п���������
typedef struct
{
	unsigned int ID;		//PacketID++
	unsigned int Size;		//sizeof(TChangeRoomID)
	unsigned int Type;		//TP_CHANGEROOMID
	unsigned char QH;		//�ں�
	unsigned char DH;		//����
	unsigned char DRH;		//��Ԫ��
	unsigned char CH;		//���
	unsigned char FH;		//����
	unsigned char FJH;		//�ֻ���
} TChangeRoomID;

//�������ݵ�����ת����
typedef struct
{
	unsigned int ID;		//PacketID++
	unsigned int Size;		//sizeof(TSendNetTrans)
	unsigned int Type;		//TP_CHANGEROOMID
	unsigned short Cmd;		//����,CMD_RF_DATA_TRAN
	short Par;				//�������,CMD_RF_DATA_TRAN
	unsigned short Len;		//����������
	unsigned char Data[46];		//����,�ɱ䳤��
	unsigned short TTL;
	unsigned char IP[4];
	unsigned short Port;
	unsigned short SysCode;
	unsigned short SysPwd;
} TSendNetTrans;

typedef struct
{
	unsigned int ID;		//PacketID++
	unsigned int Size;		//sizeof(TRecvNetTrans)
	unsigned int Type;		//TP_CHANGEROOMID
	unsigned short Cmd;		//����
	short Par;				//�������,0:��������ͨ��,-1:�豸��ʱû�����ݴ���ͨ�����ÿ���,-2���豸�յ�������Ч����RF����
	unsigned short Len;		//����������
	unsigned char Data[46];		//����,�ɱ䳤��
} TRecvNetTrans;
//---------------------------------------------------------------------------
typedef struct
{
	unsigned int ID;		//PacketID++
	unsigned int Size;		//sizeof(TRecvNetTrans)
	unsigned int Type;		//TP_TRANCMDBYSRV/TP_TRANSRTP/TP_TRANSELECTRIC/
	unsigned int RecvIP;	//���շ�IP
	unsigned int RecvPort;	//���շ�Port
} TTransPacket;				//ͨ��������ת����ͷ
//---------------------------------------------------------------------------
//��Ƶͷ����Э�� xiehb 20131013
typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	char Reserve[64];
	unsigned int VideoHeadSize;
	char VideoHeadData[4];
} TRTPVideoHead;
//---------------------------------------------------------------------------

//add by Jack
typedef struct
{
	unsigned int ID;		//��ͷID��
	unsigned int Size;		//����С
	unsigned int Type;		//�����ͣ�����ΪTP_DEVCHECK
	unsigned int Cmd;		//����,����ΪCMD_GETSTATUS
	unsigned int DevType;	//�豸����
	char Addition[20];		//������Ϣ
} TGetDeviceInfo;

//�����豸��Ϣ������
typedef struct
{
	unsigned int ID;		//��ͷID��
	unsigned int Size;		//����С
	unsigned int Type;		//�����ͣ�����ΪTP_DEVCHECK
	unsigned int Cmd;		//����,����ΪCMD_RETSTATUS
	unsigned int DevType;	//�豸����(����ֵ��ʽ����)
	char Code[16];			//�豸������
	char Name[32];			//�豸����
	char IP[16];			//�豸IP��ַ
	char dType[20];			//�豸����(���ַ���ʽ����)
	char IMEI[24];			//�豸�����
	char Addition[20];		//������Ϣ
} TRetDeviceInfo;

typedef struct
{
	int year;
	int mon;
	int day;
	int hour;
	int min;
	int sec;
}SETSYSTIME;

typedef struct
{
	int KeyCode;
	char IP[16];
}CONFIGIP;

typedef struct {
	int ID;
	int Size;
	unsigned int Type;
	HMKCMDTYPE Cmd;			//
	int Cnt;
	CONFIGIP Cfg[0];
} THMKConfig;


typedef struct
{
    unsigned int ID;
    unsigned int Size;
    unsigned int Type;
	int Cmd;						//�豸��Ϣָ�� 1.����
	int Count;						//��������
	char Data[1];					//�����׵�ַ
}CFGDEVICE;

typedef struct
{
	char ip[64];			//�������ļ�
	char file[256];               //��ʾ��Ϣ
} TUpdateRevertFile;


enum
{
	REQSERVERINF,			// �ն������������ ID �����
	RESPONSESERVERINF		// ���������ն˷��� ID �����
}; //Cmd


typedef struct
{
	unsigned int ID;		//��ͷID��
	unsigned int Size;		//����С
	unsigned int Type;		//������
	unsigned int Cmd;		//����
	unsigned int ReqType;	//������Ϣ�豸����
	unsigned long long int DevID;            //�豸ID
	char Addition[32];		//������Ϣ
} TResDeviceInf;

typedef struct {
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int Proctol;             //Э��汾
	unsigned int DevType;           //�豸����
	unsigned int dwLoDevID;               //�豸���
	unsigned int dwHiDevID;
	char QH[8];                     //����
	char DH[8];                     //����
	char DYH[8];                    //��Ԫ��
	char CH[8];                     //���
	char FH[8];                     //����
	char FJH[8];                     //����
	char Name[32];					//�豸����
	char MAC[20];                   //�豸MAC��ַ
	char GateWay[16];               //�豸���ص�ַ
	unsigned short DoorOnLine;              //�ſڻ��Ƿ�����
	unsigned short CenterOnLine;            //�����Ƿ�����
	char DoorIP[4][16];             //�ſڻ�IP
	char CenterIP[4][16];           //����IP��ַ
	unsigned int ActiveTick;              //��������ʱ��
	char BuilderDate[32];			//�����������ʱ��
	TVersion HwVer;                 //Ӳ���汾��
	TVersion EBootVer;              //EBOOT�汾��
	TVersion KerVer;                //�ں˰汾��
	char AppVer[256];                //����汾��
	TVersion ResourceVer;           //��Դ�ļ��汾��
	// BOOL AllocRomote;               //�Ƿ�����Զ�̷���
	char RouteSSID[16];             //��ǰ���ӵ�·����SSID
	char RouteMac[16];              //·����MAC��ַ
	char CurrTime[20];              //�豸��ǰʱ��
	int BatState;                   //���״̬��BatState = (�Ƿ���<<16) | ��ѹ*100;
	unsigned int RootDiskSize;            //���ļ�ϵͳʣ��ռ䵥λKB
	unsigned int UserDiskSize;            //�û��ļ�ϵͳʣ��ռ䵥λKB
} TRespDevInfo2;
//�����������ſڻ�(D-H)���ͷ�������

// typedef struct
// {

	// char Code[24];	 //����
	// char IP[32];  	 //IP��ַ

// }TUserTable2;

//IP���ձ����ָ��
// enum{
	// IP_NULL,								//���κβ���
	// IP_DOWNLOAD,							//����
	// IP_READALL,								//��ȡȫ������
	// IP_READONE,								//��ȡһ���û�������
	// IP_DEL,									//ɾ��һ������,����Code����
	// IP_ADD,									//����һ�����ݣ�����Code����
	// IP_MOIDFIE,								//�޸�һ�����ݣ�����Code����
	// IP_DELALL,								//ɾ������IP ��Ϣ 20110228 xiehb
	// IP_DOWNLOAD2,							//�°汾���� 20130906
	// IP_OK=0x100,							//OKӦ��
	// IP_ERROR								//ʧ��Ӧ��
// };

// typedef struct
// {
	// unsigned int ID;		//��ͷID��
	// unsigned int Size;		//����С
	// unsigned int Type;		//������ TP_COMPARELIST
	// unsigned int Cmd;       //IP_DOWNLOAD2
	// unsigned int UserCount; //�����û���������
	// unsigned int PacketNum; //���ذ���
	// unsigned int Count;     //�ж������û�����
	// TUserTable2 Users[1];     //�û�����
// }TComparelistPack;

typedef struct
{
	unsigned int ID;		//��ͷID��
	unsigned int Size;		//����С
	unsigned int Type;		//������
	char Data[1];           //cmd + vol
}TSetVolumePack;


// �豸����������
enum
{
	DEVCTRL_IDLE, 		// ʲôҲû������ֹ���ŵ�
	DEVCTRL_RESET,		// ��λ
	DEVCTRL_POWERDOWN,  // �ػ�
};
// ��λ�������ݰ�
typedef struct {
	unsigned int ID;		//��ͷID��
	unsigned int Size;		//����С
	unsigned int Type;		//������
    unsigned int DevType;                 //�豸����
    unsigned int Command;                 //����
	char Addition[32];				//������Ϣ
}TDevCtrlCommand;

//------------------------------------------------------------------
// �ſڻ���Ϣ������������
enum {LOG_DR_DOOR, LOG_UR_ALARM, LOG_DR_LOWPOW};
typedef struct
{
    UINT32 ID;
    UINT32 Size;
    UINT32 Type;  //TP_LOGMSG
    UINT32 WarnLevel;         //���漶�� 0 һ��,��־  1 ���棬��Ҫ������ʾ
    UINT32 DevType;             //�豸����
    UINT32 LogType;             //��־����
    char   LogMessage[255];     //��־����
} TLogMessage;

// ���Э��
enum {
	AD_SEND,			//��淢��
	AD_RECV,			//��淢��Ӧ��
	AD_DELETE,			//ɾ��ĳ������ͼƬ,������ADAreaNo������
	AD_CLEAR,			//������й��ͼƬ
	AD_DEL_FILE,		//ɾ������ͼƬ
	AD_GET,             //��ȡ�ſڻ��Ѵ��ڵĹ��ͼƬ
	AD_GET_RESP,        //�����ſڻ��Ѵ��ڵ��ļ���
};

typedef struct 
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int UserID;
	char MD5Pwd[36];
	unsigned int Cmd;			//����
	unsigned int ADAreaNo;		//�������
	char ADSrv[52];				//��������
	char ADPath[96];			//���·��
	unsigned int FileSize;		//�ļ���С
} TSendAdver;

typedef struct 
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;
	unsigned int UserID;
	unsigned int Cmd;			//����
	unsigned int Status;		//״̬���ع�淢��״̬��0ʧ��  1�ɹ�
} TReponseAdver;

typedef struct
{
	unsigned int ADAreaNo;		//�������
	char ADFileName[32];        //����ļ���
} TADFileInfo;

typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;          //
	unsigned int UserID;
	unsigned int Cmd;			//���� AD_GET_RESP
	unsigned int FreeSpace;     //ʣ��ռ��С
	unsigned int PacketCnt;     //�ְܷ���
	unsigned int PacktMaxFiles; //һ�����������ļ�����
	unsigned int PacketIdx;     //��ǰ����
	unsigned int FileCnt;       //��ǰ�����ж��ٸ��ļ�
	TADFileInfo AdFiles[1];     //���ص��ļ��ṹ����         
} TReponseAdGet;
// ������¼�ϴ�
typedef struct
{
	UINT32 DevType;			// ���������
	UINT32 UnlockMode;		// ������ʽ: ˢ�������������ڻ��������������Ŀ��������뿪����
	UINT32 UnlockStatus;	// =0 ����ʧ�ܣ� =1�����ɹ�
	char   DevName[32];		// ��������Ž��豸����
	char   QH[4];			// �ں�
	char   DH[4];			// ����	
	char   DYH[4];			// ��Ԫ��
	char   Reserver[64]; 	// ����
}TUnlockRec;

typedef struct
{
	unsigned int ID;
	unsigned int Size;
	unsigned int Type;              //TP_CALLUSER  TP_CALLUSER2
	unsigned int Version;			//Э��汾��
   	unsigned int CallType;			//
   	unsigned int IsFDoor;			// 1 ���ſڻ� 0 ��Ԫ���ſڻ�
	unsigned int VideoType;			//��Ƶ���ͣ�0 H264, 1 Mpeg4
	unsigned short VideoWidth;		//��Ƶ����  320
	unsigned short VideoHeight;		//��Ƶ����  240
	char CALLIP[16];
	char Reserve[128];	
} TDoorCallMoniter;

typedef struct{
	unsigned int ID;					//��ͷID��
	unsigned int Size;					//����С
	unsigned int Type;					//������
	unsigned long long int ReqDevID;		//�豸ID
	unsigned	char	IP[16];			//�Է�IP
	unsigned	int 	CmdPort;		//ָ����ն˿�
	unsigned	int 	VideoPort;		//��Ƶ����˿�
	unsigned char reserved[4];  		//����
}VideoReqestData;

typedef struct{
	unsigned	int ID;						//��ͷID��
	unsigned	int Size;					//����С
	unsigned	int Type;					//������
	unsigned	int AckFlag;				//Ӧ���־	0����	1Ӧ��
	unsigned	int ReadWrite;				//��д��־	0����	1д��
	unsigned	int UseCardDisDef;			//ˢ��������־	1��Ч	0ʧЧ
	unsigned	int DisUnlockWhenMon;		//��ֹ������־	1��Ч	0ʧЧ
	unsigned 	int Reserve[64];  			//����
}TSetDefenseAndLock;


//------------------------------------------------------------------
#define MakeVerison(Ver,Major,Minor,Release) \
	(*(DWORD*)&Ver) = ((Major) | ((Minor)<<8) | ((Release)<<16))

#define CopyVerison(DestVer,SrcVer) \
	((*(DWORD*)&DestVer) = (*(DWORD*)&SrcVer))

#endif
