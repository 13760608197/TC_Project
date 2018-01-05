/*=============================================================================
#
# Filename: iic.h
#
# Company: �麣̫�������������ɷ����޹�˾
#
# Author: xubin
#
# Last modified: 2015-10-12 13:39
#
# Description: IIC�뵥Ƭ��ͨ��
#
=============================================================================*/
#ifndef _IIC_H
#define _IIC_H

typedef struct _IICDevice {

	int	dev_address;	 // �豸��ַ 
	void (*Open)(struct _IICDevice *this);
	void (*Close)(struct _IICDevice *this);
	void (*Set)(struct _IICDevice *this,int address);
	int (*Send)(struct _IICDevice *this,unsigned char *idata,int len);
	int (*Read)(struct _IICDevice *this,unsigned char *odata,int len);
	void (*Recv)(struct _IICDevice *this);
	void (*Clear)(struct _IICDevice *this);
	void (*Destroy)(struct _IICDevice *this);

}IIC_device;
IIC_device *iicDevicCreat(void);
#endif
