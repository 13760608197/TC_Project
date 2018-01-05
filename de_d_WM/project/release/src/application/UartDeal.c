/*
 * =============================================================================
 *
 *       Filename:  UartDeal.c
 *
 *    Description:  ��������Э��(���ͼ�����)
 *
 *        Version:  1.0
 *        Created:  2015-11-13 10:56:26
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =============================================================================
 */
/* ---------------------------------------------------------------------------*
 *                      include head files
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

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
#if DBG_UART_DEAL > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

/* ---------------------------------------------------------------------------*
 *                      variables define
 *----------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*/
/**
 * @brief FromSingleChip ���յ�Uart������Ϣ
 *
 * @param This
 */
/* ---------------------------------------------------------------------------*/
static void FromSingleChip(UartDeal *This)
{
	int i;
	int k = 0;
	unsigned char check = 0;
	unsigned char buff[1024] = {0};
	int Address ;
	int Function;
	int Longer = 0;
	int Len;
	// static BOOL IDCardUnlockFlg = FALSE; // 20110704 xiehb ID��������־

	Len = This->uart->RecvBuffer(This->uart,buff,sizeof(buff));
	if (Len <= 0) {
		return;
	}
	for(i=0; i<Len; i++){
		if(buff[i] == 0x47){
			int length = buff[i+5]*0x100+buff[i+6];
			if (i+length+8 > 1024) {
				break;
			}
			if(buff[i+length+8] == 0x0D){
				k = i;
				Longer = length;
				break;
			}
		}
	}
	for(i=k+1; i<Longer+7+k; i++){
		check += buff[i];
	}

	if(check != buff[k+Longer+7]){
		return;
	}

	Function = buff[k+3]*0x100 + buff[k+4];
	Address = buff[k+1]*0x100 + buff[k+2];
	DBG_P("UART-->");
	switch(Function)
	{
		case 0x100:	//��������
			{
				DBG_P("Keyfun:%d\n",buff[k+7]);
				if(Longer == 1) {			//�˳����״̬��־
					unsigned char KeyVal = buff[k+7];
					screensaverStart(LCD_ON);
					form_main->keyNotify((WPARAM)KeyVal);
				}
			} break;

		case 0x101:	//�ſڻ��Ų𱨾�
			{
				DBG_P("Ubroken!\n");
				Public.SendUdpTData(Public.pCenterMsg->IP, UDPSERVER_PORT,
						TP_ALARM,
						Word[WORD_MACHINE_BROKEN].string,
					   	sizeof(Word[WORD_MACHINE_BROKEN].string),
						NULL, NULL);
				form_main->createForm(FORM_UNBROKEN,NULL);
				Public.bUnbroken = TRUE;
			} break;

		case 0x20A: // ��ȡ��Ƭ������汾��
			{
				DBG_P("SingleVersion\n");
				if(buff[k+6] == 3) {  // �汾�����ݳ��� 3
					Public.m_MCUVer.Major = buff[k+7]-'0';
					Public.m_MCUVer.Minor = buff[k+8]-'0';
					Public.m_MCUVer.Release = buff[k+9]-'0';
					Public.m_MCUVer.Reserve = 0;
				}
			} break;

		case 0x20B: // ���ű���
			{
				DBG_P("OpenDoorAlarm:%d\n",buff[k+7]);
				if(buff[k+7] == 2) { // ���������ϱ�
					Public.SendDoorOpenWarn(1,
							LOG_DR_DOOR,
							1200,
							Word[WORD_DOOR_OPEN_TOO_LONG].string,
							strlen(Word[WORD_DOOR_OPEN_TOO_LONG].string));
				// } else if (buff[k+7] == 0){	// �����ϱ�
				} else if (buff[k+7] == 1){ // �����ϱ�
					if (form_main->timerGetState(IDC_TIMER_UNLOCK_FAIL_10S)) {
						form_main->timerStop(IDC_TIMER_UNLOCK_FAIL_10S);
						Public.SendUnlockRec(1);
					}
				}
			} break;

		case 0x301:	//ˢ������
			{
#if 1
#if DBG_UART_DEAL > 0
				int i;
				DBG_P("Card:[%d]",Longer);
				for (i=0;i<Longer;i++) {
					DBG_P("%c",buff[k+7+i]);
				}
				DBG_P("\n");
#endif
				if((Address != 0x100) && (Address != 0x200))
					break;
				char buf[50];
				memset(buf,0,sizeof(buf));
				memcpy(buf,&buff[k+7],Longer);
				if (Card) {
					Card->RFCard_Deal(Card,buf);
				}
#else
#if DBG_UART_DEAL > 0
				int i;
				DBG_P("Card:[%d]",Longer);
				for (i=0;i<Longer;i++) {
					DBG_P("%02x",buff[k+7+i]);
				}
				DBG_P("\n");
#endif
				if((Address != 0x100) && (Address != 0x200))
					break;
				if(Longer <10) {
					char buf[17] = {0};
					char temp[3] = {0};
					char buf52[17] = {0};
					int i;
					for (i=0; i<Longer; i++) {
						sprintf(temp,"%02x",buff[k+7+i]);
						strcat(buf,temp);
					}
					unsigned long long int tmp_num = strtoull(buf,NULL,16);
					if (tmp_num & 0x0000000000ffffff) {
						hexToChar(tmp_num,buf52,52);
					} else {
						sprintf(buf52,"%llu",tmp_num >> 24);
					}
					if (Card) {
						Card->RFCard_Deal(Card,buf52);
					}
				}
#endif
			} break;

		default:
			DBG_P("Default cmd:%#03x,data:%d\n",Function,buff[k+7]);
			break;
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief ToSingleChip ���������Ƭ��
 *
 * @param This
 * @param Cmd ����
 * @param ... ���ñ���ʱ��,����ˢ�����ʹ��
 */
/* ---------------------------------------------------------------------------*/
static void ToSingleChip(UartDeal *This,int Cmd,...)
{
	unsigned int i;
	int check;
	unsigned int longer;
	if(Cmd == UART_AUDIOHINT) {
		return ;
	}
	This->SndData[0] = 0x47;
	This->SndData[1] = 0x01;	//��ַ
	This->SndData[2] = 0x00;
	This->SndData[3] = 0x02;	//������ֽ�
	This->SndData[5] = 0x00;	//����
	This->SndData[6] = 0x01;
	switch(Cmd)
	{
		case UART_CCDOPEN:
			{
				This->SndData[4] = 0x01; // �����
				This->SndData[7] = 0x01; // �����ֶ�
			} break;

		case UART_CCDCLOSE:
			{
				This->SndData[4] = 0x01;
				This->SndData[7] = 0x00;
			} break;

		case UART_ENWDT:
			{
				This->SndData[4] = 0x02;
				This->SndData[7] = 0x01;
			} break;

		case UART_DISWDT:
			{
				This->SndData[4] = 0x02;
				This->SndData[7] = 0x02;
			} break;

		case UART_WDT:
			{
				This->SndData[4] = 0x02;
				This->SndData[7] = 0x00;
			} break;

		case UART_ENALARM:
			{
				This->SndData[4] = 0x03;
				This->SndData[7] = 0x00;
			} break;

		case UART_DISALARM:
			{
				This->SndData[4] = 0x03;
				This->SndData[7] = 0x01;
			} break;

		case UART_CMDUNLOCK:
			{
				This->SndData[4] = 0x00;
				This->SndData[7] = 0x00;
			} break;

		case  UART_IDUNLOCK:
			{
				This->SndData[4] = 0x05;
				This->SndData[7] = 0x01;
			} break;

		case  UART_BUZZERLONG:
			{
				This->SndData[4] = 0x04;
				This->SndData[7] = 0x00;
			} break;

		case UART_BUZZERSHORT:
			{
				This->SndData[4] = 0x04;
				This->SndData[7] = 0x01;
			} break;

		case UART_MICOPEN://��MIC
			{
				This->SndData[4] = 0x06;
				This->SndData[7] = 0x00;
				break;
			}
		case UART_MICCLOSE://�ر�MIC
			{
				This->SndData[4] = 0x06;
				This->SndData[7] = 0x01;
			} break;

		case UART_RDIMEI:
			{
				This->SndData[4] = 0x08;
				This->SndData[7] = 0x00;
			} break;

		case UART_RESET:
			{
				This->SndData[4] = 0x09;
				This->SndData[7] = 0x00;
			} break;

		case UART_VERSION:
			{
				This->SndData[4] = 0x0A;
				This->SndData[7] = 0x00;
			} break;

		case UART_ALARMDOOR:  // add:xb 20151120
			{
				// �������������ñ���ʱ��
				int *pos = &Cmd + 1;
				This->SndData[4] = 0x0B;
				memcpy(&This->SndData[7],pos,sizeof(int));
			} break;

		case UART_CARD:  // add:xb 20151123
			{
				int *pos = &Cmd + 1;
				This->SndData[4] = 0x0C;
				memcpy(&This->SndData[7],pos,sizeof(int));
			} break;

		default:
			return ;
	}
	check = 0;
	// ����У��͵İ�ͷ������ 6
	longer = This->SndData[5]*256+This->SndData[6]+6;
	for(i=1;i<=longer;i++)
		check +=This->SndData[i];
	This->SndData[longer+1] = check;
	This->SndData[longer+2] = 0x0D;
	// 3 == ͬ��ͷ + У��� + ������    ȫ�����ݳ���
	This->uart->Send(This->uart,This->SndData,longer+3);

}

/* ---------------------------------------------------------------------------*/
/**
 * @brief LrcCheck �������Э��У����
 *
 * @param pElevator
 */
/* ---------------------------------------------------------------------------*/
// static void LrcCheck(Elevator *pElevator)
// {
	// unsigned char check = 0;
	// check += pElevator->Address;					//��ַ
	// check += pElevator->Cell;						//��Ԫ��
	// check += pElevator->Function;					//������
	// check += pElevator->StartFloor;					//��ʼ¥��
	// check += pElevator->EndFloor;					//����¥��
	// pElevator->Lrc = ~check+1;
// }

/* ---------------------------------------------------------------------------*/
/**
 * @brief FormatSendData ��ʽ�����͵���Э������
 *
 * @param buf  ������������
 * @param pElevator �������ݽṹ
 *
 * @returns
 */
/* ---------------------------------------------------------------------------*/
// static int FormatSendData(unsigned char *buf,Elevator *pElevator)
// {
	// unsigned char * p = (unsigned char *) pElevator;
	// int i=0,len,tmp;
	// unsigned char temp;
	// len = sizeof(Elevator)-3;
	// buf[i] = *p++;
	// for(i=0;i<len;i++)
	// {
		// tmp = i*2;
		// temp = *p++;
		// buf[tmp+1]=	temp>>4;
		// buf[tmp+2] = temp&0x0F;
		// if(buf[tmp+1]>9)
			// buf[tmp+1] += 0x37;
		// else
			// buf[tmp+1] += 0x30;
		// if(buf[tmp+2]>9)
			// buf[tmp+2] += 0x37;
		// else
			// buf[tmp+2] += 0x30;
	// }
	// tmp = i*2;
	// buf[tmp+1] = *p++;
	// buf[tmp+2] = *p++;
	// return 0;
// }

/* ---------------------------------------------------------------------------*/
/**
 * @brief ElevatorSndData ��Ƭ�����͵���Э��
 *
 * @param This
 * @param pElevator ���ݽṹ��
 */
/* ---------------------------------------------------------------------------*/
// static void ElevatorSndData(UartDeal *This,Elevator *pElevator)
// {
	// int Len=0;
	// LrcCheck(pElevator);
	// pElevator->head = 0x3A;
	// pElevator->Cr   = 0x0D;
	// pElevator->Lf   = 0x0A;
	// FormatSendData(This->SndData,pElevator);
	// Len = sizeof(Elevator)*2-3;
	// This->uart->Send(This->uart,This->SndData,Len);
// }

/* ---------------------------------------------------------------------------*/
/**
 * @brief WriteIMEI ��Ƭ��д�豸��� (3000 ��ʱ����)
 *
 * @param This
 * @param pImei �豸���
 * @param size ����
 */
/* ---------------------------------------------------------------------------*/
static void WriteIMEI(UartDeal *This,char *pImei, char size)
{
	unsigned int i;
	int check;
	unsigned int longer;

	This->SndData[0] = 0x47;
	This->SndData[1] = 0x01;				  //��ַ
	This->SndData[2] = 0x00;
	This->SndData[3] = 0x02;				  //������ֽ�
	This->SndData[4] = 0x07;
	This->SndData[5] = 0x00;				  //����
	This->SndData[6] = size;
	memcpy(&This->SndData[7], pImei, size); // ����

	longer = size+6;
	check = 0;
	for(i=1; i<=longer; i++) {
		check += This->SndData[i];
	}
	This->SndData[longer+1] = check;
	This->SndData[longer+2] = 0x0D;
	This->uart->Send(This->uart,This->SndData,longer+3);
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief UartDeal_Destory �ͷ�UART��Դ
 *
 * @param This
 */
/* ---------------------------------------------------------------------------*/
static void UartDeal_Destory(UartDeal *This)
{
	if (This) {
		This->uart->Destroy(This->uart);
		free(This);
	}
}

/* ---------------------------------------------------------------------------*/
/**
 * @brief uartDealCreate ����UART����ҵ�������
 *
 * @param hWnd ��ӦUI������
 *
 * @returns ����ָ��
 */
/* ---------------------------------------------------------------------------*/
UartDeal *uartDealCreate(HWND hWnd)
{
	UartDeal *This = (UartDeal*)calloc(sizeof(UartDeal),1);
	if(This == NULL)
		return NULL;
	This->hWnd = hWnd;
	This->uart = Create_UartServer(hWnd);
	if(This->uart->Open(This->uart,1,9600)==0) {
		printf("Err:com1 open fail!\n");
	}

	This->ToSingleChip = ToSingleChip;
	This->FromSingleChip =  FromSingleChip;
	// This->ElevatorSndData = ElevatorSndData;
	This->Destroy = UartDeal_Destory;

	return This;
}
