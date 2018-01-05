/*
 * =====================================================================================
 *
 *       Filename:  DownLoadData.c
 *
 *    Description:  ���ط��ź�ID,IC����Э��
 *
 *        Version:  1.0
 *        Created:  2015-12-16 16:19:27
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
/* ---------------------------------------------------------------------------*
 *                      include head files
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "externfunc.h"

/* ---------------------------------------------------------------------------*
 *                  extern variables declare
 *----------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*
 *                  internal functions declare
 *----------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*
 *                        macro define
 *----------------------------------------------------------------------------*/
#if DBG_DOWN > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

#define   RECORDCOUNT   6

/* ---------------------------------------------------------------------------*
 *                      variables define
 *----------------------------------------------------------------------------*/

/**
 * @brief QueryOneIDCardData ��ѯ���Ƿ������ݿ���
 *
 * @param pCardCode ������
 *
 * @return �ڷ��� TRUE, ���ڷ��� FALSE
 */
static BOOL QueryOneIDCardData(DownLoadData *This,char *pCardCode)
{
	char SqlStr[512];
	sprintf(SqlStr,"Select Count(*) As Count From IDCard Where CardCode='%s'",pCardCode);
	LocalQueryOpen(This->Sql,SqlStr);
	int tmp = LocalQueryOfInt(This->Sql,"Count");
	This->Sql->Close(This->Sql);
	if(tmp > 0) {
		return TRUE;
	}
	return FALSE;
}

/**
 * @brief QueryOneUserInfoData ��ѯ�û�IP��Ӧ��Ϣ�Ƿ������ݿ���
 * ���벻���ظ���IP���ظ�
 *
 * @param UserInfo �û���Ϣ�ṹ
 *
 * @return �����򷵻�TRUE, �����ڷ��� FALSE
 */
static BOOL QueryOneUserInfoData(DownLoadData *This,IPADDRESS *UserInfo)
{
	char SqlStr[512];
	sprintf(SqlStr,"Select Count(*) As Count From UserInfo Where Code='%s'",UserInfo->Code);
	LocalQueryOpen(This->Sql,SqlStr);
	int tmp = LocalQueryOfInt(This->Sql,"Count");
	This->Sql->Close(This->Sql);
	if(tmp > 0) {
		return TRUE;
	}
	return FALSE;
}

/**
 * @brief DownLoadIDCardData ����ID������
 *
 * @param This
 * @param Write  ���ݰ�
 *
 * @return
 */
static int DownLoadIDCardData(DownLoadData *This,DOWNDATA *Write)
{
	IDCARD *IdCard = (IDCARD *)Write->Data;
	if(This->Count == 0) {
		char buff[100]={0};
		DOWNDATA *DownIP = (DOWNDATA *)buff;
		DownIP->Cmd = IDCARD_ERROR;															//����Ӧ��
		DownIP->Count = This->Count;																//���յ���������
		DownIP->DownLoadCount = 1;															//Ӧ���
		DownIP->PacketNum = This->PacketCurID;													//��ǰ����
		printf("PackNum=%d\n",Write->PacketNum);
		Public.SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));		//����Ӧ��ɹ�
		return 0;
	}
	if(Write->PacketNum == 0) {
		This->PacketCurID = -1;
	}
	if(Write->PacketNum > This->PacketCurID) {	//����
		This->PacketCurID++;
		char SqlStr[512];
		LocalQueryExec(This->Sql,"BEGIN TRANSACTION");
		int i;
		for(i=0;i<This->Count;i++) {
			if(QueryOneIDCardData(This,IdCard->CardCode)) { // ����������ݿ��У��򲻱�����
				IdCard++;
				continue;
			}
			sprintf(SqlStr,"Insert Into IDCard(Code,IP,CardCode,Type,HintAudio) Values(\"%s\",\"%s\",\"%s\",%d,\"%s\")",
					IdCard->Code,IdCard->IP,IdCard->CardCode,IdCard->CardType,IdCard->HintAudio);
			IdCard++;
			LocalQueryExec(This->Sql,SqlStr);
			memset(SqlStr,0,sizeof(SqlStr));
		}
		LocalQueryExec(This->Sql,"COMMIT");
		char buff[100]={0};
		DOWNDATA *DownIP = (DOWNDATA *)buff;
		DownIP->Cmd = IDCARD_OK;															//����Ӧ��
		DownIP->Count = This->Count;																//���յ���������
		DownIP->DownLoadCount =1;															//Ӧ���
		DownIP->PacketNum = This->PacketCurID;													//��ǰ����
		// printf("PackNum=%d\n",Write->PacketNum);
		Public.SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));		//����Ӧ��ɹ�
	} else if(Write->PacketNum == This->PacketCurID) {	//����
			char buff[100]={0};
			DOWNDATA *DownIP = (DOWNDATA *)buff;
			DownIP->Cmd = IDCARD_OK;															//����Ӧ��
			DownIP->Count = This->Count;																//���յ���������
			DownIP->DownLoadCount = 1;															//Ӧ���
			DownIP->PacketNum = This->PacketCurID;													//��ǰ����
			printf("PackNum=%d\n",Write->PacketNum);
			Public.SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));		//����Ӧ��ɹ�
		}
	return 1;
}

/**
 * @brief DownLoadIDCardData_2 �°�Э������ID��
 *
 * @param This
 * @param Write  ��������
 *
 * @return
 */
