#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "sqlite.h"
#include "predefine.h"
//----------------------------------------------------------------------------
void ErrorLog(int ecode,const char *fmt,...);
char *strupper(char *pdst,const char *pstr,int Size);

struct SqlitePrivate
{
	BOOL Active;
	sqlite3 *db;
    sqlite3_stmt *ppStmt;
	char **pData;
	char *SqlText;
	char *pErrMsg;
	int FieldCount; 
	int RecordCount;
	int RecNo;
//	int LastRowID;
};
//----------------------------------------------------------------------------
static char * strncpytrim(char *pDest,const char *pSrc,int Size)
{
	int i = Size;
	char *pTmp = pDest;
	while(*pSrc && --i) {
		*pDest++ = *pSrc++;
	}
	//去除尾导空格
	while(pTmp!=pDest) {
		if(*(--pDest)!=32) {
			pDest++;
			break;
		}
	}
	*pDest = 0;
	return pTmp;
}
//----------------------------------------------------------------------------
static char * TField_AsChar(PSQLiteField This,char *Buf,int Size)
{
	// 
    if(!This->Private->Active)
    {
    	ErrorLog(0,"不能操作一个关闭的查询组件");
	    Buf[0]=0;
        return Buf;
    }
    if(This->Private->RecNo==0)
    {
    	ErrorLog(0,"数据集为空,不能存取字段%s",This->Name);
	    Buf[0]=0;
        return Buf;
	}
	if(This->Private->pData[This->Private->FieldCount*(This->Private->RecNo)+
		This->offset]==NULL) {
	    Buf[0]=0;
        return Buf;
	}
	strncpytrim(Buf,This->Private->pData[This->Private->FieldCount*(This->Private->RecNo)+
		This->offset],Size);
	return Buf;
}
//----------------------------------------------------------------------------
static int TField_AsInt(PSQLiteField This)
{
    if(!This->Private->Active)
    {
    	ErrorLog(0,"不能操作一个关闭的查询组件");
        return 0;
    }
    if(This->Private->RecNo==0)
    {
    	ErrorLog(0,"数据集为空,不能存取字段%s",This->Name);
        return 0;
	}
	if(This->Private->pData[This->Private->FieldCount*(This->Private->RecNo)+
		This->offset]==NULL)
		return 0;
	return atoi(This->Private->pData[This->Private->FieldCount*(This->Private->RecNo)+
		This->offset]);
}
//----------------------------------------------------------------------------
static double TField_AsFloat(PSQLiteField This)
{
    if(!This->Private->Active)
    {
    	ErrorLog(0,"不能操作一个关闭的查询组件");
        return 0;
    }
    if(This->Private->RecNo==0)
    {
    	ErrorLog(0,"数据集为空,不能存取字段%s",This->Name);
        return 0;
	}
	if(This->Private->pData[This->Private->FieldCount*(This->Private->RecNo)+
		This->offset]==NULL)
		return 0;
	return atof(This->Private->pData[This->Private->FieldCount*(This->Private->RecNo)+
		This->offset]);
}
//----------------------------------------------------------------------------
static void SQLite_Destroy(struct _TSqlite *This)
{
	if(This->Private->Active) {
		This->Close(This);
	}
	sqlite3_close(This->Private->db);
	if(This->Private->SqlText)
		free(This->Private->SqlText);
	free(This->Private);
	free(This);
}
//----------------------------------------------------------------------------
//打开
static BOOL SQLite_Open(struct _TSqlite *This)
{
	int i;
	if(This->Private->Active) {
		This->Close(This);
	}
	if(sqlite3_get_table(This->Private->db,This->Private->SqlText,&This->Private->pData,
			&This->Private->RecordCount,&This->Private->FieldCount,
			&This->Private->pErrMsg)==SQLITE_OK) {
		This->Fields = (PSQLiteField)malloc(sizeof(TSQLiteField)*This->Private->FieldCount);
		if(This->Fields==NULL) {
			// printf("Memory not enough\n");
			sqlite3_free_table(This->Private->pData);
			This->Private->pData = NULL;
			return TRUE;
		}
		memset(This->Fields,0,sizeof(TSQLiteField)*This->Private->FieldCount);
		This->Private->Active = TRUE;
		for(i=0;i<This->Private->FieldCount;i++) {
			strupper(This->Fields[i].Name,This->Private->pData[i],15);
			This->Fields[i].offset = i;
			This->Fields[i].Private = This->Private;
			This->Fields[i].AsChar = TField_AsChar;
			This->Fields[i].AsInt = TField_AsInt;
			This->Fields[i].AsFloat = TField_AsFloat;
		}
		if(This->Private->FieldCount)
			This->Private->RecNo = 1;
		else
			This->Private->RecNo = 0;
		return TRUE;
	}
	if(This->Private->pErrMsg) {
		ErrorLog(0,This->Private->pErrMsg);
		sqlite3_free(This->Private->pErrMsg);
		This->Private->pErrMsg = NULL;
	}
	return FALSE;
}
//----------------------------------------------------------------------------
//执行
static BOOL SQLite_ExecSQL(struct _TSqlite *This)
{
	if(This->Private->Active)
		This->Close(This);
	if(sqlite3_exec( This->Private->db,This->Private->SqlText,NULL,0,
		&This->Private->pErrMsg)==SQLITE_OK) {
		return TRUE;
	}
	if(This->Private->pErrMsg) {
		ErrorLog(0,This->Private->pErrMsg);
		sqlite3_free(This->Private->pErrMsg);
		This->Private->pErrMsg = NULL;
	}
	return FALSE;
}
//----------------------------------------------------------------------------
//关闭
static void SQLite_Close(struct _TSqlite *This)
{
	if(This->Private->Active) {
		sqlite3_free_table(This->Private->pData);
		This->Private->pData = NULL;
		free(This->Fields);
		This->Private->Active = FALSE;
	}
}
//----------------------------------------------------------------------------
// 返回记录数量
static int SQLite_RecordCount(struct _TSqlite *This)
{
	return This->Private->RecordCount;
}
//----------------------------------------------------------------------------
// 返回表是否打开
static BOOL SQLite_Active(struct _TSqlite *This)
{
	return This->Private->Active;
}
//----------------------------------------------------------------------------
// 返回字段数量
static int SQLite_FieldCount(struct _TSqlite *This)
{
	return This->Private->FieldCount;
}
//----------------------------------------------------------------------------
/* 取得最后插入影响的ID. */
static int SQLite_LastRowId(struct _TSqlite *This)
{
	return (int)sqlite3_last_insert_rowid(This->Private->db);
}
//----------------------------------------------------------------------------
//跳到记录号
static void SQLite_SetRecNo(struct _TSqlite *This,int RecNo)
{
	if(RecNo>0 && RecNo<=This->Private->RecordCount)
		This->Private->RecNo = RecNo;
}
//----------------------------------------------------------------------------
//首记录
static void SQLite_First(struct _TSqlite *This)
{
	if(This->Private->pData)
		This->Private->RecNo = 1;
	else
		This->Private->RecNo = 0;
}
//----------------------------------------------------------------------------
//末记录
static void SQLite_Last(struct _TSqlite *This)
{
	if(This->Private->pData)
		This->Private->RecNo = This->Private->RecordCount;
	else
		This->Private->RecNo = 0;
}
//----------------------------------------------------------------------------
//上一记录
static void SQLite_Prior(struct _TSqlite *This)
{
	if(This->Private->pData) {
		if(This->Private->RecNo>1)
			This->Private->RecNo--;
	}
}
//----------------------------------------------------------------------------
//下一记录
static void SQLite_Next(struct _TSqlite *This)
{
	if(This->Private->pData) {
		if(This->Private->RecNo<This->Private->RecordCount)
			This->Private->RecNo++;
	}
}
//----------------------------------------------------------------------------
//返回记录号
static int SQLite_RecNo(struct _TSqlite *This)
{
	return This->Private->RecNo;
}
//----------------------------------------------------------------------------
//返回字段
static PSQLiteField SQLite_FieldByName(struct _TSqlite *This,char *Name)
{
	int i;
	char FieldName[50];
	if(!This->Private->Active)
		return NULL;
	strupper(FieldName,Name,50);
	for(i=0;i<This->Private->FieldCount;i++) {
		if(strcmp(FieldName,This->Fields[i].Name)==0)
			return &This->Fields[i];
	}
	printf("SQL '%s' [%s] is not found\n",This->Private->SqlText,Name);
	return NULL;
}
//----------------------------------------------------------------------------
//取SQL命令行
static int SQLite_GetSQLText(struct _TSqlite *This,char *pBuf,int Size)
{
	if(!This->Private->SqlText)
		return FALSE;
	strncpy(pBuf,This->Private->SqlText,Size);
	return TRUE;
}
//----------------------------------------------------------------------------
//设置SQL命令行
static void SQLite_SetSQLText(struct _TSqlite *This,char *SqlCmd)
{
	int TextLen = strlen(SqlCmd)+1;
	if(This->Private->Active)
		return;
	This->Private->SqlText = (char *)realloc(This->Private->SqlText,TextLen);
	strcpy(This->Private->SqlText,SqlCmd);
}
static void SQLite_prepare(struct _TSqlite *This,char *SqlCmd)
{
    sqlite3_prepare(This->Private->db,
            SqlCmd,strlen(SqlCmd),
            &This->Private->ppStmt,NULL);
}
static void SQLite_reset(struct _TSqlite *This)
{
    sqlite3_reset(This->Private->ppStmt);
}
static void SQLite_finalize(struct _TSqlite *This)
{
    sqlite3_finalize(This->Private->ppStmt);
}
static void SQLite_step(struct _TSqlite *This)
{
    sqlite3_step(This->Private->ppStmt);
}
static void SQLite_bind_int(struct _TSqlite *This,int num_arg,int arg)
{
    sqlite3_bind_int(This->Private->ppStmt,num_arg,arg);
}
static void SQLite_bind_text(struct _TSqlite *This,int num_arg,char *text)
{
    sqlite3_bind_text(This->Private->ppStmt,num_arg,text,-1,SQLITE_STATIC);
}
//----------------------------------------------------------------------------
TSqlite * CreateLocalQuery(const char *FileName)
{
	int ret;

    TSqlite * This = (TSqlite *)malloc(sizeof(TSqlite));
    memset(This,0,sizeof(TSqlite));
    This->Private = (struct SqlitePrivate*)malloc(sizeof(struct SqlitePrivate));
    memset(This->Private,0,sizeof(struct SqlitePrivate));

	ret = sqlite3_open(FileName, &This->Private->db);
	if(ret!=SQLITE_OK) {
		free(This->Private);
		free(This);
		return NULL;
	}

	This->Destroy = SQLite_Destroy;
    This->Open = SQLite_Open;
    This->ExecSQL  = SQLite_ExecSQL;
    This->Close = SQLite_Close;
    This->First = SQLite_First;
    This->Last = SQLite_Last;
    This->Prior = SQLite_Prior;
    This->Next = SQLite_Next;
    This->SetRecNo = SQLite_SetRecNo;
    This->RecNo = SQLite_RecNo;
    This->FieldByName = SQLite_FieldByName;
    This->GetSQLText = SQLite_GetSQLText;
    This->SetSQLText = SQLite_SetSQLText;
	This->Active = SQLite_Active;
	This->RecordCount = SQLite_RecordCount;
	This->FieldCount = SQLite_FieldCount;
	This->LastRowId = SQLite_LastRowId;

    This->prepare = SQLite_prepare;
    This->reset = SQLite_reset;
    This->finalize = SQLite_finalize;
    This->step = SQLite_step;
    This->bind_int = SQLite_bind_int;
    This->bind_text = SQLite_bind_text;
    return This;
}
//----------------------------------------------------------------------------
BOOL LocalQueryOpen(TSqlite *Query,char *SqlStr)
{
    Query->Close(Query);
    Query->SetSQLText(Query,SqlStr);
    return Query->Open(Query);
}
//----------------------------------------------------------------------------
BOOL LocalQueryExec(TSqlite *Query,char *SqlStr)
{
    Query->Close(Query);
    Query->SetSQLText(Query,SqlStr);
    return Query->ExecSQL(Query);
}
//----------------------------------------------------------------------------
char* LocalQueryOfChar(TSqlite *Query,char *FieldName,char *cBuf,int Size)
{
	PSQLiteField Field;
	Field = Query->FieldByName(Query,FieldName);
    if(Field==NULL)
    {
        return cBuf;
    }

	return Field->AsChar(Field,cBuf,Size);
}
//----------------------------------------------------------------------------
int LocalQueryOfInt(TSqlite *Query,char *FieldName)
{
	PSQLiteField Field;
	Field = Query->FieldByName(Query,FieldName);
    if(Field==NULL)
    {
        return 0;
    }
    return Field->AsInt(Field);
}
//----------------------------------------------------------------------------
double LocalQueryOfFloat(TSqlite *Query,char *FieldName)
{
	PSQLiteField Field;
	Field = Query->FieldByName(Query,FieldName);
    if(Field==NULL)
    {
        return 0;
    }

    return Field->AsFloat(Field);
}
//----------------------------------------------------------------------------
