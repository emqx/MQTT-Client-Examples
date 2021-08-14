/*
 * File      : jerry_util.c
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-05     realthread   the first version
 */
#ifndef JERRY_UTIL_H__
#define JERRY_UTIL_H__

#include <rtconfig.h>
#include <jerryscript-ext/handler.h>

#ifndef RT_JS_MALLOC
#define RT_JS_MALLOC     rt_malloc
#endif

#ifndef RT_JS_REALLOC
#define RT_JS_REALLOC    rt_realloc
#endif

#ifndef RT_JS_FREE
#define RT_JS_FREE       rt_free
#endif

#ifndef RT_JS_STRDUP
#define RT_JS_STRDUP     rt_strdup
#endif

#define DECLARE_HANDLER(NAME)       \
    static jerry_value_t            \
    NAME ## _handler (const jerry_value_t func_value,   \
                        const jerry_value_t this_value, \
                        const jerry_value_t args[],     \
                        const jerry_length_t args_cnt )

#define REGISTER_HANDLER(NAME)      \
    jerryx_handler_register_global ( (jerry_char_t *)# NAME, NAME ## _handler)
#define REGISTER_HANDLER_ALIAS(NAME, HANDLER) \
    jerryx_handler_register_global ( (jerry_char_t *)# NAME, HANDLER)

#define REGISTER_METHOD(OBJ, NAME)  \
    js_add_function (OBJ, # NAME, NAME ## _handler)
#define REGISTER_METHOD_ALIAS(OBJ, NAME, HANDLER) \
    js_add_function (OBJ, # NAME, HANDLER)

#ifdef RT_USING_FINSH
#include <finsh.h>
#define JS_MODULE(NAME, MODULE_INIT)    \
    FINSH_FUNCTION_EXPORT_CMD(MODULE_INIT, __jsm_ ##NAME, RT_NULL)
#else
#define JS_MODULE(NAME, MODULE_INIT)
#endif

#define JS_STRING_BUFSZ     64

#ifdef __cplusplus
extern "C"
{
#endif

typedef void(*js_util_user)(void);

bool is_utf8_string(const char *str, unsigned int size);

void js_set_property(const jerry_value_t obj, const char *name, const jerry_value_t prop);
jerry_value_t js_get_property(const jerry_value_t obj, const char *name);
bool js_delete_property(const jerry_value_t obj, const char *name);
void *js_get_pointer_property(const jerry_value_t obj, const char *name, const jerry_object_native_info_t **out_native_info_p);

void js_set_string_property(const jerry_value_t obj, const char *name, const char* value);
void js_set_boolean_property(const jerry_value_t obj, const char *name, bool value);
void js_set_number_property(const jerry_value_t obj, const char *name, double value);
void js_set_pointer_property(const jerry_value_t obj, const char *name, void *value, const jerry_object_native_info_t *native_info_p);

void js_add_function(const jerry_value_t obj, const char *name, jerry_external_handler_t func);
jerry_value_t js_call_func_obj(const jerry_value_t func_obj_val, const jerry_value_t this_val,
                               const jerry_value_t args_p[], jerry_size_t args_count);
jerry_value_t js_call_function(const jerry_value_t obj, const char *name,
                               const jerry_value_t args[], jerry_size_t args_cnt);

jerry_value_t js_string_to_value(const char *value);
char *js_value_to_string(const jerry_value_t value);
char *js_value_get_string(const jerry_value_t value, char *str_buf, unsigned int str_buf_len);

bool jerry_resolve_error(jerry_value_t ret_value);

void js_value_dump(const jerry_value_t value);
int js_read_file(const char* filename, char **script);

int js_util_init(void);
int js_util_cleanup(void);
#ifndef JS_LOCK_DUMP_ENABLE
int js_util_lock(void);
int js_util_lock_timeout(rt_int32_t time);
int js_util_unlock(void);
#ifdef JS_ISLOCK_CHECK
int js_util_islock(void);
#endif
#else
int _js_util_lock(const char* func, int line);
int _js_util_lock_timeout(rt_int32_t time, const char* func, int line);
int _js_util_unlock(const char* func, int line);

#define js_util_lock()  _js_util_lock(__FUNCTION__, __LINE__)
#define js_util_lock_timeout(time)  _js_util_lock_timeout(time, __FUNCTION__, __LINE__)                 
#define js_util_unlock()    _js_util_unlock(__FUNCTION__, __LINE__)

#endif
void js_util_user_init(js_util_user func);
void js_util_user_cleanup(js_util_user func);

int js_console_init(void);

#ifdef __cplusplus
}
#endif

#endif
