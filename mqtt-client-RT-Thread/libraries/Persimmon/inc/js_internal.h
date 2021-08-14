#ifndef JS_INTERNAL_H__
#define JS_INTERNAL_H__

#include <rtdevice.h>
#include <rtthread.h>
#include <jerry_util.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef rt_device_t(*js_device_require)(const char *name);

void js_device_require_init(void *func);
rt_device_t js_device_find(const char *name);

int js_file_init(jerry_value_t obj);
int js_storage_init(jerry_value_t obj);
int js_navigate_init(jerry_value_t obj);
int js_systeminfo_init(jerry_value_t obj);

int js_serial_init(jerry_value_t obj);
int js_serial_deinit(void);

int js_animation_init(jerry_value_t obj);
int js_animator_init(jerry_value_t obj);

int js_timeout_init(jerry_value_t obj);

int js_audio_player_init(jerry_value_t obj);

int js_videoplayer_init(jerry_value_t obj);

int jerry_wifi_init(jerry_value_t obj);

int js_download_init(jerry_value_t obj);
int js_download_deinit(void);

int js_untar_init(jerry_value_t obj);
int js_untar_deinit(void);

int js_iic_bus_init(jerry_value_t obj);

int js_image_init(jerry_value_t obj);

int js_freetype_init(jerry_value_t obj);

int make_directory(const char *dir);

int js_canvas_init(jerry_value_t obj);

int js_canvas_lite_init(jerry_value_t obj);
int js_translate_init(jerry_value_t obj);

int js_load_init(jerry_value_t obj);

#ifdef __cplusplus
}
#endif

#endif
