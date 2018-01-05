/*----------------------------------------------------------------------------
// Copyright by zhuhai taichang 2006.5.24
// Author: guanshangming
//
// 管理共享内存区
// 方法:
// PShareMemory pObject;				//声明
// pObject = CreateShareMemory(Size);	//创建一个共享内存,Size表示每个缓冲区Buf的大小
// pObject->Destroy(pObject);			//销毁由CreateShareMemory创建的共享内存
// 写入数据时，先用SaveStart获得可写入的内存句柄，写入数据后，用SaveEnd释放该句柄
// 如果缓冲区满，则SaveStart阻塞，直至缓冲区被读取或对象被销毁
// 如果获得写句柄，返回句柄，如果对象被销毁，返回NULL
// 当释放写句柄后，需要传入缓冲区已填充的字节数
// pBuf = pObject->SaveStart(pObject);			//获得写句柄
// pObject->SaveEnd(pObject,WriteSize);			//写入WriteSize字节数,释放写句柄
// pBuf = pObject->GetStart(pObject,&BufSize);	//获得读句柄,BufSize指示缓冲区字节数
// pBuf = pObject->GetEnd(pObject);				//释放读句柄
//----------------------------------------------------------------------------*/

#ifndef ShareMemoryForGuestH
#define ShareMemoryForGuestH
#include <pthread.h>
#include <semaphore.h>

struct _ShareMemoryPrivateForGuest;

typedef struct _ShareMemoryForGuest
{
	struct _ShareMemoryPrivateForGuest *Private;
	unsigned int (*WriteCnt)(struct _ShareMemoryForGuest *This);		//返回写入的缓冲区数量
	char* (*SaveStart)(struct _ShareMemoryForGuest *This);				//获得写句柄
	void (*SaveEnd)(struct _ShareMemoryForGuest *This,int Size);		//释放写句柄，Size为写入字节数
	char* (*GetStart)(struct _ShareMemoryForGuest *This,int *Size);	//获得读句柄，Size为缓冲区字节数
	void (*GetEnd)(struct _ShareMemoryForGuest *This);					//释放读句柄
//	void (* InitSem)(struct _ShareMemoryForGuest *This);				//初始化信号
	void (* CloseMemory)(struct _ShareMemoryForGuest *This);			//结束
	void (* Destroy)(struct _ShareMemoryForGuest *This);				//销毁该ShareMemory
	void (* My_sem_post_get)(struct _ShareMemoryForGuest *This);
	void (* My_sem_post_save)(struct _ShareMemoryForGuest *This);
	void (* My_sem_wait_get)(struct _ShareMemoryForGuest *This);
	void (* My_sem_wait_save)(struct _ShareMemoryForGuest *This);
}ShareMemoryForGuest,*PShareMemoryForGuest;

PShareMemoryForGuest CreateShareMemoryForGuest(unsigned int Size,unsigned int BufCnt,int type);


#endif