static int DownLoadIDCardData_2(DownLoadData *This,DOWNDATA *Write)
{
	TIDCardStruct2 *IdCard = (TIDCardStruct2 *)Write->Data;
    char SqlStr[320];

	if(This->Count == 0) {
		char buff[100]={0};
		DOWNDATA *DownIP = (DOWNDATA *)buff;
		DownIP->Cmd = IDCARD_ERROR;															//����Ӧ��
		DownIP->Count = This->Count;																//���յ���������
		DownIP->DownLoadCount = 1;															//Ӧ���
		DownIP->PacketNum = This->PacketCurID;													//��ǰ����
		// printf("PackNum=%d\n",Write->PacketNum);
		Public.SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));		//����Ӧ��ɹ�
		return 0;
	}
	if(Write->PacketNum == 0) {
		This->PacketCurID = -1;
        form_main->createForm(FORM_UPDATE_IC,NULL);
        LocalQueryExec(This->Sql,"BEGIN TRANSACTION");
        sprintf(SqlStr,
                "Insert Into IDCard(Code, IP, CardCode, Type, MaxUseDate, UseState,LimitDate,UseDay,\
            LST1,LET1,LST2,LET2,LST3,LET3) Values(?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
        This->Sql->prepare(This->Sql,SqlStr);

	}
	// printf("[%s]PacketNum:%d,DownLoadCount:%d,id:%d,count:%d\n",
			// __FUNCTION__,
			// Write->PacketNum,
			// Write->DownLoadCount,
			// This->PacketCurID,
			// This->Count);
	if(Write->PacketNum > This->PacketCurID) {	//����
		SendMessage(Screen.hUpdate,MSG_UPDATE_WTD,0,0);
		char cTmp[16];

		This->PacketCurID++;
		int i;
		for(i=0;i<This->Count;i++) {
			memset(cTmp, 0, sizeof(cTmp));
			sprintf(cTmp, "%s%s%s", IdCard->DYH,IdCard->CH,IdCard->FH); // �������ںţ�����
            This->Sql->reset(This->Sql);
            This->Sql->bind_text(This->Sql,1,cTmp);
            This->Sql->bind_text(This->Sql,2,IdCard->IP);
            This->Sql->bind_text(This->Sql,3,IdCard->CardCode);
            This->Sql->bind_int(This->Sql,4,IdCard->CardType+CARD_USER);
            This->Sql->bind_text(This->Sql,5,IdCard->MaxUseDate);
            This->Sql->bind_int(This->Sql,6,IdCard->UseState);
            This->Sql->bind_int(This->Sql,7,IdCard->LimitDate);
            This->Sql->bind_int(This->Sql,8,IdCard->UseDay);
            This->Sql->bind_int(This->Sql,9,IdCard->LimitStartTime1);
            This->Sql->bind_int(This->Sql,10,IdCard->LimitEndTime1);
            This->Sql->bind_int(This->Sql,11,IdCard->LimitStartTime2);
            This->Sql->bind_int(This->Sql,12,IdCard->LimitEndTime2);
            This->Sql->bind_int(This->Sql,13,IdCard->LimitStartTime3);
            This->Sql->bind_int(This->Sql,14,IdCard->LimitEndTime3);
            This->Sql->step(This->Sql);
            // memset(SqlStr, 0, sizeof(SqlStr));
            // sprintf(SqlStr,
                // "Insert Into IDCard(Code, IP, CardCode, Type,  MaxUseDate, UseState,LimitDate,UseDay,\
                // LST1,LET1,LST2,LET2,LST3,LET3) Values(\"%s\",\"%s\",\"%s\",%d,\"%s\",%d,%d,%d,\
                    // %d,%d,%d,%d,%d,%d)",
                // cTmp,
                // IdCard->IP,
                // IdCard->CardCode,
                // IdCard->CardType+CARD_USER,
                // IdCard->MaxUseDate,
                // IdCard->UseState,
                // IdCard->LimitDate,
                // IdCard->UseDay,
                // IdCard->LimitStartTime1,
                // IdCard->LimitEndTime1,
                // IdCard->LimitStartTime2,
                // IdCard->LimitEndTime2,
                // IdCard->LimitStartTime3,
                // IdCard->LimitEndTime3);
            // LocalQueryExec(This->Sql,SqlStr);
			IdCard++;
		}
		char buff[100]={0};
		DOWNDATA *DownIP = (DOWNDATA *)buff;
		DownIP->Cmd = IDCARD_OK;								//����Ӧ��
		DownIP->Count = This->Count;							//���յ���������
		DownIP->DownLoadCount = 1;								//Ӧ���
		DownIP->PacketNum = This->PacketCurID;					//��ǰ����
		Public.SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));		//����Ӧ��ɹ�
		if (This->Count >= (Write->DownLoadCount - Write->PacketNum * 8)) { // ���һ�����ر������ݿ�
			// printf("[%s]end PacketNum:%d,id:%d\n", __FUNCTION__,Write->PacketNum,This->PacketCurID);
			WatchDogClose();
            This->Sql->finalize(This->Sql);
			LocalQueryExec(This->Sql,"COMMIT");
			LocalQueryExec(LocalQuery, "REINDEX index_card");
			WatchDogOpen();
			if (sqlCheck(&LocalQuery,"DataBase.db",1) == TRUE) {//���ݿ�
				backData("DataBase.db");
			}
			SendMessage(Screen.hUpdate,MSG_UPDATE_IC_SUCCESS,0,0);
		}
	} else if(Write->PacketNum == This->PacketCurID) {			//����
		char buff[100]={0};
		DOWNDATA *DownIP = (DOWNDATA *)buff;
		DownIP->Cmd = IDCARD_OK;								//����Ӧ��
		DownIP->Count = This->Count;							//���յ���������
		DownIP->DownLoadCount = 1;								//Ӧ���
		DownIP->PacketNum = This->PacketCurID;					//��ǰ����
		// printf("PackNum=%d\n",Write->PacketNum);
		Public.SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));		//����Ӧ��ɹ�
	}
	return 1;
}

