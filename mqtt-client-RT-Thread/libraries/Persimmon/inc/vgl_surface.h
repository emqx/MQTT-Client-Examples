#ifndef __VGL_SURFACE_H
#define __VGL_SURFACE_H

#include "vgl_typedef.h"

struct vgl_surface_t {
    int refcnt; /* reference count */
    const struct vgl_surface_ops *ops;
};

struct vgl_surface_ops {
    void (*blit_span)(vgl_surface_t*, const vgl_color_t*, int, int, int);
    void (*read_span)(vgl_surface_t *, vgl_color_t *, int, int, int);
    void (*get_rect)(vgl_surface_t*, vgl_rect_t*);
    void (*destroy)(vgl_surface_t *);
};

#define vgl_surface_blit_span(surface, pixels, x, y, len) \
    (surface)->ops->blit_span((surface), (pixels), (x), (y), (len))

#define vgl_surface_read_span(surface, pixels, x, y, len) \
    (surface)->ops->read_span((surface), (pixels), (x), (y), (len))

#define vgl_surface_line_buffer(surface, y) \
    (surface)->ops->line_buffer((surface), (y))

void vgl_surface_base_init(vgl_surface_t *surface, const struct vgl_surface_ops *ops);
void vgl_surface_rect(vgl_surface_t *surface, vgl_rect_t *rect);
int vgl_surface_width(vgl_surface_t *surface);
int vgl_surface_height(vgl_surface_t *surface);
void vgl_surface_prerender(vgl_surface_t* surface);

#endif
