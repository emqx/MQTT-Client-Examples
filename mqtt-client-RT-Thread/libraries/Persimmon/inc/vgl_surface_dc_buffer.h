#ifndef __VGL_SURFACE_DC_BUFFER_H
#define __VGL_SURFACE_DC_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vgl_typedef.h"

vgl_surface_t* vgl_create_dc_buffer_surface(struct rtgui_dc_buffer *dc);
void vgl_buffer_surface_divorced_dc(vgl_surface_t *surface);

#ifdef __cplusplus
}
#endif

#endif
