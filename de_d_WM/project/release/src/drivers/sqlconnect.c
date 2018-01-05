#ifdef WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlconnect.h"
#include "MsSQLConnect.h"
#include "TCPClient.h"
#include "TDateTime.h"
#include "communication.h"
#include "predefine.h"

int PingServer(const char *IP);
//----------------------------------------------------------------------------
void ErrorLog(int ecode,const char *fmt,...);

typedef struct 
{
	int Size;			//��������С
	char Data[1];		//������
}TBLOBFIELD;
//----------------------------------------------------------------------------
struct QueryPrivate
{
    BOOL Connected;		//�Ƿ�����
    BOOL Active;
    SQL_DATASETFIELD *Fields;
    char ServerIP[16];
    int ServerPort;
    char *pData;
    char *pRecord;
    char *pSQLText;
    int SQLTextSize;
    TTCPClient *tcp;
    int RecNo;
	int FieldCount;				//�ֶ���
    int RecordSize;
	int RecordCount;			//��¼����
	char *pExternData;			//��չ����
};
//----------------------------------------------------------------------------
/*
static char * strncpytrim(char *pDest,const char *pSrc,int Size)
{
	int i = Size;
	char *pTmp = pDest;
	while(*pSrc && --i)
	{
		*pDest++ = *pSrc++;
	}
	//ȥ��β���ո�
	while(pTmp!=pDest)
	{
		if(*(--pDest)!=32)
		{
			pDest++;
			break;
		}
	}
	*pDest = 0;
	return pTmp;
}
*/
//----------------------------------------------------------------------------
static BOOL FieldVaild(PField This)
{
    if(!This->Private->Active)
    {
    	ErrorLog(0,"���ܲ���һ���رյĲ�ѯ���");
        return FALSE;
    }
    if(!This->Private->RecNo)
    {
    	ErrorLog(0,"���ݼ�Ϊ��,���ܴ�ȡ�ֶ�%s",This->Field->FieldName);
        return FALSE;
    }
    if(This->Private->pRecord==NULL)
    {
    	ErrorLog(0,"���ݼ�Ϊ��,���ܴ�ȡ�ֶ�%s",This->Field->FieldName);
        return FALSE;
	}
	return TRUE;
}
//----------------------------------------------------------------------------
static char * TField_AsChar(PField This,char *Buf,int Size)
{
	int MinSize=Size<This->Field->FieldSize?Size:This->Field->FieldSize;
	if(Buf==NULL)
    	return NULL;
	if(!FieldVaild(This)) {
		Buf[0] = 0;
		return Buf;
	}
	strncpy(Buf,&This->Private->pRecord[This->Offset],MinSize);
	if(Size>MinSize)
		Buf[MinSize]=0;
	return Buf;
}
//----------------------------------------------------------------------------
static int TField_AsBlobData(PField This,char *Buf,int Size)
{
	int MinSize,BufIdx;
	TBLOBFIELD *pBlob;
	if(!FieldVaild(This)) {
		return 0;
	}
	if(This->Private->pExternData== NULL) {
		return 0;
	}
	BufIdx = *(int*)&This->Private->pRecord[This->Offset];
	if(BufIdx<0)
		return 0;
	pBlob = (TBLOBFIELD*)&This->Private->pExternData[BufIdx];
	if(!pBlob)
		return 0;
	if(Buf==NULL)
		return pBlob->Size;
	MinSize=Size<pBlob->Size?Size:pBlob->Size;
	memcpy(Buf,pBlob->Data,MinSize);
	return MinSize;
}
//----------------------------------------------------------------------------
static int TField_AsInt(PField This)
{
	if(!FieldVaild(This))
		return 0;
	return *(int*)&This->Private->pRecord[This->Offset];
}
//----------------------------------------------------------------------------
static double TField_AsFloat(PField This)
{
	if(!FieldVaild(This))
		return 0.0;
	return *(double*)&This->Private->pRecord[This->Offset];
}
//----------------------------------------------------------------------------
static char * TField_AsDateTime(PField This,char *Buf,int Size)
{
	int Date,Time;
	TDateTime *dtDate;
	if(Buf==NULL)
    	return NULL;
	if(!FieldVaild(This)) {
		Buf[0] = 0;
		return Buf;
	}
	Date = *(int *)&This->Private->pRecord[This->Offset];
	Time = *(int *)&This->Private->pRecord[This->Offset+4];
	dtDate = CreateDateTime(Date,Time);
	dtDate->GetString(dtDate,Buf,Size);
	dtDate->Destroy(dtDate);
	return Buf;
}
//----------------------------------------------------------------------------
static void TMyQuery_SetSrvIP(QUERY_THIS,const char *pSrvIP)
{
	strncpy(This->Private->ServerIP,pSrvIP,15);
}
//----------------------------------------------------------------------------
static BOOL TMyQuery_Connect(QUERY_THIS)
{
	if(This->Private->ServerIP[0] && PingServer(This->Private->ServerIP)) {
		This->Private->Connected++;
		return TRUE;
	} else {
		if(This->Private->Connected)
			This->DisConnect(This,TRUE);
		return FALSE;
	}
}
//----------------------------------------------------------------------------
static BOOL TMyQuery_DisConnect(QUERY_THIS,BOOL ClearAll)
{
	if(This->Private->Connected) {
		if(ClearAll) {
			This->Private->Connected = 0;
		} else {
			if(This->Private->Connected>0) {
				This->Private->Connected--;
			}
		}
		if(This->Private->Connected==0) {
			This->Close(This);
		}
	}
	return TRUE;
}
//----------------------------------------------------------------------------
static BOOL TMyQuery_IsConnected(QUERY_THIS)
{
	return This->Private->Connected;
}
//----------------------------------------------------------------------------
static void TMyQuery_Destroy(QUERY_THIS)
{
    if(This->Private->pSQLText)
        free(This->Private->pSQLText);
    if(This->Private->Active)
        This->Close(This);
    This->Private->tcp->Destroy(This->Private->tcp);
    free(This->Private);
    free(This);
    This = NULL;
}
//----------------------------------------------------------------------------
//ȡSQL������
static int TMyQuery_GetSQLText(QUERY_THIS,char *pBuf,int Size)
{
    if(This->Private->SQLTextSize)
        strncpy(pBuf,This->Private->pSQLText,Size);
    return This->Private->SQLTextSize;
}
//----------------------------------------------------------------------------
//����SQL������
static void TMyQuery_SetSQLText(QUERY_THIS,char *SqlCmd)
{
    int Size;
    if(SqlCmd==NULL)
        return;
    Size = strlen(SqlCmd)+1;
    This->Private->pSQLText = (char*)realloc(This->Private->pSQLText,Size);
    strcpy(This->Private->pSQLText,SqlCmd);
	This->Private->SQLTextSize = Size;
}
//----------------------------------------------------------------------------
//��
static BOOL TMyQuery_Open(QUERY_THIS)
{
	SQLCOMMUNICATION Head;
    SQLREQUEST SqlRequest;
    char cSqlAnswerBuf[sizeof(SQL_DATASETHEAD)];
    SQL_ANSWER *SqlAnswer = (SQL_ANSWER *)cSqlAnswerBuf;
    SQL_DATASETHEAD *pDatasetHead = (SQL_DATASETHEAD*)cSqlAnswerBuf;
    int CmdSize;
    int ErrorCode;
    int i,tmp;

	if(!This->Private->Connected) {
		printf("SQL Server %s not connected\n",This->Private->ServerIP);
		goto error;
	}
	if(This->Private->Active)
    {
        ErrorLog(0,"OpenSQL:��ѯ����Ѿ���!");
        goto error;
    }

	if(This->Private->ServerIP[0]==0 || This->Private->ServerPort==0)
    {
        ErrorLog(0,"OpenSQL:ServerIP:%s or ServerPort:%d is error",This->Private->ServerIP,This->Private->ServerPort);
        goto error;
    }

	if(This->Private->pSQLText==NULL)
    {
        ErrorLog(0,"OpenSQL:SQL��䲻��Ϊ��!");
        goto error;
    }
    if(This->Private->tcp->Connect(This->Private->tcp,This->Private->ServerIP,This->Private->ServerPort,3000))
    {
        ErrorLog(0,"OpenSQL:����SQL����ʧ��!");
        goto error;
    }

    //����SQL����
    CmdSize = strlen(This->Private->pSQLText)+1;
    Head.Size = sizeof(SQLCOMMUNICATION)+sizeof(SQLREQUEST)+CmdSize;
    This->Private->tcp->SendBuffer(This->Private->tcp,&Head,sizeof(Head));

    SqlRequest.SqlType = 1;
    SqlRequest.CmdSize = CmdSize;
    This->Private->tcp->SendBuffer(This->Private->tcp,&SqlRequest,sizeof(SQLREQUEST));
    This->Private->tcp->SendBuffer(This->Private->tcp,This->Private->pSQLText,CmdSize);

    //�ж�SQL��ѯ�����Ƿ��д���
    memset(cSqlAnswerBuf,0,sizeof(cSqlAnswerBuf));
    ErrorCode = This->Private->tcp->RecvBuffer(This->Private->tcp,SqlAnswer,sizeof(SQL_ANSWER),-1);
    if(ErrorCode!=sizeof(SQL_ANSWER))
    {
        ErrorLog(ErrorCode,"OpenSQL:��������ʧ��!");
        goto error1;
    }

    if(SqlAnswer->Error)
    {
        if(SqlAnswer->ErrMsgSize)
        {
            char *pErrorMsg = (char*)malloc(SqlAnswer->ErrMsgSize);
            ErrorCode = This->Private->tcp->RecvBuffer(This->Private->tcp,pErrorMsg,SqlAnswer->ErrMsgSize,-1);
            if(ErrorCode!=(int)SqlAnswer->ErrMsgSize)
                ErrorLog(ErrorCode,"OpenSQL:��������ʧ��!");
            else
	            ErrorLog(0,"SQL��ѯ����:%s",pErrorMsg);
            free(pErrorMsg);
        }
		goto error1;
    }

    //����SQL_DATASETHEAD�ṹʣ��ֵ
    ErrorCode = This->Private->tcp->RecvBuffer(This->Private->tcp,&cSqlAnswerBuf[sizeof(SQL_ANSWER)],
        sizeof(SQL_DATASETHEAD)-sizeof(SQL_ANSWER),-1);
    if(ErrorCode!=sizeof(SQL_DATASETHEAD)-sizeof(SQL_ANSWER))
    {
        ErrorLog(ErrorCode,"OpenSQL:��������ʧ��!");
    	goto error1;
    }

    This->Private->RecordCount = pDatasetHead->RecordCount;
    This->Private->RecordSize = pDatasetHead->RecordSize;
    This->Private->FieldCount = pDatasetHead->FieldCount;

    //�����ֶ������Ա�
    tmp = This->Private->FieldCount*sizeof(SQL_DATASETFIELD);
    This->Private->Fields = (SQL_DATASETFIELD *)malloc(tmp);
    if(This->Private->Fields==NULL)
    {
        ErrorLog(0x7FFFFFF,"OpenSQL:�����ڴ�ʧ��!");
    	goto error1;
    }
    ErrorCode = This->Private->tcp->RecvBuffer(This->Private->tcp,This->Private->Fields,tmp,-1);
    if(ErrorCode!=tmp)
    {
        ErrorLog(ErrorCode,"OpenSQL:��������ʧ��!");
    	goto error2;
    }

    This->Fields = (PField)malloc(This->Private->FieldCount*sizeof(TMyField));
    if(This->Fields==NULL)
    {
        ErrorLog(0x7FFFFFF,"OpenSQL:�����ڴ�ʧ��!");
    	goto error2;
    }
    tmp = 0;
    for(i=0;i<This->Private->FieldCount;i++)
    {
    	This->Fields[i].Private = This->Private;
    	This->Fields[i].Field = &This->Private->Fields[i];
        This->Fields[i].Offset = tmp;
        tmp += This->Private->Fields[i].FieldSize;
        This->Fields[i].AsChar = TField_AsChar;
        This->Fields[i].AsBlobData = TField_AsBlobData;
        This->Fields[i].AsInt = TField_AsInt;
        This->Fields[i].AsFloat = TField_AsFloat;
		This->Fields[i].AsDateTime = TField_AsDateTime;
    }
    
    //���ռ�¼
    tmp = This->Private->RecordSize * This->Private->RecordCount;
    if(tmp)
    {
    	This->Private->pData = (char*)malloc(tmp);
        if(This->Private->pData==NULL)
        {
	        ErrorLog(0x7FFFFFF,"OpenSQL:�����ڴ�ʧ��!");
    		goto error3;
        }
        This->Private->pRecord = This->Private->pData;
	    ErrorCode = This->Private->tcp->RecvBuffer(This->Private->tcp,This->Private->pData,tmp,-1);
    	if(ErrorCode!=tmp)
	    {
    	    ErrorLog(ErrorCode,"OpenSQL:��������ʧ��!");
    		goto error4;
	    }
    }

	//������չ���ݲ���
	if(pDatasetHead->HaveExternData) {
		if(This->Private->tcp->RecvBuffer(This->Private->tcp,&tmp,4,-1)!=4) {
			ErrorLog(ErrorCode,"OpenSQL:��������ʧ��!");
    		goto error2;
		}
		//ֻ���ܲ�����20M������
		if(tmp>0 && tmp<20*1024*1024) {
			This->Private->pExternData = (char*)malloc(tmp);
			if(This->Private->pExternData ==NULL) {
			    ErrorLog(0x7FFFFFF,"OpenSQL:�����ڴ�ʧ��!");
				goto error2;
			}
			if(This->Private->tcp->RecvBuffer(This->Private->tcp,This->Private->pExternData,tmp,-1)!=tmp) {
				ErrorLog(ErrorCode,"OpenSQL:��������ʧ��!");
    			goto error2;
			}
		} else {
			ErrorLog(0x7FFFFFF,"OpenSQL:��չ����̫��!");
			goto error2;
		}
	}
    This->Private->Active = TRUE;
    This->Private->RecNo = 1;
    return TRUE;
error4:
	free(This->Private->pData);
error3:
	free(This->Fields);
error2:
	if(This->Private->pExternData) {
		free(This->Private->pExternData);
		This->Private->pExternData = NULL;
	}
	free(This->Private->Fields);
error1:
    This->Private->tcp->DisConnect(This->Private->tcp);
error:
    return FALSE;
}
//----------------------------------------------------------------------------
//ִ��
static BOOL TMyQuery_ExecSQL(QUERY_THIS)
{
    SQLCOMMUNICATION Head;
    SQLREQUEST SqlRequest;
    SQL_ANSWER SqlAnswer;
    int CmdSize;
    int ErrorCode;

    //���
	if(!This->Private->Connected) {
		printf("SQL Server %s not connected\n",This->Private->ServerIP);
		return FALSE;
	}
    if(This->Private->Active)
    {
        ErrorLog(0,"ExecSQL:��ѯ����Ѿ���!");
        return FALSE;
    }
    if(This->Private->ServerIP[0]==0 || This->Private->ServerPort==0)
    {
        ErrorLog(0,"ExecSQL:ServerIP:%s or ServerPort:%d is error",
			This->Private->ServerIP,This->Private->ServerPort);
        return FALSE;
    }
    if(This->Private->pSQLText==NULL)
    {
        ErrorLog(0,"ExecSQL:SQL��䲻��Ϊ��!");
        return FALSE;
    }
    if(This->Private->tcp->Connect(This->Private->tcp,This->Private->ServerIP,
		This->Private->ServerPort,3000))
    {
        ErrorLog(0,"ExecSQL:����SQL������ʧ��");
        return FALSE;
    }

    //����SQL����
    CmdSize = strlen(This->Private->pSQLText)+1;
    Head.Size = sizeof(SQLCOMMUNICATION)+sizeof(SQLREQUEST)+CmdSize;
    This->Private->tcp->SendBuffer(This->Private->tcp,&Head,sizeof(Head));

    SqlRequest.SqlType = 2;
    SqlRequest.CmdSize = CmdSize;
    This->Private->tcp->SendBuffer(This->Private->tcp,&SqlRequest,sizeof(SQLREQUEST));
    This->Private->tcp->SendBuffer(This->Private->tcp,This->Private->pSQLText,CmdSize);

    //���շ��ؽ��
    memset(&SqlAnswer,0,sizeof(SQL_ANSWER));
    ErrorCode = This->Private->tcp->RecvBuffer(This->Private->tcp,&SqlAnswer,sizeof(SQL_ANSWER),5000);
    if(ErrorCode!=sizeof(SQL_ANSWER))
    {
        This->Private->tcp->DisConnect(This->Private->tcp);
        ErrorLog(ErrorCode,"ExecSQL:��������ʧ��!");
        return FALSE;
    }
    if(SqlAnswer.ErrMsgSize)
    {
        char *pErrorMsg = (char*)malloc(SqlAnswer.ErrMsgSize);
        ErrorCode = This->Private->tcp->RecvBuffer(This->Private->tcp,pErrorMsg,SqlAnswer.ErrMsgSize,5000);
        if(ErrorCode!=(int)SqlAnswer.ErrMsgSize)
        {
            This->Private->tcp->DisConnect(This->Private->tcp);
            free(pErrorMsg);
            ErrorLog(ErrorCode,"ExecSQL:��������ʧ��!");
            return FALSE;
        }
        ErrorLog(0,"SQLִ��ʧ��:%s",pErrorMsg);
        free(pErrorMsg);
    }

    //��������
    This->Private->tcp->DisConnect(This->Private->tcp);

    if(SqlAnswer.Error)
        return FALSE;
    return TRUE;
}
//----------------------------------------------------------------------------
//�ر�
static void TMyQuery_Close(QUERY_THIS)
{
    if(This->Private->Active)
    {
		if(This->Private->pExternData) {
			free(This->Private->pExternData);
			This->Private->pExternData = NULL;
		}
        free(This->Fields);
        This->Fields = NULL;
        free(This->Private->Fields);
        This->Private->Fields = NULL;
        free(This->Private->pData);
        This->Private->pData = NULL;
        This->Private->Active = FALSE;
        This->Private->Active = FALSE;
        This->Private->RecordCount = 0;
        This->Private->RecNo = 0;
    }
}
//----------------------------------------------------------------------------
//Query�Ƿ񼤻�(ֻ��)
static BOOL TMyQuery_Active(QUERY_THIS)
{
	return This->Private->Active;
}
//----------------------------------------------------------------------------
static int TMyQuery_RecordCount(QUERY_THIS)		//��¼��
{
	return This->Private->RecordCount;
}
//----------------------------------------------------------------------------
static int TMyQuery_FieldCount(QUERY_THIS)		//�ֶ�����
{
	return This->Private->FieldCount;
}
//----------------------------------------------------------------------------
//������¼��
static void TMyQuery_SetRecNo(QUERY_THIS,int RecNo)
{
    if(This->Private->RecordCount)
    {
        if(RecNo<=0)
            RecNo=1;
        if(RecNo>This->Private->RecordCount)
            RecNo = This->Private->RecordCount;
        This->Private->RecNo = RecNo;
        This->Private->pRecord = &This->Private->pData[(RecNo-1)*This->Private->RecordSize];
    }
}
//----------------------------------------------------------------------------
//�׼�¼
static void TMyQuery_First(QUERY_THIS)
{
    TMyQuery_SetRecNo(This,1);
}
//----------------------------------------------------------------------------
//ĩ��¼
static void TMyQuery_Last(QUERY_THIS)
{
    TMyQuery_SetRecNo(This,This->Private->RecordCount);
}
//----------------------------------------------------------------------------
//��һ��¼
static void TMyQuery_Prior(QUERY_THIS)
{
    TMyQuery_SetRecNo(This,This->Private->RecNo-1);
}
//----------------------------------------------------------------------------
//��һ��¼
static void TMyQuery_Next(QUERY_THIS)
{
    TMyQuery_SetRecNo(This,This->Private->RecNo+1);
}
//----------------------------------------------------------------------------
//���ؼ�¼��
static int TMyQuery_RecNo(QUERY_THIS)
{
    return This->Private->RecNo;
}
//----------------------------------------------------------------------------
//�����ֶ�
static PField TMyQuery_FieldByName(QUERY_THIS,char *Name)
{
    int i;
    if(This->Private->Active==0)
        return NULL;
    for(i=0;i<This->Private->FieldCount;i++)
    {
#ifdef WIN32
        if(stricmp(This->Fields[i].Field->FieldName,Name)==0)
#else
        if(strcasecmp(This->Fields[i].Field->FieldName,Name)==0)
#endif
        {
            return &This->Fields[i];
        }
    }
    ErrorLog(0,"�ֶ���%sû�ж���!",Name);
    return NULL;
}
//----------------------------------------------------------------------------
TMyQuery * CreateQuery(char *Host,char *User,char *Password)
{
    TMyQuery * This = (TMyQuery *)malloc(sizeof(TMyQuery));
    memset(This,0,sizeof(TMyQuery));
    This->Private = (struct QueryPrivate*)malloc(sizeof(struct QueryPrivate));
    memset(This->Private,0,sizeof(struct QueryPrivate));
    This->Private->tcp = TTCPClient_Create();
	strncpy(This->Private->ServerIP,Host,15);
	This->Private->ServerPort = 7890;
//	strncpy(This->Private->User,User,15);
//	strncpy(This->Private->Password,Password,19);

	This->SetSrvIP = TMyQuery_SetSrvIP;
	This->Connect = TMyQuery_Connect;
	This->DisConnect = TMyQuery_DisConnect;
	This->IsConnected = TMyQuery_IsConnected;

	This->GetSQLText = TMyQuery_GetSQLText;
	This->SetSQLText = TMyQuery_SetSQLText;
	This->Open = TMyQuery_Open;
	This->Active = TMyQuery_Active;
	This->Close = TMyQuery_Close;
	This->ExecSQL = TMyQuery_ExecSQL;
	This->RecordCount = TMyQuery_RecordCount;
	This->FieldCount = TMyQuery_FieldCount;

	This->First = TMyQuery_First;
	This->Last = TMyQuery_Last;
	This->Prior = TMyQuery_Prior;
	This->Next = TMyQuery_Next;
	This->SetRecNo = TMyQuery_SetRecNo;
	This->RecNo = TMyQuery_RecNo;
	This->FieldByName = TMyQuery_FieldByName;

	This->Destroy = TMyQuery_Destroy;
    return This;
}
//----------------------------------------------------------------------------
BOOL QueryOpen(TMyQuery *Query,char *SqlStr)
{
	if(Query->Private->Active)
    	Query->Close(Query);
    Query->SetSQLText(Query,SqlStr);
    return Query->Open(Query);
}
//----------------------------------------------------------------------------
BOOL QueryExec(TMyQuery *Query,char *SqlStr)
{
	if(Query->Private->Active)
    	Query->Close(Query);
    Query->SetSQLText(Query,SqlStr);
    return Query->ExecSQL(Query);
}
//----------------------------------------------------------------------------
char* QueryOfChar(TMyQuery *Query,char *FieldName,char *cBuf,int Size)
{
	PField Field;
	Field = Query->FieldByName(Query,FieldName);
    if(Field==NULL)
    {
        return cBuf;
    }

	return Field->AsChar(Field,cBuf,Size);
}
//----------------------------------------------------------------------------
int QueryOfBlobData(TMyQuery *Query,char *FieldName,char *cBuf,int Size)
{
	PField Field;
	Field = Query->FieldByName(Query,FieldName);
    if(Field==NULL)
    {
        return 0;
    }

	return Field->AsBlobData(Field,cBuf,Size);
}
//----------------------------------------------------------------------------
int QueryOfInt(TMyQuery *Query,char *FieldName)
{
	PField Field;
	Field = Query->FieldByName(Query,FieldName);
    if(Field==NULL)
    {
        return 0;
    }

    return Field->AsInt(Field);
}
//----------------------------------------------------------------------------
double QueryOfFloat(TMyQuery *Query,char *FieldName)
{
	PField Field;
	Field = Query->FieldByName(Query,FieldName);
    if(Field==NULL)
    {
        return 0;
    }
    return Field->AsFloat(Field);
}
//----------------------------------------------------------------------------
char* QueryOfDateTime(TMyQuery *Query,char *FieldName,char *cBuf,int Size)
{
	PField Field;
	Field = Query->FieldByName(Query,FieldName);
    if(Field==NULL)
    {
        return cBuf;
    }

	return Field->AsDateTime(Field,cBuf,Size);
}
//----------------------------------------------------------------------------
#endif
