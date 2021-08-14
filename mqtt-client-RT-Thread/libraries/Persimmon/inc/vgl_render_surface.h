#ifndef __VGL_RENDER_SURFACE_H
#define __VGL_RENDER_SURFACE_H

#include "vgl_render.h"

vgl_render_t* vgl_render_surface_new(vgl_surface_t *surface);
void vgl_render_surface_set_surface(vgl_render_t *ren, vgl_surface_t *surface);

#endif
