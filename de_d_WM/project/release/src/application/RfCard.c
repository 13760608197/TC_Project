/*
 * =====================================================================================
 *
 *       Filename:  RfCard.c
 *
 *    Description:  ID,ICˢ������
 *
 *        Version:  1.0
 *        Created:  2015-11-13 11:04:09
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
/* ----------------------------------------------------------------*
 *                      include head files
 *-----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

#include "externfunc.h"

/* ----------------------------------------------------------------*
 *                  extern variables declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                  internal functions declare
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                        macro define
 *-----------------------------------------------------------------*/

/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/

/**
 * @brief SaveCardRecord ����ˢ����¼
 *
 * @param Buf ����
 *
 * @return
 */
static int SaveCardRecord(RfCard *This,char *Buf)
{
	char SqlStr[512];
	int Ret = 0;
	LocalQueryOpen(This->Sql,"Select Count(* ) As Count From IDCardRecord");
	Ret = LocalQueryOfInt(This->Sql,"Count");
	This->Sql->Close(This->Sql);
	if(Ret>=500){		//��ౣ��500��
		LocalQueryOpen(This->Sql,"Select ID From IDCardRecord Order By ID Limit 1");
		Ret = LocalQueryOfInt(This->Sql,"ID");
		This->Sql->Close(This->Sql);
		sprintf(SqlStr,"Delete  From IDCardRecord Where ID =%d",Ret);
		LocalQueryExec(This->Sql,SqlStr);
	}
	char RecDate[50];
	GetDate(RecDate,sizeof(RecDate));

	sprintf(SqlStr,"Insert Into IDCardRecord(CardCode,dTime) Values(\"%s\",\"%s\")",Buf,RecDate);
	LocalQueryExec(This->Sql,SqlStr);

	return  Ret;                                                                    //���ؿ�������
}

/**
 * @brief FindCardCode �����������ҿ���
 *
 * @param Buf
 * @param DataType 1�Ǹ���IP�����û����� 0�Ǹ����û��Ŀ��Ų���IP
 * @param Hint
 *
 * @return 0����ʧ�ܣ�1�û�����2������Ѳ����3��������
 */
