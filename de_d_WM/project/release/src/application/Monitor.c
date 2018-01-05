/*
 * =====================================================================================
 *
 *       Filename:  Monitor.c
 *
 *    Description:  摄像头侦测报警
 *
 *        Version:  1.0
 *        Created:  2017-12-06
 *       Revision:  1.0
 *
 *         Author:  LEE
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
#include <unistd.h>

#include "externfunc.h"
#include "V4L.h"
#include "Misc.h"

unsigned int WidthValue = 0;
unsigned int HeightValue = 0;
static int MonitorExit = 0;

struct Monitor_Type monitor_type[] = {
	{320,	240},
	{640,	480},
	{1280,	720},
};

volatile uint32_t u32MDJudgeTime;
volatile uint32_t u32CurrTime;

uint32_t GetTime(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)(tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

static void * MonitorThreadExecute(void){
	uint8_t *pu8PicPtr;
	uint64_t u64TS;
	uint32_t u32PicPhyAdr;
	uint32_t MonitorDetValue = 0;
	u32MDJudgeTime = GetTime() + 2000;
	printf("[%s] ------------------------------>4 \n",__FUNCTION__);
	while(1){

		if(MonitorExit){
			break;
		}
		
		if(ReadV4LPicture(&pu8PicPtr, &u64TS, &u32PicPhyAdr) == ERR_V4L_SUCCESS){ 
			TriggerV4LNextFrame();
		}
		else{
			ERR_PRINT("Read V4L Error\n");
		}

		u32CurrTime = GetTime();
		
		if(u32CurrTime > u32MDJudgeTime) {			
			MonitorDetValue = MotionDetV4LJudge();
			printf("Check Detvalue-----------[%d]-------------\n",MonitorDetValue);
			if(MonitorDetValue == 0){
				//u32MDJudgeTime = u32CurrTime;
				PlayMp3->Play(PlayMp3,"res/wav/dodo.mp3",0,NULL);
			}
			
		//	printf("Detect motion count %d, current time= %d\n", , u32CurrTime);			
			//u32MDJudgeTime = u32CurrTime + 2000;
		//	printf("Detect motion time= %d\n",u32MDJudgeTime);
		}
		DelayMs(10);
	}
}

static void MonitorThreadCreate(void)
{
	int result;
	pthread_t Monitor_pthread;
	struct sched_param SchedParam;	//优先级
	pthread_attr_t threadAttr1;				//线程属性

	SchedParam.sched_priority = 5;
	pthread_attr_init(&threadAttr1);

	pthread_attr_setschedparam(&threadAttr1, &SchedParam);
	pthread_attr_setdetachstate(&threadAttr1,PTHREAD_CREATE_DETACHED);	//设置线程为自动销毁
	result = pthread_create(&Monitor_pthread,&threadAttr1,MonitorThreadExecute,NULL);
	if(result) {
		printf("%s() pthread failt,Error code:%d\n", __FUNCTION__, result);
	}
	pthread_attr_destroy(&threadAttr1);		//释放附加参数
}

void MyMonitorStart(int videotype){
	MonitorExit = 0;
	printf("[%s] ------------------------------>1 \n",__FUNCTION__);
	WidthValue = monitor_type[videotype].img_width;
	HeightValue = monitor_type[videotype].img_height;
	if(InitV4LDevice(WidthValue, HeightValue) != ERR_V4L_SUCCESS) {
		printf("Init V4L device fail\n");
	}
	printf("[%s] ------------------------------>2 \n",__FUNCTION__);
	MotionDetV4LSetThreshold(20);
	printf("Ready to query user control\n");
	
	SetV4LEncode(0, WidthValue, HeightValue, 50);
	
	StartV4LCapture();
	printf("Start capturing\n");
	
	MonitorThreadCreate();
	
}

void MyMonitorStop(){

	MonitorExit = 1;
	
	FinializeV4LDevice();
}


/* ----------------------------------------------------------------*/
/**
 * @brief myMonitorCreate 创建Monitor对象
 *
 *
 * @returns Monitor对象
 */
/* ----------------------------------------------------------------*/
MyMonitor* myMonitorCreate()
{
	MyMonitor *this = (MyMonitor *)malloc(sizeof(MyMonitor));
	memset(this,0,sizeof(MyGpio));
	
	this->MonitorStart = MyMonitorStart;
	this->MonitorStop = MyMonitorStop;

	return this;
}

