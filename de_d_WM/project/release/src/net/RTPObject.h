#ifndef TLINK_SIP

#ifndef RTPObjectH
#define RTPObjectH

#include "UDPClient.h"
//---------------------------------------------------------------------------

#ifndef FALSE
    #define FALSE 0
    #define TRUE 1
    #define BOOL int
#endif

#if (defined TC3000_BZ)
#define MAXENCODEPACKET (120*1024-16)
#else
#define MAXENCODEPACKET (60*1024-16)
#endif


typedef struct
{
#if (defined TC3000_BZ)
	unsigned int		packet_cnt;		//分包数量
	unsigned int		packet_size;	//分包大小
	unsigned int		packet_idx;		//包索引
	unsigned int		alen;			//audio长度
	unsigned int		atype;
	unsigned int  		tlen;			//数据长度
	unsigned int		dead;
	unsigned int		seq;			//帧序号
	unsigned int  		slen;			//第一帧长度
	unsigned int  		vtype;			//第一帧类型
	unsigned int		checksum;		// 校验和
	unsigned char	 	sdata[MAXENCODEPACKET];	//帧数据
#else
	unsigned int		packet_cnt;		//分包数量
	unsigned short		packet_size;	//分包大小
	unsigned short		packet_idx;		//包索引
	unsigned short		alen;			//audio长度
	unsigned short		atype;
	unsigned short  	tlen;			//数据长度
	unsigned short		dead;
	unsigned int		seq;			//帧序号
	unsigned short  	slen;			//第一帧长度
	unsigned short  	vtype;			//第一帧类型
	unsigned int		checksum;		// 校验和
	unsigned char	 	sdata[MAXENCODEPACKET];	//帧数据
#endif

}rec_body;

#define RECBODYSIZE ((unsigned int)sizeof(rec_body))
#define RECHEADSIZE ((unsigned int)(sizeof(rec_body)-MAXENCODEPACKET))

typedef struct _TRTPObject
{
	unsigned int SendPacketIdx;
	int Terminate;
	int RecvTimeOut;
	TUDPClient *udp;
	char cPeerIP[16];
	char cMasterDevIP[16];
	char ViceDeviceIP[16];
	char cServerIP[16];
	unsigned int dwPeerIP;
	unsigned int dwMasterDevIP;
	unsigned int dwWEBSrvIP;
	BOOL bRecvLocalPacket;			//是否接收到直传的包
	int PeerPort;
	int bInternet;		//是否基于互联网的传输
	int LostFramePrecent;	//丢包率
	int DelayTime;			//延迟时间
	int SendPacketCnt;		//发送统计

	void (* Destroy)(struct _TRTPObject *This);
	int (* RecvBuffer)(struct _TRTPObject *This,void *buf,int count,int TimeOut);
	int (* SendBuffer)(struct _TRTPObject *This,void *buf,int count);
	void (*SetPeerPort)(struct _TRTPObject *This,int PeerPort);
	int (*SendPortNumber)(struct _TRTPObject *This,const char *IP,int Port,
		const char *SrvIP,unsigned int UserID);
} TRTPObject;
//---------------------------------------------------------------------------

//  创建一个UDP客户端，Port为0则不绑定端口
TRTPObject* TRTPObject_Create(const char *PeerIP,char *LocalIP,const char *MasterDevIP,
							  const char *ViceDeviceIP,int PeerPort,const char *ServerIP);

#endif

#endif