static int FindCardCode(RfCard *This,char *Buf,int DataType,char *Hint)
{
	char SqlStr[512];
	char *Tmp = Buf;
	int Ret = CARDSTATUS_INVALID;
	int num;
	if(DataType == 1) {
		sprintf(SqlStr,"Select Code,Type From IDCard Where IP=\"%s\"",Tmp);
		LocalQueryOpen(This->Sql,SqlStr);
		memset(Tmp,0,16);
		num = This->Sql->RecordCount(This->Sql);
		if(num) {
			char Code[12];
			LocalQueryOfChar(This->Sql,"Code",Code,12);
			Public.EleVatorFloor = atoi(Code);
			Ret = LocalQueryOfInt(This->Sql,"Type");
			This->Sql->Next(This->Sql);
		}
		return  Ret;																	//���ؿ�������
	}

	// ���ݿ��Ų�IP
	sprintf(SqlStr,"Select IP From IDCard Where CardCode=\"%s\"",Tmp);
	LocalQueryOpen(This->Sql,SqlStr);
	num = This->Sql->RecordCount(This->Sql);
	This->Sql->Close(This->Sql);
	printf("sql:%s,num:%d\n",SqlStr,num);
	if(num == 0) { // ���Ŷ�ӦIP������
		return CARDSTATUS_INVALID;
	}

	time_t timer;
	struct tm *Dt;
	timer = time(&timer);
	Dt = localtime(&timer);

	char RecDate[50];
	GetDate(RecDate,sizeof(RecDate));
	sprintf(SqlStr,"Select Count(*) As Count from IDCard Where '%s'<=DATETIME([MaxUseDate]) and CardCode=\"%s\"",
			RecDate,Tmp);
	LocalQueryOpen(This->Sql,SqlStr);
	int count = LocalQueryOfInt(This->Sql,"Count");
	This->Sql->Close(This->Sql);
	
	if(count <= 0){ // �ѹ���
		return CARDSTATUS_OVERDUE;
	}

	sprintf(SqlStr,"Select IP,Type,HintAudio,Code,UseState,LimitDate,UseDay,\
			LST1,LET1,LST2,LET2,LST3,LET3 From IDCard Where CardCode=\"%s\""
			,Tmp);
	LocalQueryOpen(This->Sql,SqlStr);
	num = This->Sql->RecordCount(This->Sql);
	if(num <= 0) {
		This->Sql->Close(This->Sql);
		return  Ret;
	}

	Ret = LocalQueryOfInt(This->Sql,"Type");
	if((Ret <= CARDSTATUS_EMPTY) || (Ret > CARDSTATUS_TEMP)){  // ��Ч��ˢ��
		return  Ret;
	}

	int UseState = LocalQueryOfInt(This->Sql,"UseState");//��ѯ���Ƿ���Ч
	if(UseState == CSTATE_STOP) {
		Ret = CARDSTATUS_STOPUSE;
	} else if(UseState == CSTATE_LOSS) {
		Ret = CARDSTATUS_LOSS;
	} else if(UseState == CSTATE_VALID) { // ��Ч��
		BOOL bUseDayValib = FALSE;      // ʹ�����ڷ��������Ľ��
		BOOL bTimeValib = FALSE; 		// ʱ��������Ƿ�������� Ĭ��Ϊ����������
		int LimitDate = LocalQueryOfInt(This->Sql,"LimitDate");//��ѯ���Ƿ���ʹ��ʱ������
		if(LimitDate==1){     // ������
			char UseDay = (char)LocalQueryOfInt(This->Sql,"UseDay");
			if(UseDay & 1<<Dt->tm_wday) // ����������������
				bUseDayValib = TRUE;
			int wCurHour = Dt->tm_hour*3600 + Dt->tm_min*60 + Dt->tm_sec;    // ʱ�������
			if(wCurHour>=LocalQueryOfInt(This->Sql,"LST1")
					&& wCurHour<=LocalQueryOfInt(This->Sql,"LET1")) {
				bTimeValib = TRUE;
			} else if(wCurHour>=LocalQueryOfInt(This->Sql,"LST2")
					&& wCurHour<=LocalQueryOfInt(This->Sql,"LET2")) {
				bTimeValib = TRUE;
			} else if(wCurHour>=LocalQueryOfInt(This->Sql,"LST3")
					&& wCurHour<=LocalQueryOfInt(This->Sql,"LET3")) {
				bTimeValib = TRUE;
			}
		}
		if(LimitDate == 0 || (bUseDayValib && bTimeValib)) { // ����ʱ�����ƻ�����Чʱ����
			// ȡ����
			char Code[12];
			LocalQueryOfChar(This->Sql,"Code",Code,12);
			Public.EleVatorFloor = atoi(Code);
			Ret = LocalQueryOfInt(This->Sql,"Type");
			This->Sql->Next(This->Sql);
			// ȡIP
			memset(Tmp,0,16);
			LocalQueryOfChar(This->Sql,"IP",Tmp,16);
			printf("IP = %s\n",Buf);
			// ȡ��ʾ����
			// wcscpy(HintAudio,sqlQuery.fieldValue(2));
			// WideCharToMultiByte(CP_OEMCP,NULL,HintAudio,-1,Hint,50,NULL,FALSE);			//����������ʾ
		} else {
			Ret = CARDSTATUS_OVERDUE;
		}
	}
	// This->Sql->Next(This->Sql);
	// printf("ElaVatorFloor:%d\n",Public.ElaVatorFloor);
	//
	//���ؿ�������
	return  Ret;
}

/* ----------------------------------------------------------------*/
/**
 * @brief RFCard_Deal ID,IC��ҵ����
 *
 * @param This
 * @param buf ����
 */
