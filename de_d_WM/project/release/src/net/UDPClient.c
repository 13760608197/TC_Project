#ifndef WIN32

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#include "UDPClient.h"
#include "predefine.h"
#include "communication.h"
//---------------------------------------------------------------------------
static void TUDPClient_Destroy(TUDPClient *This)
{
	if(This->m_socket>0)
		close(This->m_socket);
    free(This);
	This = NULL;
}
//---------------------------------------------------------------------------
void TUDPClient_ClearBuffer(TUDPClient *This)
{
    char cBuf[1000];
    while(This->RecvBuffer(This,cBuf,1000,0,0,0)>0);
}
//---------------------------------------------------------------------------
static int TUDPClient_SendBuffer(TUDPClient *This,const char *IP,int port,const void *pBuf,int size)
{
	struct sockaddr_in *p;
	struct sockaddr addr;
	struct hostent *hostaddr;
	if(This->m_socket<0)
		return -1;
	if(IP[0]==0 || port==0)
		return -2;
	memset(&addr,0,sizeof(addr));
	p=(struct sockaddr_in *)&addr;
	p->sin_family=AF_INET;
	p->sin_port=htons(port);
	if(IP[0]<'0' || IP[0]>'9') {
		hostaddr = gethostbyname(IP);
		if(!hostaddr)
		{
			printf("Parse address %s fail!\n",IP);
			return -3;
		}
		memcpy(&p->sin_addr,hostaddr->h_addr,hostaddr->h_length);
	} else {
		p->sin_addr.s_addr = inet_addr(IP);
		if((p->sin_addr.s_addr == INADDR_NONE) && (strcmp(IP,"255.255.255.255")!=0))
		{
			printf("IP Address %s Error\n",IP);
			return -4;
		}
	}
	return sendto(This->m_socket,(char*)pBuf,size,MSG_NOSIGNAL,&addr,sizeof(struct sockaddr_in));
}
//---------------------------------------------------------------------------
// �ɿ�����,Ҫ�󷵻���,ʧ���ط�
//---------------------------------------------------------------------------
static int TUDPClient_TrusthSend(TUDPClient *This,const char *IP,int port,const void *pBuf,int size)
{
    int RecvSize;
	fd_set fdR;
	struct timeval timeout;
    ASKPACKET ask;
    int TrySend = TRYSENDTIMES;     //ʧ�����Դ���

    if(This->m_socket<0)
        return 0;

	This->Clear(This);				//���������

	while(TrySend)
    {
        if(TUDPClient_SendBuffer(This,IP,port,pBuf,size)!=size)
            return 0;       //����ʧ��

		FD_ZERO(&fdR);
		FD_SET(This->m_socket, &fdR);
        timeout.tv_sec=UDPTIMEOUT/1000;
        timeout.tv_usec=(UDPTIMEOUT%1000)*1000;
        if(select(This->m_socket+1, &fdR,NULL, NULL, &timeout)<=0 || !FD_ISSET(This->m_socket,&fdR))
        {   //���ͳ�ʱ,�ط�
            if(--TrySend == 0)
                return 0;
            continue;
        }
		RecvSize = recvfrom(This->m_socket,(char*)&ask,sizeof(ask),MSG_NOSIGNAL,0,0);
		if(RecvSize!=sizeof(ask) || ask.ID!=*(unsigned int *)pBuf)
		{
			//�������ݲ��ԣ��ط�
			if(--TrySend == 0)
				return 0;
			continue;
		}
		break;
    }
    return 1;
}
//---------------------------------------------------------------------------
static int TUDPClient_RecvBuffer(TUDPClient *This,void *pBuf,int size,int TimeOut,
    void *from,int * fromlen)
{
	struct timeval timeout;
	fd_set fdR;

	if(This->m_socket<0)
		return -1;
    if(TimeOut<0)
    	return recvfrom(This->m_socket,(char*)pBuf,size,MSG_NOSIGNAL,(struct sockaddr *)from,fromlen);
    else
    {
		FD_ZERO(&fdR);
		FD_SET(This->m_socket, &fdR);
        timeout.tv_sec=TimeOut / 1000;
        timeout.tv_usec=(TimeOut % 1000)<<10;
        if(select(This->m_socket+1, &fdR,NULL, NULL, &timeout)<=0 || !FD_ISSET(This->m_socket,&fdR))
            return -1;
    	return recvfrom(This->m_socket,(char*)pBuf,size,MSG_NOSIGNAL,(struct sockaddr *)from,fromlen);
    }
}
//---------------------------------------------------------------------------
// �������ݺ���Ӧ��
//---------------------------------------------------------------------------
static int TUDPClient_TrusthRecv(TUDPClient *This,void *pBuf,int size,int TimeOut)
{
	struct sockaddr fromip;
	int fromlen = sizeof(struct sockaddr);
	int RetSize;

	RetSize = TUDPClient_RecvBuffer(This,pBuf,size,TimeOut,&fromip,&fromlen);
	if(RetSize>sizeof(COMMUNICATION)) {
		sendto(This->m_socket,pBuf,4,0,&fromip,sizeof(fromip));
	}
	return RetSize;
}
//---------------------------------------------------------------------------
//  ����һ��UDP�ͻ��ˣ�PortΪ0�򲻰󶨶˿�
//---------------------------------------------------------------------------
TUDPClient* TUDPClient_Create(int Port)
{
	int ret;
    TUDPClient* This;
	struct sockaddr_in local_addr;
	int   opt=1;

    This = (TUDPClient *)malloc(sizeof(TUDPClient));
    if(This==NULL) {
        PRINTF("alloc UDPClient memory failt!\n");
        return NULL;
    }
	memset(&local_addr,0,sizeof(local_addr));
	This->m_socket=socket(AF_INET,SOCK_DGRAM,0);
	if(This->m_socket<0) {
		PRINTF("UDP Client init socket failed!\n");
        free(This);
        return NULL;
    }
	ret = setsockopt(This->m_socket,SOL_SOCKET,SO_BROADCAST,(char*)&opt,sizeof(opt));
	if(ret!=0) {
		printf("setsockopt error %d\n",ret);
	}

    if(Port)
    {
    	local_addr.sin_family = AF_INET;
	    local_addr.sin_port = htons(Port);
    	local_addr.sin_addr.s_addr = INADDR_ANY;
    	if(bind(This->m_socket, (struct sockaddr *)&local_addr, sizeof(struct sockaddr))<0)
        {
            PRINTF("UDP client bind to port failed!\n");
            close(This->m_socket);
            free(This);
            return NULL;
        }
    }
    This->Destroy = TUDPClient_Destroy;
	This->Clear = TUDPClient_ClearBuffer;
    This->RecvBuffer = TUDPClient_RecvBuffer;
    This->SendBuffer = TUDPClient_SendBuffer;
    This->TrusthSend = TUDPClient_TrusthSend;
	This->TrusthRecv = TUDPClient_TrusthRecv;

    return This;
}
//---------------------------------------------------------------------------
















