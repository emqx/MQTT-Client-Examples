/*
 * File      : jerry_module.c
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-05     realthread   the first version
 */
#ifndef JERRY_MODULE_H__
#define JERRY_MODULE_H__

#ifdef __cplusplus
extern "C"
{
#endif

char *js_module_dirname(char *path);
char *js_module_normalize_path(const char *directory, const char *filename);
bool  js_module_clear_cache(const jerry_value_t name);
int   js_module_init(void);
int   js_module_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