/* ----------------------------------------------------------------*/
static void RFCard_Deal(RfCard *This,char *buf)
{
	char idcode[20];
	char HintAudio[100]={0};
	int result;
	memset(idcode,0,sizeof(idcode));
	memcpy(idcode,buf,sizeof(idcode));
	// SaveCardRecord(This,idcode);
	// SendMessage(This->hWnd,MSG_MAIN_SHOW_DIAL,0,0);
	HWND Form = Screen.Find("TFrmSetAc");
	if (Form) {
		updateFormSetAccessCardNum(Form,buf);
		uart->ToSingleChip(uart,UART_CARD,1);
		return ;
	}
	result = FindCardCode(This,buf,0,HintAudio);
	switch(result)
	{
		case CARDSTATUS_EMPTY:
			break;

		case CARDSTATUS_USER:
			{
				Public.UnlockTheDoor(0);
				form_main->setTextOpration(Word[WORD_CARD_SUCCESS].string);
				if(Public.eElevator) {		//�û�ˢ����������
					TElevator lift;
					//lift.AddrH = Public.ElevatorAddr/0x100;
					lift.AddrH = Public.ElevatorAddr%0x100;
					lift.AddrL =  Public.EleVatorFloor/10000;
					lift.Function = 0x01;
					lift.CellCode= (char)(Public.ElevatorFloorCode&0xFF);
					lift.FloorCode= (char)(((Public.EleVatorFloor/100)%100)&0xFF);
					lift.Reserved = (char)(Public.ElevatorLobby);
					lift.RoomNumber = (char)((Public.EleVatorFloor%100)&0xFF);;
					Public.ElevatorSndData(&lift);
					// uart->ElevatorSndData(uart,&lift);   //�û�ˢ��ʱ�����е���

					uart->ToSingleChip(uart,UART_CARD,1);
					// form_main->timerStart(IDC_TIMER_SEND_CMD_CARD_100ms);
				} else {
					// �޵�������ʱֱ�ӷ�����Ϣ
					uart->ToSingleChip(uart,UART_CARD,1);
				}

				//ˢ��ʱ���Ƿ񳷷��������ڻ�������Ϣ
				if(Public.IsAllowDisarm == 0){
					Public.SendUdpTData(buf, UDPSERVER_PORT,
							TP_USERCARD,
							idcode, strlen(idcode)+1,
							NULL, NULL);			//����Ϣ֪ͨ���ڻ�						
				}


				Public.SendUdpTData(Public.pCenterMsg->IP, UDPSERVER_PORT,
						TP_USERIDRECORD,
						idcode, strlen(idcode)+1,
						NULL, NULL);
			} break;

		// case CARD_GUARD:
			// Public.SendUdpTData(Public.cMngCenterIP, UDPSERVER_PORT,
					// TP_GUARD,
					// idcode, strlen(idcode)+1,
					// CallBackXunGeng, this);//Ѳ��֪ͨ��������
			// break;

		case CARDSTATUS_MANAGER:
			{
				uart->ToSingleChip(uart,UART_CARD,1);
				Public.UnlockTheDoor(0);
				form_main->setTextOpration(Word[WORD_CARD_SUCCESS].string);
				Public.SendUdpTData(Public.pCenterMsg->IP, UDPSERVER_PORT,
						TP_USERIDRECORD,
						idcode, strlen(idcode)+1,
						NULL, NULL);
			} break;

		case CARDSTATUS_TEMP:
			{
				uart->ToSingleChip(uart,UART_CARD,1);
				Public.UnlockTheDoor(0);
				form_main->setTextOpration(Word[WORD_CARD_SUCCESS].string);
				Public.SendUdpTData(Public.pCenterMsg->IP, UDPSERVER_PORT,
						TP_USERIDRECORD,
						idcode, strlen(idcode)+1,
						NULL, NULL);
			} break;

		case CARDSTATUS_STOPUSE:
			{
				uart->ToSingleChip(uart,UART_CARD,0);
				Public.PlayVoice("CardStop");
				form_main->setTextOpration(Word[WORD_CARD_USELESS].string);
			} break;

		case CARDSTATUS_LOSS:
			{
				uart->ToSingleChip(uart,UART_CARD,0);
				Public.PlayVoice("CardLoss");
				form_main->setTextOpration(Word[WORD_CARD_LOSS].string);
			} break;

		case CARDSTATUS_OVERDUE:
			{
				uart->ToSingleChip(uart,UART_CARD,0);
				Public.PlayVoice("CardOverDue");
				form_main->setTextOpration(Word[WORD_CARD_OVERDUE].string);
			} break;

		case CARDSTATUS_INVALID:
			{
				uart->ToSingleChip(uart,UART_CARD,0);
				Public.PlayVoice("CardUnregist");
				form_main->setTextOpration(Word[WORD_CARD_UNREGIST].string);
				// û�������ݿ����ѯ������Ҫ�����ŷ��͵� FormIDCard ҳ��
				Public.SendUdpTData(Public.pCenterMsg->IP, UDPSERVER_PORT,
						TP_USERIDRECORD,
						idcode, strlen(idcode)+1,
						NULL, NULL);		//�����ŵ�����
			} break;

		default:
			break;
	}
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief RfCard_ReloadSql µ±Êý¾Ý¿âËð»µÊ±£¬»Ö¸´±¸·Ýºó£¬ÖØÐÂ¼ÓÔØÊý¾Ý¿â
 *
 * @param This
 * @param Sql
 */
/* ---------------------------------------------------------------------------*/
static void RfCard_ReloadSql(struct _RfCard *This,struct _TSqlite *Sql)
{
	This->Sql = Sql;
}

/* ----------------------------------------------------------------*/
/**
 * @brief RfCard_Destroy �ͷ���Դ
 *
 * @param This
 */
/* ----------------------------------------------------------------*/
static void RfCard_Destroy(RfCard *This)
{
	if (This) {
		free(This);
	}
}

/* ----------------------------------------------------------------*/
/**
 * @brief rfCardCreate ����IC,IDˢ��ҵ�������
 *
 * @param hWnd ��ӦUI������
 * @param Sql �õ������ݿ��ļ�
 *
 * @returns
 */
/* ----------------------------------------------------------------*/
RfCard* rfCardCreate(HWND hWnd,TSqlite *Sql)
{
	RfCard *This = (RfCard *)malloc(sizeof(RfCard));
	This->hWnd = hWnd;
	This->Sql = Sql;
	This->RFCard_Deal = RFCard_Deal;
	This->SaveCardRecord = SaveCardRecord;
	This->FindCardCode = FindCardCode;
	This->reloadSql = RfCard_ReloadSql;
	This->Destroy = RfCard_Destroy;
	return This;
}