/**
 * @brief DownLoadUserInfoData ���ط���
 *
 * @param This
 * @param Write // ��������
 *
 * @return
 */
static int DownLoadUserInfoData(DownLoadData *This,DOWNDATA *Write)
{
	IPADDRESS *UserInfo = (IPADDRESS *)Write->Data;
	if(This->Count == 0) {
		char buff[100]={0};
		DOWNDATA *DownIP = (DOWNDATA *)buff;
		DownIP->Cmd = IP_ERROR;															//����Ӧ��
		DownIP->Count =This->Count;																//���յ���������
		DownIP->DownLoadCount = 1;															//Ӧ���
		DownIP->PacketNum = This->PacketCurID;													//��ǰ����
		Public.SendUdpData(This->cIP,This->Port,TP_COMPARELIST,buff,sizeof(buff));		//����Ӧ��ɹ�
		return 0;
	}
	if(Write->PacketNum == 0) {
		This->PacketCurID = -1;
	}
	if(Write->PacketNum > This->PacketCurID) {						//����
		This->PacketCurID++;
		char SqlStr[512];
		LocalQueryExec(This->Sql,"BEGIN TRANSACTION");
		int i;
		for(i=0;i<This->Count;i++) {
			if(QueryOneUserInfoData(This,UserInfo)) {
				sprintf(SqlStr,"Update UserInfo Set IP='%s' Where Code='%s'",
						UserInfo->IP,UserInfo->Code);
			} else {
				sprintf(SqlStr,"Insert Into UserInfo(Code,IP) Values(\"%s\",\"%s\") ",
						UserInfo->Code,UserInfo->IP);
			}
			LocalQueryExec(This->Sql,SqlStr);
			UserInfo++;
		}
		LocalQueryExec(This->Sql,"COMMIT");
		char buff[100]={0};
		DOWNDATA *DownIP = (DOWNDATA *)buff;
		DownIP->Cmd = IP_OK;										//����Ӧ��
		DownIP->Count =This->Count;									//���յ���������
		DownIP->DownLoadCount =1;									//Ӧ���
		DownIP->PacketNum = This->PacketCurID;						//��ǰ����
		printf("ret: Ip:%s,cont:%d,PacketNum:%d\n",
				This->cIP,DownIP->Count,DownIP->PacketNum);
		Public.SendUdpData(This->cIP,This->Port,TP_COMPARELIST,buff,sizeof(buff));		//����Ӧ��ɹ�
	} else if(Write->PacketNum == This->PacketCurID) {
		char buff[100]={0};
		DOWNDATA *DownIP = (DOWNDATA *)buff;
		DownIP->Cmd = IP_OK;										//����Ӧ��
		DownIP->Count =This->Count;									//���յ���������
		DownIP->DownLoadCount =1;									//Ӧ���
		DownIP->PacketNum = This->PacketCurID;						//��ǰ����
		Public.SendUdpData(This->cIP,This->Port,TP_COMPARELIST,buff,sizeof(buff));		//����Ӧ��ɹ�
	}
	return 1;
}

//����ID������ ʹ����Э��: IP_DOWNLOAD2
/**
 * @brief DownLoadUserInfoData_2 ���ط��Ŷ��ձ���Э��
 *
 * @param This
 * @param Write // ��������
 *
 * @return
 */
