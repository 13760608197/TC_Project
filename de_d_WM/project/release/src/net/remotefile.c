#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#ifndef WIN32
#include <dirent.h>
#endif
#include "TCPClient.h"
#include "remotefile.h"

#ifndef WIN32
#define MyPostMessage SendNotifyMessage
extern int SendNotifyMessage(unsigned int hWnd,unsigned int Msg,unsigned int wParam,unsigned int lParam);
#else
extern int MyPostMessage(unsigned int hWnd,unsigned int Msg,unsigned int wParam,unsigned int lParam);
#endif

#define tcpclient This->Private->client

typedef struct
{
    int Cmd;            //open , delete
    int Mode;			//1 �� 2д 3��д
    int FileNameLen;    // filename size
    char FileName[1];   // filename
}FILE_OPEN_STRUCT;

// operate
enum {FILE_OPEN,FILE_DELETE,FILE_DOWNLOAD};
enum {FILE_READ,FILE_WRITE,FILE_SEEK,FILE_SETEND,FILE_CLOSE};

// file read
typedef struct
{
    int Cmd;            //FILE_READ
    int ReadSize;       //Read Size
}FILE_READ_STRUCT;

// file read return
typedef struct
{
    int Size;
    int Data[1];
}FILE_RET_READ;

// file write head
// return successful is writebyte, or fail is zero.
typedef struct
{
    int Cmd;        //FILE_WRITE
    int WriteSize;
}FILE_WRITE_STRUCT;

// file seek
// return file new postion;
typedef struct
{
    int Cmd;        //FILE_SEEK
    int offset;
    int origin;     // 0 file_begin, 1 file_curren, 2 file_end
}FILE_SEEK_STRUCT;

// set end of file
// return 1 if success, 0 if fail;
typedef struct
{
    int Cmd;        //FILE_SETEND
}FILE_SETEND_STRUCT;

struct RemoteFilePrivate		//˽������
{
	char TmpFile[64];
	char IP[64];
	BOOL IsOpen;
	int Pos;
	TTCPClient *client;
};

