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
	unsigned int		packet_cnt;		//�ְ�����
	unsigned int		packet_size;	//�ְ���С
	unsigned int		packet_idx;		//������
	unsigned int		alen;			//audio����
	unsigned int		atype;
	unsigned int  		tlen;			//���ݳ���
	unsigned int		dead;
	unsigned int		seq;			//֡���
	unsigned int  		slen;			//��һ֡����
	unsigned int  		vtype;			//��һ֡����
	unsigned int		checksum;		// У���
	unsigned char	 	sdata[MAXENCODEPACKET];	//֡����
#else
	unsigned int		packet_cnt;		//�ְ�����
	unsigned short		packet_size;	//�ְ���С
	unsigned short		packet_idx;		//������
	unsigned short		alen;			//audio����
	unsigned short		atype;
	unsigned short  	tlen;			//���ݳ���
	unsigned short		dead;
	unsigned int		seq;			//֡���
	unsigned short  	slen;			//��һ֡����
	unsigned short  	vtype;			//��һ֡����
	unsigned int		checksum;		// У���
	unsigned char	 	sdata[MAXENCODEPACKET];	//֡����
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
	BOOL bRecvLocalPacket;			//�Ƿ���յ�ֱ���İ�
	int PeerPort;
	int bInternet;		//�Ƿ���ڻ������Ĵ���
	int LostFramePrecent;	//������
	int DelayTime;			//�ӳ�ʱ��
	int SendPacketCnt;		//����ͳ��

	void (* Destroy)(struct _TRTPObject *This);
	int (* RecvBuffer)(struct _TRTPObject *This,void *buf,int count,int TimeOut);
	int (* SendBuffer)(struct _TRTPObject *This,void *buf,int count);
	void (*SetPeerPort)(struct _TRTPObject *This,int PeerPort);
	int (*SendPortNumber)(struct _TRTPObject *This,const char *IP,int Port,
		const char *SrvIP,unsigned int UserID);
} TRTPObject;
//---------------------------------------------------------------------------

//  ����һ��UDP�ͻ��ˣ�PortΪ0�򲻰󶨶˿�
TRTPObject* TRTPObject_Create(const char *PeerIP,char *LocalIP,const char *MasterDevIP,
							  const char *ViceDeviceIP,int PeerPort,const char *ServerIP);

#endif

#endif
