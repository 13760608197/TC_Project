/*
 * =====================================================================================
 *
 *       Filename:  DownLoadData.h
 *
 *    Description:  房号和ID,IC卡号下载
 *
 *        Version:  1.0
 *        Created:  2015-12-16 16:20:37 
 *       Revision:  none
 *
 *         Author:  xubin
 *        Company:  Taichuan
 *
 * =====================================================================================
 */
#ifndef _DOWNLOADDATA_H
#define _DOWNLOADDATA_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


	//文件下载类型
	enum{
		TYPE_NULL,
		TYPE_PICTURE,		// 图片
		TYPE_AFFICHE,		// 公告
		TYPE_IDCARD,		// 寻更卡
		TYPE_MP3,			// 用户卡
		TYPE_RING,			// 音乐
		TYPE_COMPARELIST,	// 铃声
		TYPE_KERNEL,		// IP对照表
		TYPE_FILESYSTEM,	// 系统文件更新
		TYPE_USERFILE,		// 
		TYPE_UBOOT
	};		 

	//文件操作命令	
	enum{
		DOWNCMD_NULL,			// 无任何操作
		DOWNCMD_DOWNLOAD,		// 下载
		DOWNCMD_READALL,		// 读取文件
		DOWNCMD_READONE,		// 
		DOWNCMD_IDCARDRECORD,	// 读取IDCARD记录
		DOWNCMD_OK=0x100
	};	
	//IDCARD
	enum{
		IDCARD_NULL,							//无任何操作
		IDCARD_DOWNLOAD,						//下载
		IDCARD_READALL,							//读取全部数据
		IDCARD_READONE,							//读取一个用户的数据
		IDCARD_DEL,								//删除一条数据,根据ID卡号
		IDCARD_ADD,								//增加一条数据，根据ID卡号
		IDCARD_MOIDFIE,							//修改一条数据，根据ID卡号
		IDCARD_IDCARDRECORD,					//读取ID卡记录
		IDCARD_DELALLRECORD,					//删除所有ID卡记录 20110228 xiehb
		IDCARD_DOWNLOAD2,						//下载 新协议
		IDCARD_OK=0x100,						//OK应答
		IDCARD_ERROR							//失败应答
	};
	//IP对照表操作指令
	enum{
		IP_NULL,								//无任何操作
		IP_DOWNLOAD,							//下载
		IP_READALL,								//读取全部数据
		IP_READONE,								//读取一个用户的数据
		IP_DEL,									//删除一条数据,根据Code卡号
		IP_ADD,									//增加一条数据，根据Code卡号
		IP_MOIDFIE,								//修改一条数据，根据Code卡号
		IP_DELALL,								//删除所有IP 信息 20110228 xiehb
		IP_DOWNLOAD2,							//新版本下载 20130906
		IP_OK=0x100,							//OK应答
		IP_ERROR								//失败应答
	};
	//---------------------------------------------------------------------------------------------------------------
	typedef struct _DOWNIP{
		int Cmd;						//控制命令，1下载，2读取
		int DownLoadCount;				//下载数据总数
		int PacketNum;					//当前下载包号
		int Count;						//当前包的数据量
		char Data[1];					//下载数据首地址
	}DOWNDATA;
	//=========================================================================
	//用户对照表数据结构
	typedef struct _IPADDRESS{
		char Code[12];					//DA_A门口机按键号1~4；栋门口机为层号放好，其他门口机为虚拟号码
		char IP[16];					//IP地址
	}IPADDRESS;
	typedef struct
	{
		char Code[12];					//DA_A门口机按键号1~4；栋门口机为层号放好，其他门口机为虚拟号码
		char IP[16];					//IP地址
		char Roomname[36];
	}IPADDRESS2;

	// 使用新协议: IP_DOWNLOAD2 下载房号对照表 20130906
	typedef struct
	{
		char QH[4];			//期号
		char DH[4];			//栋号
		char DYH[4];		//单元号
		char CH[4];			//层号
		char FH[4];			//房号
		char Code[12]; 		//卡号
		char IP[16];    	//IP地址
		char Name[36];  	//房间名称
	} TUserTable2;
	//=========================================================================
	//用户id卡数据结构
	typedef struct{
		char Code[12];				//房间号码
		char IP[16];				//IP地址
		char CardCode[12];		//卡号
		int	  CardType;			//卡的类型，1用户卡、2警卫巡更卡、3警卫开门卡
		char HintAudio[20];		//提示音内容
	}IDCARD;
	typedef struct{
		char Code[12];				//房间号码
		char dTime[28];			//刷卡时间
	}CARDRCORD;
	typedef struct
	{
		int Year;
		int Mon;
		int Day;
		int Hour;
		int Min;
		int Sec;
	}SYSTIME;
	//用户id卡刷卡记录数据结构
	struct _IDCARD{
		char CardCode[12];		//卡号
		SYSTIME  dTime;			//刷卡时间
	};

	// 新版本协议 20130909
	typedef struct {
		char QH[4];					//期号
		char DH[4];					//栋号
		char DYH[4];				//单元号
		char CH[4];					//层号
		char FH[4];					//房号
		char IP[16]; 				//IP地址
		char CardCode[12];	 		//卡号
		int  CardType; 				//卡的类型，1用户卡、2警卫巡更卡、3警卫开门卡
		char HintAudio[20]; 		//提示音内容
		char MaxUseDate[20];   		//卡有效期
		char UseState;         		//卡使用状态，0停用 1 有效 2 挂失
		char LimitDate;        		//是否限制使用时间 0无限制  1限制
		char UseDay;           		//周日、周一到周六是否有效 1 bit表示一天
		char Resereve;         		//保留，对齐用
		int  LimitStartTime1;       //有效起始时间段1，以秒为单位 例如，早上8:00 为 8*3600=28800
		int  LimitEndTime1;       	//有效结束时间段1
		int  LimitStartTime2;       //有效起始时间段2
		int  LimitEndTime2;       	//有效结束时间段2
		int  LimitStartTime3;       //有效起始时间段3
		int  LimitEndTime3;       	//有效结束时间段3
	} TIDCardStruct2;
	//=========================================================================
	//---------------------------------------------------------------------------------------------------------------
	typedef struct _DownLoadData
	{
		int (*ProcData)(struct _DownLoadData *This,DOWNDATA *Write,int Type);//下载数据
		//	int (*ReadData)(struct _DownLoadData *This,DOWNDATA *Read,int Type);								//读取记录

		struct _TSqlite *Sql;			//使用的数据库
		int Cmd;						//控制命令
		int DownLoadCount;				//下载数据总数
		int PacketID;					//当前下载包号
		int Count;						//当前包的数据量
		char cIP[16];					//中心IP
		int Port;						//端口号

		int PacketCurID;				//记录当前包ＩＤ

		int (*ReadAllIDCard)(struct _DownLoadData *This,DOWNDATA *Read);
		int (*ReadOneIDCard)(struct _DownLoadData *This,DOWNDATA *Read);
		int (*DownLoadIDCardData)(struct _DownLoadData *This,DOWNDATA *Write);
		int (*DownLoadIDCardData_2)(struct _DownLoadData *This,DOWNDATA *Write);
		int (*AddIdCard)(struct _DownLoadData *This,DOWNDATA *Data);	//增加记录
		int (*DelIdCard)(struct _DownLoadData *This,DOWNDATA *Data);	//删除记录
		int (*DeleteAllIDCard)(struct _DownLoadData *This);				// 删除所有卡号 20110228 xiehb
		int (*ModifyIdCard)(struct _DownLoadData *This,DOWNDATA *Data);	//增加记录
		int (*ReadIDCardRecord)(struct _DownLoadData *This);			//读取ID卡刷卡记录

		int (*ReadAllUserIP)(struct _DownLoadData *This,DOWNDATA *Read);//读取全部IP
		int (*ReadOneUserIP)(struct _DownLoadData *This,DOWNDATA *Read);	//读取一户IP
		int (*DownLoadUserInfoData)(struct _DownLoadData *This,DOWNDATA *Write);
		int (*DownLoadUserInfoData_2)(struct _DownLoadData *This,DOWNDATA *Write);
		int (*AddUserInfo)(struct _DownLoadData *This,DOWNDATA *Data);		//增加用户信息
		int (*DelUserInfo)(struct _DownLoadData *This,DOWNDATA *Data);		//删除用户信息
		int (*DelAllUserInfo)(struct _DownLoadData *This);					// 删除所有用户信息 20110228 xiehb
		int (*ModifyUserInfo)(struct _DownLoadData *This,DOWNDATA *Data);	//修改用户信息
		void (*reloadSql)(struct _DownLoadData *This,struct _TSqlite *Sql);

	}DownLoadData;

	DownLoadData *downloadDataCreate(struct _TSqlite *Sql);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
