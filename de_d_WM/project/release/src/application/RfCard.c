/*
 * =====================================================================================
 *
 *       Filename:  RfCard.c
 *
 *    Description:  ID,IC刷卡程序
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
 * @brief SaveCardRecord 保存刷卡记录
 *
 * @param Buf 卡号
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
	if(Ret>=500){		//最多保存500条
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

	return  Ret;                                                                    //返回卡的类型
}

/**
 * @brief FindCardCode 根据条件查找卡号
 *
 * @param Buf
 * @param DataType 1是根据IP查找用户房号 0是根据用户的卡号查找IP
 * @param Hint
 *
 * @return 0查找失败，1用户卡，2警卫卡巡更，3警卫开锁
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
		return  Ret;																	//返回卡的类型
	}

	// 根据卡号查IP
	sprintf(SqlStr,"Select IP From IDCard Where CardCode=\"%s\"",Tmp);
	LocalQueryOpen(This->Sql,SqlStr);
	num = This->Sql->RecordCount(This->Sql);
	This->Sql->Close(This->Sql);
	printf("sql:%s,num:%d\n",SqlStr,num);
	if(num == 0) { // 卡号对应IP不存在
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
	
	if(count <= 0){ // 已过期
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
	if((Ret <= CARDSTATUS_EMPTY) || (Ret > CARDSTATUS_TEMP)){  // 无效卡刷卡
		return  Ret;
	}

	int UseState = LocalQueryOfInt(This->Sql,"UseState");//查询卡是否有效
	if(UseState == CSTATE_STOP) {
		Ret = CARDSTATUS_STOPUSE;
	} else if(UseState == CSTATE_LOSS) {
		Ret = CARDSTATUS_LOSS;
	} else if(UseState == CSTATE_VALID) { // 有效卡
		BOOL bUseDayValib = FALSE;      // 使用日期符合条件的结果
		BOOL bTimeValib = FALSE; 		// 时间段限制是否符合条件 默认为不符合条件
		int LimitDate = LocalQueryOfInt(This->Sql,"LimitDate");//查询看是否有使用时间限制
		if(LimitDate==1){     // 有限制
			char UseDay = (char)LocalQueryOfInt(This->Sql,"UseDay");
			if(UseDay & 1<<Dt->tm_wday) // 星期限制条件符合
				bUseDayValib = TRUE;
			int wCurHour = Dt->tm_hour*3600 + Dt->tm_min*60 + Dt->tm_sec;    // 时间段限制
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
		if(LimitDate == 0 || (bUseDayValib && bTimeValib)) { // 卡无时间限制或在有效时段内
			// 取房号
			char Code[12];
			LocalQueryOfChar(This->Sql,"Code",Code,12);
			Public.EleVatorFloor = atoi(Code);
			Ret = LocalQueryOfInt(This->Sql,"Type");
			This->Sql->Next(This->Sql);
			// 取IP
			memset(Tmp,0,16);
			LocalQueryOfChar(This->Sql,"IP",Tmp,16);
			printf("IP = %s\n",Buf);
			// 取提示语音
			// wcscpy(HintAudio,sqlQuery.fieldValue(2));
			// WideCharToMultiByte(CP_OEMCP,NULL,HintAudio,-1,Hint,50,NULL,FALSE);			//返回语音提示
		} else {
			Ret = CARDSTATUS_OVERDUE;
		}
	}
	// This->Sql->Next(This->Sql);
	// printf("ElaVatorFloor:%d\n",Public.ElaVatorFloor);
	//
	//返回卡的类型
	return  Ret;
}

/* ----------------------------------------------------------------*/
/**
 * @brief RFCard_Deal ID,IC卡业务处理
 *
 * @param This
 * @param buf 卡号
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
				if(Public.eElevator) {		//用户刷卡电梯联动
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
					// uart->ElevatorSndData(uart,&lift);   //用户刷卡时，呼叫电梯

					uart->ToSingleChip(uart,UART_CARD,1);
					// form_main->timerStart(IDC_TIMER_SEND_CMD_CARD_100ms);
				} else {
					// 无电梯联动时直接发送消息
					uart->ToSingleChip(uart,UART_CARD,1);
				}

				//刷卡时，是否撤防，向室内机发送消息
				if(Public.IsAllowDisarm == 0){
					Public.SendUdpTData(buf, UDPSERVER_PORT,
							TP_USERCARD,
							idcode, strlen(idcode)+1,
							NULL, NULL);			//发消息通知室内机						
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
					// CallBackXunGeng, this);//巡更通知管理中心
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
				// 没有在数据库里查询到，需要将卡号发送到 FormIDCard 页面
				Public.SendUdpTData(Public.pCenterMsg->IP, UDPSERVER_PORT,
						TP_USERIDRECORD,
						idcode, strlen(idcode)+1,
						NULL, NULL);		//发卡号到中心
			} break;

		default:
			break;
	}
}
/* ---------------------------------------------------------------------------*/
/**
 * @brief RfCard_ReloadSql 碌卤脢媒戮脻驴芒脣冒禄碌脢卤拢卢禄脰赂麓卤赂路脻潞贸拢卢脰脴脨脗录脫脭脴脢媒戮脻驴芒
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
 * @brief RfCard_Destroy 释放资源
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
 * @brief rfCardCreate 创建IC,ID刷卡业务处理对象
 *
 * @param hWnd 对应UI主窗体
 * @param Sql 用到的数据库文件
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

