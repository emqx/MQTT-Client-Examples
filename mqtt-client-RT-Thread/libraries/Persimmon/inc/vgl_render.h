#ifndef __VGL_RENDER_H
#define __VGL_RENDER_H

#include "vgl_typedef.h"

#define VGL_RENDER_COMMON   \
    vgl_surface_t *surface; \
    vgl_color_t *span_buf; \
    int buf_size; \
    void (*blend_span)(vgl_render_t *ren, int x0, int y0, int x1, unsigned alpha); \
    void (*destroy)(vgl_render_t *ren); \
    float pensize; \
    float fontsize; \
    int16_t span_y; \
    int16_t span_x0; \
    int16_t span_x1; \
    uint8_t linecap; \
    uint8_t alignment \

struct vgl_render_t {
    VGL_RENDER_COMMON;
};

#define vgl_render_alloc(type, surface) \
    ((type *)vgl_render_base_new((surface), sizeof(type)))

#define vgl_render_pensize(render)              ((render)->pensize)
#define vgl_render_set_pensize(render, size)    ((render)->pensize = (size))

#define vgl_render_fontsize(render)             ((render)->fontsize)
#define vgl_render_set_fontsize(render, size)   ((render)->fontsize = (size))

#define vgl_render_linecap(render)              ((render)->linecap)
#define vgl_render_set_linecap(render, type)    ((render)->linecap = (type))

#define vgl_render_alignment(render)            ((render)->alignment)
#define vgl_render_set_alignment(render, type)  ((render)->alignment = (type))

vgl_render_t* vgl_render_base_new(vgl_surface_t *surface, size_t size);
void vgl_render_free(vgl_render_t *render);
vgl_color_t* vgl_render_update_line(vgl_render_t *ren, int x0, int x1, int y);
void vgl_render_sweep_end(vgl_render_t *ren);

#endif
