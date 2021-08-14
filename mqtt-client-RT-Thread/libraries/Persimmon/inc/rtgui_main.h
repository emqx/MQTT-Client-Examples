/*
* File      : rtgui_main.h
* COPYRIGHT (C) 2012-2017, Shanghai Real-Thread Technology Co., Ltd
*
* Change Logs:
* Date           Author       Notes
* 2017-12-22     realthread   the first version
*/

#ifndef __RTGUI_MAIN_H__
#define __RTGUI_MAIN_H__

#include <rthw.h>
#include <rtthread.h>
#include <rtgui/event.h>
#include <rtgui/rtgui_server.h>


#ifdef __cplusplus
extern "C" {
#endif

extern void rtgui_main_app(void);

int rtgui_thread_init(void);

#ifdef __cplusplus
}
#endif

#endif
