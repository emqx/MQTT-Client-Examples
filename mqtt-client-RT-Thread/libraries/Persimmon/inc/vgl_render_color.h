#ifndef __VGL_RENDER_COLOR_H
#define __VGL_RENDER_COLOR_H

#include "vgl_render.h"

vgl_render_t* vgl_render_color_new(vgl_surface_t *surface);
void vgl_render_color_set_color(vgl_render_t *ren, vgl_color_t color);

#endif
