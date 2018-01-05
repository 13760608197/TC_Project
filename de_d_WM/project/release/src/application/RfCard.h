/*
 * =====================================================================================
 *
 *       Filename:  RfCard.h
 *
 *    Description:  ID,IC刷卡处理
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

	enum //  ID(IC)卡类型                       
	{                                           
		CARD_EMPTY,         // 空卡             
		CARD_USER,          // 住户卡           
		CARD_MANAGER,       // 物管卡           
		CARD_TEMP,          // 临时卡           
		CARD_INVALIDATION   // 无效卡           
	};                                          

	enum // 用户刷卡后的返回动作                
	{                                           
		CARDSTATUS_EMPTY,   // 空操作   无效    
		CARDSTATUS_USER,    // 用户刷卡 有效    
		CARDSTATUS_MANAGER, // 物管刷卡 有效    
		CARDSTATUS_TEMP,    // 临时卡   有效    
		CARDSTATUS_STOPUSE, // 停用卡   无效    
		CARDSTATUS_LOSS,    // 挂失卡   无效    
		CARDSTATUS_OVERDUE, // 过期     无效    
		CARDSTATUS_INVALID  // 不明原因无效卡   
	};                                          

	enum                                        
	{                                           
		CSTATE_STOP,        // 卡已停用         
		CSTATE_VALID,   // 卡有效               
		CSTATE_LOSS     // 卡已挂失             
	};                                          

	typedef struct _RfCard
	{
		struct _TSqlite *Sql;	
		HWND hWnd; // 窗口句柄 

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
