/*
 * File      : rtgui_system.h
 * This file is part of RT-Thread GUI Engine
 * COPYRIGHT (C) 2006 - 2020, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-10-04     Bernard      first version
 */
#ifndef __RTGUI_SYSTEM_H__
#define __RTGUI_SYSTEM_H__

#include <rtthread.h>
#include <rtgui/rtgui.h>

#ifdef __cplusplus
extern "C" {
#endif

struct rtgui_dc;
struct rtgui_event;
struct rtgui_widget;

struct rtgui_timer;
typedef void (*rtgui_timeout_func)(struct rtgui_timer *timer, void *parameter);

enum rtgui_timer_state
{
    RTGUI_TIMER_ST_INIT,
    RTGUI_TIMER_ST_RUNNING,
    RTGUI_TIMER_ST_DESTROY_PENDING,
};

enum
{
    RTGUI_EMASK_VSYNC = 1 << 3,
    RTGUI_EMASK_MESSAGE = 1 << 5,
    RTGUI_EMASK_ALL = RTGUI_EMASK_VSYNC | RTGUI_EMASK_MESSAGE
};

struct rtgui_timer
{
    /* rt timer */
    struct rt_timer timer;
    /* How many events are pending on the queue. */
    rt_int16_t pending_cnt;
    rt_uint16_t state;

    /* timeout function and user data */
    rtgui_timeout_func timeout;
    void *user_data;
};
typedef struct rtgui_timer rtgui_timer_t;

#define RTGUI_TIMER_NAME    "rtgui\a"

rtgui_timer_t *rtgui_timer_create(rt_int32_t time, rt_int32_t flag, rtgui_timeout_func timeout, void *parameter);
void rtgui_timer_destory(rtgui_timer_t *timer);

void rtgui_timer_set_timeout(rtgui_timer_t *timer, rt_int32_t time);
rt_int32_t rtgui_timer_get_timeout(rtgui_timer_t *timer);
void rtgui_timer_start(rtgui_timer_t *timer);
void rtgui_timer_stop(rtgui_timer_t *timer);
int rtgui_timer_detach(void);

/* rtgui system initialization function */
int rtgui_system_server_init(void);
void rtgui_system_server_deinit(void);

void *rtgui_malloc(rt_size_t size);
void *rtgui_realloc(void *ptr, rt_size_t size);
void rtgui_free(void *ptr);
char *rtgui_strdup(const char *s);

void rtgui_set_mainwin_rect(struct rtgui_rect *rect);
void rtgui_get_mainwin_rect(struct rtgui_rect *rect);
void rtgui_get_screen_rect(struct rtgui_rect *rect);

void rtgui_screen_lock(rt_int32_t timeout);
void rtgui_screen_unlock(void);
int rtgui_screen_lock_freeze(void);
void rtgui_screen_lock_thaw(int value);

struct rtgui_event;
rt_err_t rtgui_post(struct rtgui_app* app, struct rtgui_event *event);
rt_err_t rtgui_send(struct rtgui_app* app, struct rtgui_event *event, rt_size_t event_size);
rt_err_t rtgui_send_urgent(struct rtgui_app* app, struct rtgui_event *event, rt_size_t event_size);
rt_err_t rtgui_send_sync(struct rtgui_app* app, struct rtgui_event *event, rt_size_t event_size);
rt_err_t rtgui_ack(struct rtgui_event *event, rt_int32_t status);
rt_err_t rtgui_recv(struct rtgui_event *event, rt_size_t event_size, rt_int32_t timeout);
rt_err_t rtgui_recv_filter(rt_uint32_t type, struct rtgui_event *event, rt_size_t event_size);

void* rtgui_sys_malloc(rt_size_t size);
void rtgui_sys_free(void *ptr);

int rtgui_sync_event(rt_int32_t timeout);
int rtgui_check_vsync(int clean);

#ifdef __cplusplus
}
#endif

#endif