static int DownLoadUserInfoData_2(DownLoadData *This,DOWNDATA *Write)
{
	// IPADDRESS2 UniIP;
	TUserTable2 *UserInfo = (TUserTable2 *)Write->Data;
    char SqlStr[512];

	if(This->Count == 0) {
		char buff[100]={0};
		DOWNDATA *DownIP = (DOWNDATA *)buff;
		DownIP->Cmd = IP_ERROR;											//����Ӧ��
		DownIP->Count = This->Count;									//���յ���������
		DownIP->DownLoadCount = 1;										//Ӧ���
		DownIP->PacketNum = This->PacketCurID;							//��ǰ����
		Public.SendUdpData(This->cIP,This->Port,TP_COMPARELIST,buff,sizeof(buff));		//����Ӧ��ɹ�
		return 0;
	}
    if(Write->PacketNum == 0) {
        This->PacketCurID = -1;
        // printf("[%s]start PacketNum:%d,id:%d\n", __FUNCTION__,Write->PacketNum,This->PacketCurID);
        form_main->createForm(FORM_UPDATE_USER,NULL);
        LocalQueryExec(This->Sql,"BEGIN TRANSACTION");
        sprintf(SqlStr, "Insert Into UserInfo(Code,IP,Roomname) Values(?,?,?)");
        This->Sql->prepare(This->Sql,SqlStr);
    }
    // printf("[%s]PacketNum:%d,DownLoadCount:%d,id:%d,count:%d\n",
            // __FUNCTION__,
            // Write->PacketNum,
            // Write->DownLoadCount,
            // This->PacketCurID,
            // This->Count);
	if(Write->PacketNum > This->PacketCurID) {		//����
		SendMessage(Screen.hUpdate,MSG_UPDATE_WTD,0,0);
		This->PacketCurID++;
		// printf("cont:%d,this->Count:%d,num:%d,id:%d\n",
				// Write->DownLoadCount,This->Count,Write->PacketNum,This->PacketCurID);
		int i;
		for(i=0;i<This->Count;i++) {
            This->Sql->reset(This->Sql);
            This->Sql->bind_text(This->Sql,1,UserInfo->Code);
            This->Sql->bind_text(This->Sql,2,UserInfo->IP);
            This->Sql->bind_text(This->Sql,3,UserInfo->Name);
            This->Sql->step(This->Sql);
			// sprintf(SqlStr,"Insert Into UserInfo(Code,IP,Roomname) Values(\"%s\",\"%s\",\"%s\") ",
					// UserInfo->Code,UserInfo->IP,UserInfo->Name);
			// LocalQueryExec(This->Sql,SqlStr);
			UserInfo++;
		}

		char buff[100]={0};
		DOWNDATA *DownIP = (DOWNDATA *)buff;
		DownIP->Cmd = IP_OK;											//����Ӧ��
		DownIP->Count =This->Count;										//���յ���������
		DownIP->DownLoadCount =1;										//Ӧ���
		DownIP->PacketNum = This->PacketCurID;							//��ǰ����
		Public.SendUdpData(This->cIP,This->Port,TP_COMPARELIST,buff,sizeof(buff));		//����Ӧ��ɹ�
		if (This->Count >= (Write->DownLoadCount - Write->PacketNum * 11)) { // ���һ�����ر������ݿ�
			// printf("[%s]end PacketNum:%d,id:%d\n", __FUNCTION__,Write->PacketNum,This->PacketCurID);
			WatchDogClose();
            This->Sql->finalize(This->Sql);
			LocalQueryExec(This->Sql,"COMMIT");
			LocalQueryExec(LocalQuery, "REINDEX index_user");
			WatchDogOpen();
#if (defined TC3000_X6)
			Public.LoadHomeList();
#endif
			if (sqlCheck(&LocalQuery,"DataBase.db",1) == TRUE) {//���ݿ�
				backData("DataBase.db");
			}
			
			char buf[64] = {0};
			Public.IsDownLoadUseInfo = 1;
			sprintf(buf,"%d",Public.IsDownLoadUseInfo);
			SetValueToEtcFile_Ext("interface.ini","PUBLIC","IsDownLoadUseInfo",buf);
			
			SendMessage(Screen.hUpdate,MSG_UPDATE_USER_SUCCESS,0,0);
		}
	} else if(Write->PacketNum == This->PacketCurID) {
		printf("[%s]PacketNum:%d\n", __FUNCTION__,Write->PacketNum);
		LocalQueryExec(This->Sql,"BEGIN TRANSACTION");
		char buff[100]={0};
		DOWNDATA *DownIP = (DOWNDATA *)buff;
		DownIP->Cmd = IP_OK;											//����Ӧ��
		DownIP->Count =This->Count;										//���յ���������
		DownIP->DownLoadCount =1;										//Ӧ���
		DownIP->PacketNum = This->PacketCurID;							//��ǰ����
		Public.SendUdpData(This->cIP,This->Port,TP_COMPARELIST,buff,sizeof(buff));		//����Ӧ��ɹ�
	}
	return 1;
}

/**
 * @brief ReadAllIDCard ��ȡȫ���û�ID������
 *
 * @param This
 * @param Read // ��������
 *
 * @return ID������
 */
static int ReadAllIDCard(DownLoadData *This,DOWNDATA *Read)
{
	int CardCount = 0;
	IDCARD *IdCard = (IDCARD*)Read->Data;
	char SqlStr[512] = "Select Code,IP,CardCode,Type,HintAudio From IDCard";

	if(Read->Cmd == IDCARD_READALL){
		LocalQueryOpen(This->Sql,"Select Count(*) As Count From IDCard");
		CardCount = LocalQueryOfInt(This->Sql,"Count");
		LocalQueryOpen(This->Sql,SqlStr);
		char SendBuf[1300]={0};									//���ͻ�����
		DOWNDATA *Id = (DOWNDATA*)SendBuf;
		Id->Cmd = IDCARD_READALL;
		IdCard = (IDCARD *)Id->Data;
		int i;
		Id->PacketNum = 0;
		if(CardCount%10)
			Id->DownLoadCount = CardCount/10+1;
		else
			Id->DownLoadCount = CardCount/10;
		int CardNum=0;
		int num = This->Sql->RecordCount(This->Sql);
		while(num--) {
			IdCard = (IDCARD *)Id->Data;
			for( i=0;i<10;i++) {		//ȡ����
				CardNum++;
				if(CardNum>CardCount)
					break;
				LocalQueryOfChar(This->Sql,"Code",IdCard->Code,12);
				LocalQueryOfChar(This->Sql,"IP",IdCard->IP,16);
				LocalQueryOfChar(This->Sql,"CardCode",IdCard->CardCode,12);
				LocalQueryOfChar(This->Sql,"HintAudio",IdCard->HintAudio,20);
				IdCard->CardType = LocalQueryOfInt(This->Sql,"Type");
				IdCard++;
				This->Sql->Next(This->Sql);
			}
			Id->Count = i;												//��ǰ�����͵���������

			Public.SendUdpData(This->cIP,UDPSERVER_PORT,TP_USERCARD,SendBuf,sizeof(SendBuf));
			// ::Sleep(10);
			Id->PacketNum = Id->PacketNum +1;										//����
		}
		This->Sql->Close(This->Sql);
	}
	return CardCount;
}

/**
 * @brief ReadOneIDCard ��ȡ��һ�û�ID������
 *
 * @param This
 * @param Read // ��������
 *
 * @return
 */
