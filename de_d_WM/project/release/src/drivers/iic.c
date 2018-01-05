/*=============================================================================
#
# Filename: iic.c
#
# Company: �麣̫�������������ɷ����޹�˾
#
# Author: xubin
#
# Last modified: 2015-10-12 16:49
#
# Description: iic��д�ӿ�
#
=============================================================================*/
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

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "iic.h"

static int iic_fd = -1;

/**
 * @brief iicDevicSet ���ôӻ���ַ
 *
 * @param this
 * @param address �ӻ���ַ
 */
static void iicDevicSet(IIC_device *this,int address)
{
	this->dev_address = address;
}

/**
 * @brief iicDevicSend ��������
 *
 * @param this
 * @param data ��������
 * @param len  ���ݳ���
 *
 * @return  -1ʧ�� 0�ɹ�
 */
static int iicDevicSend(IIC_device *this,unsigned char *idata,int len)
{
	struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[1];

	messages[0].addr  = (this->dev_address >>1);
    messages[0].flags = 0;
    messages[0].len   = len;
    messages[0].buf   = idata;

    /* Transfer the i2c packets to the kernel and verify it worked */
    packets.msgs  = messages;
    packets.nmsgs = 1;
    if(ioctl(iic_fd, I2C_RDWR, &packets) < 0) {
		fprintf(stderr,"Err: file:%s,line:%d: %s\n",__FILE__,__LINE__,strerror(errno));
		return -1;
	}
    return 0;
}

/**
 * @brief iicDevicRead ��ȡIIC����
 *
 * @param this
 * @param odata ��ȡ������
 * @param len   ��ȡ�����ݳ��� 
 *
 * @return 
 */
static int iicDevicRead(IIC_device *this,unsigned char *odata,int len) 
{
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];
	messages[0].addr  = (this->dev_address >> 1);
	messages[0].flags = I2C_M_RD;
	messages[0].len   = len;
	messages[0].buf   = odata;

	/* Send the request to the kernel and get the result back */
	packets.msgs      = messages;
	packets.nmsgs     = 1;
	if(ioctl(iic_fd, I2C_RDWR, &packets) < 0) {
		fprintf(stderr,"Err: file:%s,line:%d: %s\n",__FILE__,__LINE__,strerror(errno));
		return -1;
	}
	return 0;
}

/**
 * @brief iicDevicDestory �ر�iic�ӿ�
 *
 * @param this
 */
static void iicDevicDestory(IIC_device *this)
{
	close(iic_fd);
	free(this);
	iic_fd = -1;
	this = NULL;
}

/**
 * @brief iicDevicCreat ����һ��iic�豸
 *
 * @param address �豸��ַ
 *
 * @return 
 */
IIC_device *iicDevicCreat(void)
{
	IIC_device *this = (IIC_device *)malloc(sizeof(IIC_device));
	if (iic_fd < 0) {
		iic_fd = open("/dev/i2c-0", O_RDWR);
		if(iic_fd < 0) {
			fprintf(stderr,"Err: file:%s,line:%d :%s\n",__FILE__,__LINE__,strerror(errno));
			return NULL;
		}
	}
	this->dev_address = 0;
	this->Set = iicDevicSet;
	this->Send = iicDevicSend;
	this->Read = iicDevicRead;
	this->Destroy = iicDevicDestory;
	return this;
}

