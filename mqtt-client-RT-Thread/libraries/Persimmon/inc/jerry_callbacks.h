/*
 * File      : jerry_callback.h
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-05     realthread   the first version
 */
#ifndef JERRY_CALLBACKS_H__
#define JERRY_CALLBACKS_H__

#include <rtthread.h>
#include "jerryscript.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef rt_bool_t(*js_mq_func)(jerry_value_t parameter);
typedef js_mq_func js_callback_func;

void js_call_callback(jerry_value_t parameter);
rt_bool_t js_send_callback(js_callback_func callback, jerry_value_t parameter);
void js_mq_func_init(js_mq_func signal);
void js_mq_func_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