static int ReadOneIDCard(DownLoadData *This,DOWNDATA *Read)
{
	int CardCount = 0;
	IDCARD *IdCard = (IDCARD*)Read->Data;
	char SqlStr[512];
	if(Read->Cmd == IDCARD_READONE){
		sprintf(SqlStr,"Select Code,IP,CardCode,Type,HintAudio From IDCard Where CardCode='%s'",
				IdCard->CardCode);
		LocalQueryOpen(This->Sql,SqlStr);
		char SendBuf[300]={0};									//���ͻ�����
		DOWNDATA *Id = (DOWNDATA*)SendBuf;
		Id->Cmd = IDCARD_READONE;
		IDCARD *Card = (IDCARD *)Id->Data;
		int num = This->Sql->RecordCount(This->Sql);
		while(num--) {
			CardCount++;
			LocalQueryOfChar(This->Sql,"Code",IdCard->Code,12);
			LocalQueryOfChar(This->Sql,"IP",IdCard->IP,16);
			LocalQueryOfChar(This->Sql,"CardCode",IdCard->CardCode,12);
			LocalQueryOfChar(This->Sql,"HintAudio",IdCard->HintAudio,20);
			IdCard->CardType = LocalQueryOfInt(This->Sql,"Type");
			Card++;
			This->Sql->Next(This->Sql);
		}
		This->Sql->Close(This->Sql);
		Id->Count = CardCount;
		Id->DownLoadCount = CardCount;
		Id->PacketNum = 0;
		Public.SendUdpData(This->cIP,UDPSERVER_PORT,TP_USERCARD,SendBuf,sizeof(SendBuf));
	}
	return CardCount;
}

/**
 * @brief ReadIDCardRecord ��ȡID��ˢ����¼
 *
 * @param This
 *
 * @return
 */
static int ReadIDCardRecord(DownLoadData *This)
{

	int RecordCount = 0;
	char SendBuf[600]={0};									//���ͻ�����
	DOWNDATA *Id = (DOWNDATA*)SendBuf;
	LocalQueryOpen(This->Sql,"Select Count(*) As Count From IDCardRecord");
	RecordCount = This->Sql->RecordCount(This->Sql);
	if(RecordCount == 0)
		return 0;
	if(RecordCount%20)													//���㷢�Ͱ���
		Id->DownLoadCount = RecordCount/RECORDCOUNT+1;
	else
		Id->DownLoadCount = RecordCount/RECORDCOUNT;
	char SqlStr[512] = "Select CardCode,dTime From IDCardRecord ";
	LocalQueryOpen(This->Sql,SqlStr);
	Id->Cmd = IDCARD_IDCARDRECORD;
	int i;
	Id->PacketNum = -1;
	int CardNum=0;
	CARDRCORD *IdCard;
	int num = This->Sql->RecordCount(This->Sql);
	while(num--) {
		IdCard = (CARDRCORD *)Id->Data;
		for( i=0;i<RECORDCOUNT;i++)	{									//ȡ����
			CardNum++;
			if(CardNum>RecordCount)
				break;
			LocalQueryOfChar(This->Sql,"CardCode",IdCard->Code,12);
			LocalQueryOfChar(This->Sql,"dTime",IdCard->dTime,28);
			IdCard++;
			This->Sql->Next(This->Sql);
		}
		Id->Count = i;													//��ǰ�����͵���������
		Id->PacketNum = Id->PacketNum +1;										//����
		Public.SendUdpData(This->cIP,UDPSERVER_PORT,TP_USERCARD,SendBuf,sizeof(SendBuf));
	}
	This->Sql->Close(This->Sql);
	return 1;
}

/**
 * @brief AddIdCard ���Ӽ�¼
 *
 * @param This
 * @param Data // ��������
 *
 * @return
 */
static int AddIdCard(DownLoadData *This,DOWNDATA *Data)
{
	char buff[100]={0};
	DOWNDATA *DownIP = (DOWNDATA *)buff;
	DownIP->Cmd = IDCARD_OK;											//Ӧ��
	DownIP->Count =1;													//������
	DownIP->DownLoadCount =1;											//Ӧ���
	DownIP->PacketNum = 0;
	IDCARD *IdCard = (IDCARD *)Data->Data;
	if(Data->Count == 0) {
		DownIP->Cmd = IDCARD_ERROR;										//�޸�ʧ��,������
		Public.SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));		//����Ӧ��ɹ�
		return 0; // 20110302 xiehb
	}
	char SqlStr[512];
	sprintf(SqlStr,"Select Count(*) As Count From IDCard Where CardCode='%s'",
			IdCard->CardCode);
	LocalQueryOpen(This->Sql,SqlStr);
	int tmp = LocalQueryOfInt(This->Sql,"Count");
	This->Sql->Close(This->Sql);
	if(tmp>0) {
		DownIP->Cmd = IDCARD_ERROR;										//�޸�ʧ��,������
		Public.SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));		//����Ӧ��ɹ�
		return 0; // 20110302 xiehb
	}
	sprintf(SqlStr,"Insert Into IDCard(Code,IP,CardCode,Type,HintAudio) Values(\"%s\",\"%s\",\"%s\",%d,\"%s\")",
			IdCard->Code,IdCard->IP,IdCard->CardCode,IdCard->CardType,IdCard->HintAudio);
	LocalQueryExec(This->Sql,SqlStr);
	Public.SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));		//����Ӧ��ɹ�
	return 1;
}

/**
 * @brief DelIdCard ɾ����¼
 *
 * @param This
 * @param Data // ��������
 *
 * @return
 */
