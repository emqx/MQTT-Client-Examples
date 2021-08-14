
#ifndef __JS_PERSIM_H__
#define __JS_PERSIM_H__

#include <rthw.h>
#include <rtthread.h>
#include <rtgui/event.h>
#include <rtgui/rtgui_server.h>
#include <finsh.h>

#ifdef __cplusplus
extern "C" {
#endif

#define JS_WIN  0x01
#define JS_WIN_CLOSE  0x01

typedef void(*light_up_lcd)(void);

void rtgui_light_up_lcd(void *func);
void rtgui_logo_show(int timeSec);
rt_err_t rtgui_logo_hide(void);

void rtgui_logo_show_time(int ms);
int js_app_ready_sem_release(void);

int app_switch(const char *str);
int app_exit(void);

// int js_persim_thread_init(void);
int js_persim_thread_init(const char *str);

#ifdef __cplusplus
}
#endif

#endif
