#ifndef __LOAD_H__
#define __LOAD_H__

#include <rtthread.h>

#ifdef __cplusplus
extern "C" {
#endif

void pm_load_activate_tick(rt_tick_t tick);
void pm_load_idle_detect(void);
int pm_load_detach(void);

#ifdef __cplusplus
}
#endif

#endif
