/*
 * =====================================================================================
 *
 *       Filename:  update.c
 *
 *    Description:  ��������
 *
 *        Version:  1.0
 *        Created:  2015-11-20 10:50:14
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
#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <dirent.h>

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
#define DBG_UPDATE 0
#if DBG_UPDATE > 0
	#define DBG_P( x... ) printf( x )
#else
	#define DBG_P( x... )
#endif

/* ----------------------------------------------------------------*
 *                      variables define
 *-----------------------------------------------------------------*/


/* ----------------------------------------------------------------*/
/**
 * @brief UpdateThreadProc �����߳�ִ�к���
 *
 * @param arg
 *
 * @returns 
 */
/* ----------------------------------------------------------------*/
static void * UpdateThreadProc(void *arg)
{
	TUpdateRevertFile *recdata = (TUpdateRevertFile *)arg;
	DBG_P("ip:%s,file:%s\n",recdata->ip,recdata->file);

	TRemoteFile * Remote = CreateRemoteFile(recdata->ip,"Update1.cab"); // �˴�����Ҫ������������һ�����������
	if(Remote) {
		BOOL bRet = Remote->Download(Remote,Screen.hMainWnd,recdata->file,"/mnt/nand1-2/Update.cab",FALSE);
		Remote->Destroy(Remote);
		if(bRet) {
			SendMessage(Screen.hUpdate,MSG_UPDATE_SUCCESS,0,0);
			DBG_P("Reboot!! \n");
			// �޽���ʱ���
			// WatchDogClose();
			// exit(0);
		} else {
			SendMessage(Screen.hUpdate,MSG_UPDATE_FAIL,0,0);
			DBG_P("[Update] Download Error!\n");
		}
	} else {
		SendMessage(Screen.hUpdate,MSG_UPDATE_FAIL,0,0);
		DBG_P("Download fail!! \n");
	}
	free(recdata);
	pthread_exit(NULL);
}



/* ----------------------------------------------------------------*/
/**
 * @brief CreateUpdateThread ���������߳�
 *
 * @param arg
 */
/* ----------------------------------------------------------------*/
void CreateUpdateThread(void *arg)
{
	int result;
	pthread_t m_pthread;					//�̺߳�
	pthread_attr_t threadAttr1;				//�߳�����

	pthread_attr_init(&threadAttr1);		//���Ӳ���

//	TUpdateRevertFile *recdata = (TUpdateRevertFile *)arg;
//	DBG_P("ip:%s,file:%s\n",recdata->ip,recdata->file);

	pthread_attr_setdetachstate(&threadAttr1,PTHREAD_CREATE_DETACHED);	//�����߳�Ϊ�Զ�����
	result = pthread_create(&m_pthread,&threadAttr1,UpdateThreadProc,arg);		//�����̣߳��޴��ݲ���
	if(result) {
		printf("[%s] pthread failt,Error code:%d\n",__FUNCTION__,result);
	}
	pthread_attr_destroy(&threadAttr1);		//�ͷŸ��Ӳ���
}


//---------------------------------------------------------------------------