int ExcuteCmd(int bWait,char *Cmd,...);
//---------------------------------------------------------------------------
int ChangeFileName(TRemoteFile * This,char *NewName,const char *FileName)
{
	int k=strlen(FileName)-1;
	if(k<=0)
		return 0;
	while(k>0) {
		if(FileName[k]=='/' || FileName[k]=='\\')
			break;
		k--;
	}
	if(k==0) {
		strcpy(NewName,This->Private->TmpFile);
		return 1;
	}
	strncpy(NewName,FileName,k+1);
	strcpy(&NewName[k+1],This->Private->TmpFile);
	return 1;
}
//---------------------------------------------------------------------------
static int RemoteFile_Download(TRemoteFile * This,unsigned int hWnd,const char *SrcFile,
							   const char *DstFile,int ExecuteMode)
{
	int FailState;
	int Pos,LastPos=0;
	int filesize,iRet,readpos,readsize;
	FILE *hFile;
	char NewFileName[1024];
	char cBuf[1024];
	FILE_OPEN_STRUCT *pBuf = (FILE_OPEN_STRUCT*)cBuf;

	if(This->Private->IsOpen){
		This->Close(This);
	}
	tcpclient = TTCPClient_Create();
	if(tcpclient==NULL) {
		printf("RemoteFile:TCPClient Create fail!\n");
		if(hWnd)
			MyPostMessage(hWnd,MSG_UPDATEMSG,UPDATE_FAIL,-1);
		return FALSE;
	}
	if(tcpclient->Connect(tcpclient,This->Private->IP,7893,2000)!=0) {
		//����
		if(tcpclient->Connect(tcpclient,This->Private->IP,7893,2000)!=0) {
			printf("RemoteFile:connect %s fail!\n",This->Private->IP);
			FailState = -2;
			goto error;
		}
	}

	pBuf->Cmd = FILE_DOWNLOAD;
	pBuf->FileNameLen = strlen(SrcFile)+1;
	if(pBuf->FileNameLen>(512-12)) {
		printf("RemoteFile:filename %s is too length!\n",SrcFile);
		FailState = -3;
		goto error;
	}

	pBuf->Mode = 0;
	strcpy(pBuf->FileName,SrcFile);
	tcpclient->SendBuffer(tcpclient,cBuf,12+pBuf->FileNameLen);
	//printf("Jack : remotefile size = %d\n",filesize);
	int tmp = 0;
	filesize = 0;
	if(tmp = tcpclient->RecvBuffer(tcpclient,&filesize,sizeof(filesize),3000)!=sizeof(filesize)) {
		printf("RemoteFile:Open file Recv Data Error! tmp = %d, size = %d\n",tmp,(filesize));
		FailState = -4;
		goto error;
	}
	if(filesize<0) {
		printf("RemoteFile:[%s] Return filesize %d is Fail!\n",SrcFile,filesize);
		FailState = -5;
		goto error;
	}
	This->Private->Pos = 0;

	//�ȱ��浽��ʱ�ļ�
	if(ChangeFileName(This,NewFileName,DstFile)==0) {
		FailState = -6;
		goto error;
	}
	//�����ļ�
	if((hFile=fopen(NewFileName,"wb"))==NULL) {
		printf("Open %s to write error!\n",NewFileName);
		FailState = -7;
		goto error;
	}

	readpos = 0;
	while(readpos<filesize) {
		if(filesize-readpos>1024)
			readsize = 1024;
		else
			readsize = filesize-readpos;
		iRet = tcpclient->RecvBuffer(tcpclient,cBuf,readsize,3000);
		if(readsize!=iRet) {
			printf("Want read size %d, Real read size %d, Abort!\n",readsize,iRet);
			break;
		}
		readpos+=readsize;
		iRet = fwrite(cBuf,1,readsize,hFile);
		fflush(hFile);
		if(iRet!=readsize) {
			printf("Want write size %d, Real write size %d, Abort!\n",readsize,iRet);
			break;
		}
		if(hWnd)
		{
			Pos = readpos*100/filesize;
			if(Pos!=LastPos) {
				LastPos = Pos;
				MyPostMessage(hWnd,MSG_UPDATEMSG,UPDATE_POSITION,Pos);
			}
		}
	}
	fclose(hFile);
	tcpclient->Destroy(tcpclient);
	if(readpos==filesize) {
		remove(DstFile);
		if(rename(NewFileName,DstFile)==-1) {
			printf("ChangeFileName %s error\n",DstFile);
			FailState = -8;
			if(hWnd)
				MyPostMessage(hWnd,MSG_UPDATEMSG,UPDATE_FAIL,FailState);
			return FALSE;
		}
		if(ExecuteMode) {
			ExcuteCmd(1,"chmod","+x",DstFile,NULL);
		}
		printf("Receive %s success\n",SrcFile);
		if(hWnd) {
			MyPostMessage(hWnd,MSG_UPDATEMSG,UPDATE_SUCCESS,0);
		}
		return TRUE;
	}
	else {
		printf("Receive %s abort!\n",SrcFile);
		FailState = -9;
		if(hWnd)
			MyPostMessage(hWnd,MSG_UPDATEMSG,UPDATE_FAIL,FailState);
		return FALSE;
	}
error:
	tcpclient->Destroy(tcpclient);
	//if(hWnd)
		//MyPostMessage(hWnd,MSG_UPDATEMSG,UPDATE_FAIL,FailState);
	return FALSE;
}
//---------------------------------------------------------------------------
static int RemoteFile_Open(TRemoteFile * This,const char *FileName,int OpenMode)
{
	int iRet;
	char cBuf[256];
	FILE_OPEN_STRUCT *pBuf = (FILE_OPEN_STRUCT*)cBuf;
	if(This->Private->IsOpen){
		This->Close(This);
	}
	tcpclient = TTCPClient_Create();
	if(tcpclient==NULL)
		return -1;
	if(tcpclient->Connect(tcpclient,This->Private->IP,7893,3000)!=0) {
		printf("RemoteFile:connect fail!\n");
		goto error;
	}

	pBuf->Cmd = FILE_OPEN;
	pBuf->FileNameLen = strlen(FileName)+1;
	if(pBuf->FileNameLen>(256-12)) {
		printf("RemoteFile:filename is too length!\n");
		goto error;
	}

	pBuf->Mode = OpenMode;
	strcpy(pBuf->FileName,FileName);
	tcpclient->SendBuffer(tcpclient,cBuf,12+pBuf->FileNameLen);
	if(tcpclient->RecvBuffer(tcpclient,&iRet,sizeof(iRet),3000)!=sizeof(iRet)) {
		printf("RemoteFile:Open file Recv Data Error!\n");
		goto error;
	}
	if(iRet<0) {
		printf("RemoteFile:Open File is Fail!\n");
		goto error;
	}
	This->Private->IsOpen = TRUE;
	This->Private->Pos = 0;
	return iRet;
error:
	tcpclient->Destroy(tcpclient);
	return -1;
}
//---------------------------------------------------------------------------
static int RemoteFile_Read(TRemoteFile * This,void *Buffer,int Size)
{
	int RetSize;
	FILE_READ_STRUCT fileread;
	if(!This->Private->IsOpen)
		return 0;
	if(Size<1 && Size>1024*1024)
		return -1;
	fileread.Cmd = FILE_READ;
	fileread.ReadSize = Size;
	tcpclient->SendBuffer(tcpclient,&fileread,sizeof(FILE_READ_STRUCT));
	if(tcpclient->RecvBuffer(tcpclient,&RetSize,sizeof(int),2000)!=sizeof(int))
		return 0;
	if(tcpclient->RecvBuffer(tcpclient,Buffer,RetSize,2000)!=RetSize)
		return 0;
	This->Private->Pos+=RetSize;
	return RetSize;
}
//---------------------------------------------------------------------------
static BOOL RemoteFile_Write(TRemoteFile * This,void *Buffer,int Size)
{
	BOOL RetVal;
	FILE_WRITE_STRUCT filewrite;
	if(!This->Private->IsOpen)
		return FALSE;
	filewrite.Cmd = FILE_WRITE;
	filewrite.WriteSize = Size;
	tcpclient->SendBuffer(tcpclient,&filewrite,sizeof(FILE_WRITE_STRUCT));
	if(tcpclient->SendBuffer(tcpclient,Buffer,Size)!=Size)
		return FALSE;
	if(tcpclient->RecvBuffer(tcpclient,&RetVal,sizeof(RetVal),1500)!=sizeof(RetVal))
		return FALSE;
	if(RetVal)
		This->Private->Pos+=Size;
	return RetVal;
}
//---------------------------------------------------------------------------
static int RemoteFile_Seek(TRemoteFile * This,int offset,int origin)
{
	int NewPos;
	FILE_SEEK_STRUCT fileseek;
	if(!This->Private->IsOpen)
		return FALSE;
	fileseek.Cmd = FILE_SEEK;
	fileseek.offset = offset;
	fileseek.origin = origin;
	if(tcpclient->SendBuffer(tcpclient,&fileseek,sizeof(FILE_SEEK_STRUCT))!=sizeof(FILE_SEEK_STRUCT))
		return -1;
	if(tcpclient->RecvBuffer(tcpclient,&NewPos,sizeof(NewPos),1000)!=sizeof(NewPos))
		return -1;
	This->Private->Pos = NewPos;
	return NewPos;
}
//---------------------------------------------------------------------------
static BOOL RemoteFile_SetEnd(TRemoteFile * This)
{
	BOOL RetVal;
	FILE_SETEND_STRUCT filesetend;
	if(!This->Private->IsOpen)
		return FALSE;
	filesetend.Cmd = FILE_SETEND;
	if(tcpclient->SendBuffer(tcpclient,&filesetend,sizeof(FILE_SETEND_STRUCT))!=sizeof(FILE_SETEND_STRUCT))
		return FALSE;
	if(tcpclient->RecvBuffer(tcpclient,&RetVal,sizeof(RetVal),1000)!=sizeof(RetVal))
		return FALSE;
	return RetVal;
}
//---------------------------------------------------------------------------
static int RemoteFile_GetPos(TRemoteFile * This)
{
	return This->Private->Pos;
}
//---------------------------------------------------------------------------
static void RemoteFile_Close(TRemoteFile * This)
{
	if(This->Private->IsOpen){
		BOOL Cmd = FILE_CLOSE;
		This->Private->IsOpen = FALSE;
		tcpclient->SendBuffer(tcpclient,&Cmd,sizeof(Cmd));
		tcpclient->Destroy(tcpclient);
	}
}
//---------------------------------------------------------------------------
static void RemoteFile_Destroy(TRemoteFile * This)
{
	This->Close(This);
	free(This->Private);
	free(This);
}
//---------------------------------------------------------------------------
TRemoteFile * CreateRemoteFile(const char *IP,const char *TempFile)
{
	TRemoteFile * This = (TRemoteFile*)malloc(sizeof(TRemoteFile));
	if(This==NULL) {
        printf("alloc TRemoteFile memory failt!\n");
        return NULL;
    }
	This->Private = (struct RemoteFilePrivate*)malloc(sizeof(struct RemoteFilePrivate));
	if(This->Private==NULL) {
		free(This);
        printf("alloc TRemoteFile memory failt!\n");
        return NULL;
	}
	memset(This->Private,0,sizeof(struct RemoteFilePrivate));
	strncpy(This->Private->IP,IP,sizeof(This->Private->IP)-1);
	strncpy(This->Private->TmpFile,TempFile,(sizeof(This->Private->TmpFile))-1);
	This->Open = RemoteFile_Open;
	This->Download = RemoteFile_Download;
	This->Read = RemoteFile_Read;
	This->Write = RemoteFile_Write;
	This->Seek = RemoteFile_Seek;
	This->SetEnd = RemoteFile_SetEnd;
	This->GetPos = RemoteFile_GetPos;
	This->Close = RemoteFile_Close;
	This->Destroy = RemoteFile_Destroy;
	return This;
}
//---------------------------------------------------------------------------