static int DelIdCard(DownLoadData *This,DOWNDATA *Data)
{
	char buff[100]={0};
	DOWNDATA *DownIP = (DOWNDATA *)buff;
	DownIP->Cmd = IDCARD_OK;											//Ӧ��
	DownIP->Count =1;													//������
	DownIP->DownLoadCount =1;											//Ӧ���
	DownIP->PacketNum = 0;
	IDCARD *IdCard = (IDCARD *)Data->Data;
	if(Data->Count == 0) {
		DownIP->Cmd = IDCARD_ERROR;										//�޸�ʧ��,������
		Public.SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));		//����Ӧ��ɹ�
		return 0; // 20110302 xiehb
	}
	char SqlStr[512];
	sprintf(SqlStr,"Delete From IDCard Where CardCode=%s\"",
			IdCard->CardCode);
	LocalQueryExec(This->Sql,SqlStr);
	//SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));	//����Ӧ��ɹ�
	Public.SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));		//����Ӧ��ɹ�
	return 1;
}

/**
 * @brief DeleteAllIDCard ɾ������ID������
 *
 * @param This
 *
 * @return
 */
static int DeleteAllIDCard(DownLoadData *This)
{
	char buff[100]={0};
	DOWNDATA *DownIP = (DOWNDATA *)buff;
	DownIP->Cmd = IDCARD_OK;											//Ӧ��
	DownIP->Count =1;													//������
	DownIP->DownLoadCount =1;											//Ӧ���
	DownIP->PacketNum = 0;

	LocalQueryExec(This->Sql,"Delete From IDCard");

	Public.SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));		//����Ӧ��ɹ�
	return 1;
}

/**
 * @brief ModifyIdCard �޸Ŀ���¼
 *
 * @param This
 * @param Data
 *
 * @return
 */
static int ModifyIdCard(DownLoadData *This,DOWNDATA *Data)
{
	char buff[100]={0};
	DOWNDATA *DownIP = (DOWNDATA *)buff;
	DownIP->Cmd = IDCARD_OK;											//Ӧ��
	DownIP->Count =1;													//������
	DownIP->DownLoadCount =0;											//Ӧ���
	DownIP->PacketNum = 0;
	IDCARD *IdCard = (IDCARD *)Data->Data;
	if(Data->Count == 0) {
		DownIP->Cmd = IDCARD_ERROR;										//�޸�ʧ��,������
		Public.SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));
		return 0; // 20110302 xiehb
	}
	char SqlStr[512];
	sprintf(SqlStr,"Select Count(*) As Count From IDCard Where CardCode='%s'",IdCard->CardCode);
	LocalQueryOpen(This->Sql,SqlStr);
	int tmp = LocalQueryOfInt(This->Sql,"Count");
	This->Sql->Close(This->Sql);
	if(tmp == 0) {
		DownIP->Cmd = IDCARD_ERROR;										//�޸�ʧ��,������
		Public.SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));
		return 0; // 20110302 xiehb
	}
	sprintf(SqlStr,"Update IDCard Set Code=%s,IP=%s,CardCode=%s,Type=%d,HintAudio=%s",
			IdCard->Code,IdCard->IP,IdCard->CardCode,IdCard->CardType,IdCard->HintAudio);
	LocalQueryExec(This->Sql,SqlStr);
	Public.SendUdpData(This->cIP,This->Port,TP_USERCARD,buff,sizeof(buff));		//����Ӧ��ɹ�
	return 1;
}

/**
 * @brief AddUserInfo �����û���Ϣ
 *
 * @param This
 * @param Data
 *
 * @return
 */
static int AddUserInfo(DownLoadData *This,DOWNDATA *Data)
{
	char buff[100]={0};
	DOWNDATA *DownIP = (DOWNDATA *)buff;
	DownIP->Cmd = IP_OK;												//Ӧ��
	DownIP->Count =1;													//������
	DownIP->DownLoadCount =0;											//Ӧ���
	DownIP->PacketNum = 0;
	IPADDRESS *UserInfo = (IPADDRESS *)Data->Data;
	if(Data->Count == 0) {
		DownIP->Cmd = IP_ERROR;											//�޸�ʧ��,������
		Public.SendUdpData(This->cIP,This->Port,TP_COMPARELIST,buff,sizeof(buff));
		return 0; // 20110302 xiehb
	}
	char SqlStr[512];
	sprintf(SqlStr,"Select Count(*) As Count From UserInfo Where Code='%s'",UserInfo->Code);
	LocalQueryOpen(This->Sql,SqlStr);
	int tmp = LocalQueryOfInt(This->Sql,"Count");
	This->Sql->Close(This->Sql);
	if(tmp>0) {
		DownIP->Cmd = IP_ERROR;											//�޸�ʧ��,������
		Public.SendUdpData(This->cIP,This->Port,TP_COMPARELIST,buff,sizeof(buff));										//����ʧ���Ѿ�����
		return 0; // 20110302 xiehb
	}
	sprintf(SqlStr,"Insert Into UserInfo(Code,IP) Values(%s,%s)",
			UserInfo->Code,UserInfo->IP);
	LocalQueryExec(This->Sql,SqlStr);
	Public.SendUdpData(This->cIP,This->Port,TP_COMPARELIST,buff,sizeof(buff));		//����Ӧ��ɹ�
	return 1;
}

/**
 * @brief DelUserInfo ɾ���û���Ϣ
 *
 * @param This
 * @param Data
 *
 * @return
 */
