/*
 * =====================================================================================
 *
 *       Filename:  Monitor.h
 *
 *    Description:  创建Monitor对象
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
#ifndef _MY_MONITOR_H
#define _MY_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

	struct Monitor_Type {
		unsigned int img_width;
		unsigned int img_height;	
	};
	
	typedef struct _MyMonitor {
		void (*MonitorStart)(int VideoType); //开始侦测
		void (*MonitorStop)(void); //停止侦测
	}MyMonitor;

	MyMonitor* myMonitorCreate();

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
