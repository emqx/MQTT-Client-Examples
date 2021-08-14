#ifndef JS_API_INIT_H__
#define JS_API_INIT_H__

#include <jerry_util.h>

#ifdef __cplusplus
extern "C"
{
#endif

int js_api_init(jerry_value_t obj);
int js_api_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