static int DelUserInfo(DownLoadData *This,DOWNDATA *Data)
{
	char buff[100]={0};
	DOWNDATA *DownIP = (DOWNDATA *)buff;
	DownIP->Cmd = IP_OK;												//Ӧ��
	DownIP->Count =1;													//������
	DownIP->DownLoadCount =0;											//Ӧ���
	DownIP->PacketNum = 0;
	IPADDRESS *UserInfo = (IPADDRESS *)Data->Data;
	if(Data->Count == 0) {
		DownIP->Cmd = IP_ERROR;											//�޸�ʧ��,������
		Public.SendUdpData(This->cIP,This->Port,TP_COMPARELIST,buff,sizeof(buff));		//����Ӧ��ɹ�
		return 0; // 20110302 xiehb
	}
	char SqlStr[512];
	sprintf(SqlStr,"Delete From UserInfo Where Code=%s",
			UserInfo->Code);
	LocalQueryExec(This->Sql,SqlStr);
	Public.SendUdpData(This->cIP,This->Port,TP_COMPARELIST,buff,sizeof(buff));		//����Ӧ��ɹ�
	return 1;
}

/**
 * @brief DelAllUserInfo ɾ�������û���Ϣ
 *
 * @param This
 *
 * @return
 */
static int DelAllUserInfo(DownLoadData *This)
{
	char buff[100]={0};
	DOWNDATA *DownIP = (DOWNDATA *)buff;
	DownIP->Cmd = IP_OK;												//Ӧ��
	DownIP->Count =1;													//������
	DownIP->DownLoadCount =0;											//Ӧ���
	DownIP->PacketNum = 0;

	LocalQueryExec(This->Sql,"Delete From UserInfo where Code<>\"0000\"");
	Public.SendUdpData(This->cIP,This->Port,TP_COMPARELIST,buff,sizeof(buff));
	return 1;
}

/**
 * @brief ModifyUserInfo �޸��û���Ϣ
 *
 * @param This
 * @param Data
 *
 * @return
 */
static int ModifyUserInfo(DownLoadData *This,DOWNDATA *Data)
{
	char buff[100]={0};
	DOWNDATA *DownIP = (DOWNDATA *)buff;
	DownIP->Cmd = IP_OK;												//Ӧ��
	DownIP->Count =1;													//������
	DownIP->DownLoadCount =0;											//Ӧ���
	DownIP->PacketNum = 0;
	IPADDRESS *UserInfo = (IPADDRESS *)Data->Data;
	if(Data->Count == 0) {
		DownIP->Cmd = IP_ERROR;											//�޸�ʧ��,������
		Public.SendUdpData(This->cIP,This->Port,TP_COMPARELIST,buff,sizeof(buff));		//����Ӧ��ɹ�
		return 0; // 20110302 xiehb
	}
	char SqlStr[512];
	sprintf(SqlStr,"Select Count(*) As Count From UserInfo Where Code = %s",UserInfo->Code);
	LocalQueryOpen(This->Sql,SqlStr);
	int tmp = LocalQueryOfInt(This->Sql,"Count");
	This->Sql->Close(This->Sql);
	if(tmp == 0) {
		DownIP->Cmd = IP_ERROR;											//�޸�ʧ��,������
		Public.SendUdpData(This->cIP,This->Port,TP_COMPARELIST,buff,sizeof(buff));										//����ʧ���Ѿ�������
		return 0; // 20110302 xiehb
	}
	sprintf(SqlStr,"Update UserInfo Set IP=%s Where Code=%s",
			UserInfo->IP,UserInfo->Code);
	LocalQueryExec(This->Sql,SqlStr);
																//��ǰ����
	Public.SendUdpData(This->cIP,This->Port,TP_COMPARELIST,buff,sizeof(buff));		//����Ӧ��ɹ�
	return 1;
}

/**
 * @brief ReadAllUserIP ��ȡȫ���û�IP��Ϣ
 *
 * @param This
 * @param Read
 *
 * @return
 */
static int ReadAllUserIP(DownLoadData *This,DOWNDATA *Read)
{
	int CardCount = 0;
	IPADDRESS *UserInfo = (IPADDRESS*)Read->Data;
	// char SqlStr[512] = "Select Code,IP From UserInfo";

	if(Read->Cmd == IP_READALL){
		LocalQueryOpen(This->Sql,"Select Count(*) As Count From UserInfo");
		CardCount = This->Sql->RecordCount(This->Sql);
		LocalQueryOpen(This->Sql,"Select Code,IP From UserInfo");
		char SendBuf[1300]={0};									//���ͻ�����
		DOWNDATA *Id = (DOWNDATA*)SendBuf;

		Id->Cmd = IP_READALL;
		UserInfo = (IPADDRESS *)Id->Data;
		Id->DownLoadCount = CardCount;
		int i;
		Id->PacketNum = 0;
		CardCount = 0;
		int num = Id->DownLoadCount;
		while(num--) {
			UserInfo = (IPADDRESS *)Id->Data;
			memset(Id->Data,0,1240);
			for( i=0;i<10;i++) {										//ȡ����
				CardCount++;
				if(CardCount>Id->DownLoadCount)
					break;
				LocalQueryOfChar(This->Sql,"Code",UserInfo->Code,12);
				LocalQueryOfChar(This->Sql,"IP",UserInfo->IP,16);
				UserInfo++;
				This->Sql->Next(This->Sql);
			}
			Id->Count = i;												//��ǰ�����͵���������
			Id->PacketNum = Id->PacketNum +1;							//����
			Public.SendUdpData(This->cIP,UDPSERVER_PORT,TP_COMPARELIST,SendBuf,sizeof(SendBuf));
		}
		This->Sql->Close(This->Sql);
	}
	return CardCount;
}

