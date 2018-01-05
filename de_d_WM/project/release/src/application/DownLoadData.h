/*
 * =====================================================================================
 *
 *       Filename:  DownLoadData.h
 *
 *    Description:  ���ź�ID,IC��������
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


	//�ļ���������
	enum{
		TYPE_NULL,
		TYPE_PICTURE,		// ͼƬ
		TYPE_AFFICHE,		// ����
		TYPE_IDCARD,		// Ѱ����
		TYPE_MP3,			// �û���
		TYPE_RING,			// ����
		TYPE_COMPARELIST,	// ����
		TYPE_KERNEL,		// IP���ձ�
		TYPE_FILESYSTEM,	// ϵͳ�ļ�����
		TYPE_USERFILE,		// 
		TYPE_UBOOT
	};		 

	//�ļ���������	
	enum{
		DOWNCMD_NULL,			// ���κβ���
		DOWNCMD_DOWNLOAD,		// ����
		DOWNCMD_READALL,		// ��ȡ�ļ�
		DOWNCMD_READONE,		// 
		DOWNCMD_IDCARDRECORD,	// ��ȡIDCARD��¼
		DOWNCMD_OK=0x100
	};	
	//IDCARD
	enum{
		IDCARD_NULL,							//���κβ���
		IDCARD_DOWNLOAD,						//����
		IDCARD_READALL,							//��ȡȫ������
		IDCARD_READONE,							//��ȡһ���û�������
		IDCARD_DEL,								//ɾ��һ������,����ID����
		IDCARD_ADD,								//����һ�����ݣ�����ID����
		IDCARD_MOIDFIE,							//�޸�һ�����ݣ�����ID����
		IDCARD_IDCARDRECORD,					//��ȡID����¼
		IDCARD_DELALLRECORD,					//ɾ������ID����¼ 20110228 xiehb
		IDCARD_DOWNLOAD2,						//���� ��Э��
		IDCARD_OK=0x100,						//OKӦ��
		IDCARD_ERROR							//ʧ��Ӧ��
	};
	//IP���ձ����ָ��
	enum{
		IP_NULL,								//���κβ���
		IP_DOWNLOAD,							//����
		IP_READALL,								//��ȡȫ������
		IP_READONE,								//��ȡһ���û�������
		IP_DEL,									//ɾ��һ������,����Code����
		IP_ADD,									//����һ�����ݣ�����Code����
		IP_MOIDFIE,								//�޸�һ�����ݣ�����Code����
		IP_DELALL,								//ɾ������IP ��Ϣ 20110228 xiehb
		IP_DOWNLOAD2,							//�°汾���� 20130906
		IP_OK=0x100,							//OKӦ��
		IP_ERROR								//ʧ��Ӧ��
	};
	//---------------------------------------------------------------------------------------------------------------
	typedef struct _DOWNIP{
		int Cmd;						//�������1���أ�2��ȡ
		int DownLoadCount;				//������������
		int PacketNum;					//��ǰ���ذ���
		int Count;						//��ǰ����������
		char Data[1];					//���������׵�ַ
	}DOWNDATA;
	//=========================================================================
	//�û����ձ����ݽṹ
	typedef struct _IPADDRESS{
		char Code[12];					//DA_A�ſڻ�������1~4�����ſڻ�Ϊ��ŷźã������ſڻ�Ϊ�������
		char IP[16];					//IP��ַ
	}IPADDRESS;
	typedef struct
	{
		char Code[12];					//DA_A�ſڻ�������1~4�����ſڻ�Ϊ��ŷźã������ſڻ�Ϊ�������
		char IP[16];					//IP��ַ
		char Roomname[36];
	}IPADDRESS2;

	// ʹ����Э��: IP_DOWNLOAD2 ���ط��Ŷ��ձ� 20130906
	typedef struct
	{
		char QH[4];			//�ں�
		char DH[4];			//����
		char DYH[4];		//��Ԫ��
		char CH[4];			//���
		char FH[4];			//����
		char Code[12]; 		//����
		char IP[16];    	//IP��ַ
		char Name[36];  	//��������
	} TUserTable2;
	//=========================================================================
	//�û�id�����ݽṹ
	typedef struct{
		char Code[12];				//�������
		char IP[16];				//IP��ַ
		char CardCode[12];		//����
		int	  CardType;			//�������ͣ�1�û�����2����Ѳ������3�������ſ�
		char HintAudio[20];		//��ʾ������
	}IDCARD;
	typedef struct{
		char Code[12];				//�������
		char dTime[28];			//ˢ��ʱ��
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
	//�û�id��ˢ����¼���ݽṹ
	struct _IDCARD{
		char CardCode[12];		//����
		SYSTIME  dTime;			//ˢ��ʱ��
	};

	// �°汾Э�� 20130909
	typedef struct {
		char QH[4];					//�ں�
		char DH[4];					//����
		char DYH[4];				//��Ԫ��
		char CH[4];					//���
		char FH[4];					//����
		char IP[16]; 				//IP��ַ
		char CardCode[12];	 		//����
		int  CardType; 				//�������ͣ�1�û�����2����Ѳ������3�������ſ�
		char HintAudio[20]; 		//��ʾ������
		char MaxUseDate[20];   		//����Ч��
		char UseState;         		//��ʹ��״̬��0ͣ�� 1 ��Ч 2 ��ʧ
		char LimitDate;        		//�Ƿ�����ʹ��ʱ�� 0������  1����
		char UseDay;           		//���ա���һ�������Ƿ���Ч 1 bit��ʾһ��
		char Resereve;         		//������������
		int  LimitStartTime1;       //��Ч��ʼʱ���1������Ϊ��λ ���磬����8:00 Ϊ 8*3600=28800
		int  LimitEndTime1;       	//��Ч����ʱ���1
		int  LimitStartTime2;       //��Ч��ʼʱ���2
		int  LimitEndTime2;       	//��Ч����ʱ���2
		int  LimitStartTime3;       //��Ч��ʼʱ���3
		int  LimitEndTime3;       	//��Ч����ʱ���3
	} TIDCardStruct2;
	//=========================================================================
	//---------------------------------------------------------------------------------------------------------------
	typedef struct _DownLoadData
	{
		int (*ProcData)(struct _DownLoadData *This,DOWNDATA *Write,int Type);//��������
		//	int (*ReadData)(struct _DownLoadData *This,DOWNDATA *Read,int Type);								//��ȡ��¼

		struct _TSqlite *Sql;			//ʹ�õ����ݿ�
		int Cmd;						//��������
		int DownLoadCount;				//������������
		int PacketID;					//��ǰ���ذ���
		int Count;						//��ǰ����������
		char cIP[16];					//����IP
		int Port;						//�˿ں�

		int PacketCurID;				//��¼��ǰ���ɣ�

		int (*ReadAllIDCard)(struct _DownLoadData *This,DOWNDATA *Read);
		int (*ReadOneIDCard)(struct _DownLoadData *This,DOWNDATA *Read);
		int (*DownLoadIDCardData)(struct _DownLoadData *This,DOWNDATA *Write);
		int (*DownLoadIDCardData_2)(struct _DownLoadData *This,DOWNDATA *Write);
		int (*AddIdCard)(struct _DownLoadData *This,DOWNDATA *Data);	//���Ӽ�¼
		int (*DelIdCard)(struct _DownLoadData *This,DOWNDATA *Data);	//ɾ����¼
		int (*DeleteAllIDCard)(struct _DownLoadData *This);				// ɾ�����п��� 20110228 xiehb
		int (*ModifyIdCard)(struct _DownLoadData *This,DOWNDATA *Data);	//���Ӽ�¼
		int (*ReadIDCardRecord)(struct _DownLoadData *This);			//��ȡID��ˢ����¼

		int (*ReadAllUserIP)(struct _DownLoadData *This,DOWNDATA *Read);//��ȡȫ��IP
		int (*ReadOneUserIP)(struct _DownLoadData *This,DOWNDATA *Read);	//��ȡһ��IP
		int (*DownLoadUserInfoData)(struct _DownLoadData *This,DOWNDATA *Write);
		int (*DownLoadUserInfoData_2)(struct _DownLoadData *This,DOWNDATA *Write);
		int (*AddUserInfo)(struct _DownLoadData *This,DOWNDATA *Data);		//�����û���Ϣ
		int (*DelUserInfo)(struct _DownLoadData *This,DOWNDATA *Data);		//ɾ���û���Ϣ
		int (*DelAllUserInfo)(struct _DownLoadData *This);					// ɾ�������û���Ϣ 20110228 xiehb
		int (*ModifyUserInfo)(struct _DownLoadData *This,DOWNDATA *Data);	//�޸��û���Ϣ
		void (*reloadSql)(struct _DownLoadData *This,struct _TSqlite *Sql);

	}DownLoadData;

	DownLoadData *downloadDataCreate(struct _TSqlite *Sql);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
