#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <linux/types.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <linux/fs.h>

#include "externfunc.h"

#define IIC_DEVICE_ADD 0xC0


typedef struct IICDATA {
	int reg;
	int data;
} IIC_DATA;

typedef struct DATA {
	char reg[5];
	char data[5];
} DATA;

static DATA *iic_char_data;
static IIC_DATA *iic_data;
static int date_num;
static IIC_device *iic_dev;

/**
 * @brief read_from_echo_iic 读原有寄存器的值
 *
 * @return 0成功 -1失败
 */
static int read_from_echo_iic(void)
{
	unsigned char inbuf[2], outbuf[5];
	int i;
	for (i = 0; i < date_num; i++) {
		outbuf[0] = 0xFC;
		outbuf[1] = 0xF3;
		outbuf[2] = 0x37;
		outbuf[3] = (unsigned char)((iic_data[i].reg & 0xff00) >> 8);
		outbuf[4] = (unsigned char)(iic_data[i].reg & 0x00ff);

		if (iic_dev != NULL) {
			if (iic_dev->Send(iic_dev,outbuf,sizeof(outbuf))) {
				printf("iic send fail!\n");
				return -1;
			}
			if (iic_dev->Read(iic_dev,inbuf,sizeof(inbuf)) < 0) {
				printf("iic read fail!\n");
				return -1;
			}
		}
	}
	return 0;
}

/**
 * @brief write_iic_handle 写入iic数据
 */
static void write_iic_handle(void)
{
	int i;
	unsigned char *buf;
	int len = 7 * date_num;
	buf = (unsigned char *)malloc(sizeof(unsigned char) * len);

	for (i = 0; i < date_num; i++) {
		buf[0 + 7*i] = 0xFC;
		buf[1 + 7*i] = 0xF3;
		buf[2 + 7*i] = 0x3B;
		buf[3 + 7*i] = (unsigned char)((iic_data[i].reg & 0xff00) >> 8);
		buf[4 + 7*i] = (unsigned char)(iic_data[i].reg & 0x00ff);
		buf[5 + 7*i] = (unsigned char)((iic_data[i].data & 0xff00) >> 8);
		buf[6 + 7*i] = (unsigned char)(iic_data[i].data & 0x00ff);
	}

	if (iic_dev->Send(iic_dev,buf,len) < 0){
		printf("echo init fail!\n");
	} else {
		printf("echo init sucsess!\n");
	}
	free(buf);
}

/**
 * @brief Data_type_change 转换iic配置文件数据
 *
 * @param sr  输入 字符串数据
 * @param dir 输出 整型数据
 */
static void Data_type_change(DATA *sr,IIC_DATA *dir)
{
	int i,num;
	num = sizeof(sr)/sizeof(DATA);
	for (i=0; i<date_num; i++) {
		dir[i].reg = strtoul(sr[i].reg,NULL,16);
		dir[i].data = strtoul(sr[i].data,NULL,16);
	}
}

int echo_iic_load(void)
{
	GHANDLE hFile;
	char buf[10];
	int i;
	hFile = LoadEtcFile("iic.ini");

	if(GetIntValueFromEtc(hFile,"NUM","NUM",&date_num) != ETC_OK) {
		printf("load date_num error\n");
		return -1;
	}

	iic_char_data = (DATA *)malloc(sizeof(DATA) * date_num);
	iic_data = (IIC_DATA *)malloc(sizeof(IIC_DATA) * date_num);

	for (i=0; i<date_num; i++) {
		sprintf(buf,"REG%d",i+1);
		if(GetValueFromEtc(hFile,buf,"REG",iic_char_data[i].reg,sizeof(iic_char_data[i].reg)) != ETC_OK) {
			printf("load reg 0x%s error\n",iic_char_data[i].reg);
			return -1;
		}
		if(GetValueFromEtc(hFile,buf,"DATA",iic_char_data[i].data,sizeof(iic_char_data[i].data)) != ETC_OK) {
			printf("load data 0x%s error\n",iic_char_data[i].data);
			return -1;
		}
	}
	Data_type_change(iic_char_data,iic_data);

	Mygpio->SetValueNow(Mygpio,ENUM_GPIO_ECHO_RESET,IO_INACTIVE);
	usleep(3000);//复位3MS
	Mygpio->SetValueNow(Mygpio,ENUM_GPIO_ECHO_RESET,IO_ACTIVE);
	usleep(15000);//复位15MS
	Mygpio->SetValueNow(Mygpio,ENUM_GPIO_ECHO_RESET,IO_INACTIVE);
	usleep(15000);//复位15MS

	iic_dev = iicDevicCreat();
	// 设置从机地址
	iic_dev->Set(iic_dev,IIC_DEVICE_ADD);
	write_iic_handle();

	free(iic_char_data);
	free(iic_data);
	UnloadEtcFile(hFile);
	if (iic_dev != NULL)
		iic_dev->Destroy(iic_dev);
	return 0;
}