/**
 * @brief ReadOneUserIP ��ȡ�û�IP��Ϣ
 *
 * @param This
 * @param Read
 *
 * @return
 */
static int ReadOneUserIP(DownLoadData *This,DOWNDATA *Read)
{
	int CardCount = 0;
	IPADDRESS *UserInfo = (IPADDRESS*)Read->Data;
	char SqlStr[512];
	if(Read->Cmd == IP_READONE){
	sprintf(SqlStr,"Select Code,IP From UserInfo Where Code='%s'",UserInfo->Code);
	LocalQueryOpen(This->Sql,SqlStr);
		char SendBuf[400]={0};									//���ͻ�����
		DOWNDATA *Id = (DOWNDATA*)SendBuf;

		Id->Cmd = IP_READONE;
		IPADDRESS *User = (IPADDRESS *)Id->Data;
		int num = This->Sql->RecordCount(This->Sql);
		while(num--) {
			CardCount++;
			LocalQueryOfChar(This->Sql,"Code",UserInfo->Code,12);
			LocalQueryOfChar(This->Sql,"IP",UserInfo->IP,16);
			User++;
			This->Sql->Next(This->Sql);
		}
		Id->Count = CardCount;
		Id->DownLoadCount = CardCount;
		Id->PacketNum = 0;
		Public.SendUdpData(This->cIP,UDPSERVER_PORT,TP_COMPARELIST,SendBuf,sizeof(SendBuf));
	}
	return CardCount;
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief ReloadSql �����ݿ���ʱ���ָ����ݺ����¼������ݿ�
 *
 * @param This
 * @param Sql
 */
/* ---------------------------------------------------------------------------*/
static void ReloadSql(DownLoadData *This,struct _TSqlite *Sql)
{
	This->Sql = Sql;
}
/**
 * @brief ProcData ��������
 *
 * @param This
 * @param Comm ��������
 * @param Type TYPE_IDCARD:ID������
 *			   TYPE_COMPARELIST:���Ŷ��ձ�
 *
 * @return
 */
static int ProcData(DownLoadData *This,DOWNDATA *Comm,int Type)
{
	if(Type == TYPE_IDCARD) {
		DBG_P("IDCARD:");
		switch(Comm->Cmd)
		{
			case IDCARD_DOWNLOAD:
				DBG_P("DOWNLOAD\n");
				DownLoadIDCardData_2(This,Comm);
				break;
			case IDCARD_DOWNLOAD2:
				DBG_P("DOWNLOAD2\n");
				DownLoadIDCardData_2(This,Comm);
				break;
			case IDCARD_ADD:
				DBG_P("ADD\n");
				AddIdCard(This,Comm);
				break;
			case IDCARD_DEL:
				DBG_P("DEL\n");
				DelIdCard(This,Comm);
				break;
			case IDCARD_MOIDFIE:
				DBG_P("MOIDFIE\n");
				ModifyIdCard(This,Comm);
				break;
			case  IDCARD_READALL:
				DBG_P("READALL\n");
				ReadAllIDCard(This,Comm);
				break;
			case  IDCARD_READONE:
				DBG_P("READONE\n");
				ReadOneIDCard(This,Comm);
				break;
			case IDCARD_IDCARDRECORD:
				DBG_P("IDCARDRECORD\n");
				ReadIDCardRecord(This);
				break;
			case IDCARD_DELALLRECORD:
				DBG_P("DELALLRECORD\n");
				DeleteAllIDCard(This); // 20110228 xiehb ����ɾ������ID����Ϣ����
				break;
		}
	} else if(Type == TYPE_COMPARELIST) {
		DBG_P("COMPARELIST:");
		switch(Comm->Cmd)
		{
			case IP_DOWNLOAD:
				DBG_P("DOWNLOAD\n");
				DownLoadUserInfoData_2(This,Comm);
				break;
			case IP_DOWNLOAD2:
				DBG_P("DOWNLOAD2\n");
				DownLoadUserInfoData_2(This,Comm);
				break;
			case  IP_DEL:
				DBG_P("DEL\n");
				DelUserInfo(This,Comm);
				break;
			case IP_MOIDFIE:
				DBG_P("MOIDFIE\n");
				ModifyUserInfo(This,Comm);
				break;
			case  IP_ADD:
				DBG_P("ADD\n");
				AddUserInfo(This,Comm);
				break;
			case IP_READONE:
				DBG_P("READONE\n");
				ReadOneUserIP(This,Comm);
				break;
			case IP_READALL:
				DBG_P("READALL\n");
				ReadAllUserIP(This,Comm);
				break;
			case IP_DELALL: // 20110228 xiehb ����ɾ�������û�IP��Ϣ����
				DBG_P("DELALL\n");
				DelAllUserInfo(This);
				break;
		}
	}
	return 1;
}

/**
 * @brief downloadDataCreate �������ض���
 *
 * @param Sql �õ������ݿ��ļ�
 *
 * @return
 */
DownLoadData *downloadDataCreate(TSqlite *Sql)
{
	DownLoadData *This = (DownLoadData *)malloc(sizeof(DownLoadData));
	memset(This,0,sizeof(DownLoadData));
	This->Sql = Sql;
	This->PacketCurID = -1;
	This->Count = 0;
	This->DownLoadCount = 0;
	This->ProcData = ProcData;
    This->reloadSql = ReloadSql;
	return This;
}