//////////////////////////////////////////////////////////////////////////////
#else
//---------------------------------------------------------------------------
//  WINDOWS ������
//---------------------------------------------------------------------------
#include <stdio.h>
#include <Winsock2.h>
#include "UDPClient.h"
#include "predefine.h"
#include "communication.h"

static void TUDPClient_Destroy(TUDPClient *This)
{
	if(This->m_socket>0)
		closesocket(This->m_socket);
    free(This);
    WSACleanup();
}
//---------------------------------------------------------------------------
void TUDPClient_ClearBuffer(TUDPClient *This)
{
    char cBuf[1000];
    while(This->RecvBuffer(This,cBuf,1000,0,0,0)>0);
}
//---------------------------------------------------------------------------
static int TUDPClient_SendBuffer(TUDPClient *This,const char *IP,int port,const void *pBuf,int size)
{
	struct sockaddr_in *p;
	struct sockaddr addr;
	struct hostent *hostaddr;
	if(This->m_socket<=0)
		return -1;
	if(IP[0]==0 || port==0)
		return -2;
	memset(&addr,0,sizeof(addr));
	p=(struct sockaddr_in *)&addr;
	p->sin_family=AF_INET;
	p->sin_port=htons(port);
	if(IP[0]<'0' || IP[0]>'9') {
		hostaddr = gethostbyname(IP);
		if(!hostaddr)
		{
			return -3;
		}
		memcpy(&p->sin_addr,hostaddr->h_addr,hostaddr->h_length);
	} else {
		p->sin_addr.s_addr = inet_addr(IP);
		if((p->sin_addr.s_addr == INADDR_NONE) && (strcmp(IP,"255.255.255.255")!=0))
		{
			return -4;
		}
	}
	return sendto(This->m_socket,(char*)pBuf,size,0,&addr,sizeof(struct sockaddr_in));
}
//---------------------------------------------------------------------------
// �ɿ�����,Ҫ�󷵻���,ʧ���ط�
//---------------------------------------------------------------------------
static int TUDPClient_TrusthSend(TUDPClient *This,const char *IP,int port,const void *pBuf,int size)
{
    int RecvSize;
	fd_set fdR;
    ASKPACKET ask;
	struct timeval timeout;
    int TrySend = TRYSENDTIMES;     //ʧ�����Դ���

    if(This->m_socket==0)
        return 0;

	This->Clear(This);				//���������

    while(TrySend)
    {
        if(TUDPClient_SendBuffer(This,IP,port,pBuf,size)!=size)
            return 0;       //����ʧ��

        FD_ZERO(&fdR);
        FD_SET(This->m_socket,&fdR);

        timeout.tv_sec=UDPTIMEOUT / 1000;
        timeout.tv_usec=(UDPTIMEOUT % 1000)*1000;
        if(select(0, &fdR,NULL, NULL, &timeout)<=0)
        {   //���ͳ�ʱ,�ط�
            if(--TrySend == 0)
                return 0;
            continue;
        }
        if(!FD_ISSET(This->m_socket, &fdR))
        {
            if(--TrySend == 0)
                return 0;
            continue;
        }
        RecvSize = recvfrom(This->m_socket,(char*)&ask,sizeof(ask),0,0,0);
        if(RecvSize!=sizeof(ask) || ask.ID!=*(unsigned int *)pBuf)
        {
            //�������ݲ��ԣ��ط�
            if(--TrySend == 0)
                return 0;
            continue;
        }
        break;
    }
    return 1;
}
//---------------------------------------------------------------------------
static int TUDPClient_RecvBuffer(TUDPClient *This,void *pBuf,int size,int TimeOut,
    void * from,int * fromlen)
{
    TIMEVAL timeout;
	FD_SET fdR;

	if(This->m_socket==0)
		return -1;
    if(TimeOut<0)
    	return recvfrom(This->m_socket,(char*)pBuf,size,0,(struct sockaddr*)from,fromlen);
    else
    {
		int RetSize;
		FD_ZERO(&fdR);
		FD_SET(This->m_socket, &fdR);
        timeout.tv_sec=TimeOut / 1000;
        timeout.tv_usec=(TimeOut % 1000)<<10;
        if(select(This->m_socket+1, &fdR,NULL, NULL, &timeout)<=0)
            return -1;
    	RetSize = recvfrom(This->m_socket,(char*)pBuf,size,0,(struct sockaddr*)from,fromlen);

		return RetSize;
    }
}
//---------------------------------------------------------------------------
// �������ݺ���Ӧ��
//---------------------------------------------------------------------------
static int TUDPClient_TrusthRecv(TUDPClient *This,void *pBuf,int size,int TimeOut)
{
    TIMEVAL timeout;
	FD_SET fdR;
	struct sockaddr fromip;
	int fromlen = sizeof(struct sockaddr);
	int RetSize;

	if(This->m_socket==0)
		return -1;
	if(TimeOut<0) {
		RetSize = recvfrom(This->m_socket,(char*)pBuf,size,0,&fromip,&fromlen);
    	if(RetSize>sizeof(COMMUNICATION)) {
			sendto(This->m_socket,pBuf,4,0,&fromip,sizeof(fromip));
		}
	}
    else
    {
		FD_ZERO(&fdR);
		FD_SET(This->m_socket, &fdR);
        timeout.tv_sec=TimeOut / 1000;
        timeout.tv_usec=(TimeOut % 1000)<<10;
        if(select(This->m_socket+1, &fdR,NULL, NULL, &timeout)<=0)
            return -1;
    	RetSize = recvfrom(This->m_socket,(char*)pBuf,size,0,&fromip,&fromlen);
		if(RetSize>sizeof(COMMUNICATION)) {
			sendto(This->m_socket,pBuf,4,0,&fromip,sizeof(fromip));
		}
    }
	return RetSize;
}
//---------------------------------------------------------------------------
//  ����һ��UDP�ͻ��ˣ�PortΪ0�򲻰󶨶˿�
//---------------------------------------------------------------------------
TUDPClient* TUDPClient_Create(int Port)
{
	int ret;
    TUDPClient* This;
	struct sockaddr_in local_addr;
    WSADATA  wsadata;
	int   opt=1;

    if(WSAStartup(MAKEWORD(2,2),&wsadata)!=0)
        return NULL;

    This = (TUDPClient *)malloc(sizeof(TUDPClient));
    if(This==NULL) {
        PRINTF("alloc UDPClient memory failt!\n");
        WSACleanup();
        return NULL;
    }
	memset(&local_addr,0,sizeof(local_addr));
	This->m_socket=socket(AF_INET,SOCK_DGRAM,0);
	if(This->m_socket==0) {
		PRINTF("UDP Client init socket failed!\n");
        free(This);
        WSACleanup();
        return NULL;
    }
	ret = setsockopt(This->m_socket,SOL_SOCKET,SO_BROADCAST,(char*)&opt,sizeof(opt));
	if(ret!=0) {
		printf("setsockopt error %d\n",ret);
	}

	if(Port)
    {
    	local_addr.sin_family = AF_INET;
	    local_addr.sin_port = htons(Port);
    	local_addr.sin_addr.s_addr = INADDR_ANY;
    	if(bind(This->m_socket, (struct sockaddr *)&local_addr, sizeof(struct sockaddr))<0)
        {
            printf("UDP client bind to port failed!\n");
            closesocket(This->m_socket);
            free(This);
            WSACleanup();
            return NULL;
        }
    }
    This->Destroy = TUDPClient_Destroy;
	This->Clear = TUDPClient_ClearBuffer;
    This->RecvBuffer = TUDPClient_RecvBuffer;
    This->SendBuffer = TUDPClient_SendBuffer;
    This->TrusthSend = TUDPClient_TrusthSend;
	This->TrusthRecv = TUDPClient_TrusthRecv;
    return This;
}
//---------------------------------------------------------------------------
#endif
