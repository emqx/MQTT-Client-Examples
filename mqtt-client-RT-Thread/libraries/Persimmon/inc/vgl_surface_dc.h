#ifndef __VGL_SURFACE_DC_H
#define __VGL_SURFACE_DC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vgl_typedef.h"

vgl_surface_t* vgl_create_dc_surface(struct rtgui_dc *dc);
vgl_surface_t* vgl_create_dc_surface_auto_destory(struct rtgui_dc *dc);

#ifdef __cplusplus
}
#endif

#endif
