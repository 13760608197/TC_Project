/*
 * =====================================================================================
 *
 *       Filename:  RfCard.h
 *
 *    Description:  ID,ICˢ������
 *
 *        Version:  v.0
 *        Created:  2015-11-13 11:05:32 
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _RFCARD_H
#define _RFCARD_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

	enum //  ID(IC)������                       
	{                                           
		CARD_EMPTY,         // �տ�             
		CARD_USER,          // ס����           
		CARD_MANAGER,       // ��ܿ�           
		CARD_TEMP,          // ��ʱ��           
		CARD_INVALIDATION   // ��Ч��           
	};                                          

	enum // �û�ˢ����ķ��ض���                
	{                                           
		CARDSTATUS_EMPTY,   // �ղ���   ��Ч    
		CARDSTATUS_USER,    // �û�ˢ�� ��Ч    
		CARDSTATUS_MANAGER, // ���ˢ�� ��Ч    
		CARDSTATUS_TEMP,    // ��ʱ��   ��Ч    
		CARDSTATUS_STOPUSE, // ͣ�ÿ�   ��Ч    
		CARDSTATUS_LOSS,    // ��ʧ��   ��Ч    
		CARDSTATUS_OVERDUE, // ����     ��Ч    
		CARDSTATUS_INVALID  // ����ԭ����Ч��   
	};                                          

	enum                                        
	{                                           
		CSTATE_STOP,        // ����ͣ��         
		CSTATE_VALID,   // ����Ч               
		CSTATE_LOSS     // ���ѹ�ʧ             
	};                                          

	typedef struct _RfCard
	{
		struct _TSqlite *Sql;	
		HWND hWnd; // ���ھ�� 

		int (*SaveCardRecord)(struct _RfCard *This,char *Buf);
		int (*FindCardCode)(struct _RfCard *This,char *Buf,int DataType,char *Hint);
		void (*RFCard_Deal)(struct _RfCard *This,char *buf);
		
		void (*Destroy)(struct _RfCard *This);
		void (*reloadSql)(struct _RfCard *This,struct _TSqlite *Sql);
	}RfCard;

	RfCard* rfCardCreate(HWND hWnd,struct _TSqlite *Sql);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
