#ifndef __VGL_H
#define __VGL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vgl_typedef.h"

/* vgl public struct define */
typedef struct vgl_t vgl_t;

/* vgl public APIs */
vgl_t* vgl_context_create(void);
void vgl_context_free(vgl_t *vgl);
void vgl_context_clear(vgl_t *vgl);

/* style functions */
void vgl_set_line_width(vgl_t *vgl, float width);
void vgl_set_font_size(vgl_t *vgl, float size);
void vgl_set_source_color(vgl_t *vgl, vgl_color_t color);
void vgl_set_source_argb(vgl_t *vgl, float a, float r, float g, float b);
void vgl_set_source_surface(vgl_t *vgl, vgl_surface_t *surface);
void vgl_set_line_cap(vgl_t *vgl, vgl_linecap_t linecap);
void vgl_set_alignment(vgl_t *vgl, int mode);
void vgl_set_alignment_horizontal(vgl_t *vgl, int mode);
void vgl_set_alignment_vertical(vgl_t *vgl, int mode);

/* path functions */
void vgl_move_to(vgl_t *vgl, float x, float y);
void vgl_line_to(vgl_t *vgl, float x, float y);
void vgl_rect_to(vgl_t *vgl, float x, float y);
void vgl_curve2_to(vgl_t *vgl, float x1, float y1, float x2, float y2);
void vgl_curve3_to(vgl_t *vgl, float x1, float y1, float x2, float y2, float x3, float y3);
void vgl_arc_to(vgl_t *vgl, float cx, float cy, float rx, float ry, float angle_start, float angle_end);
void vgl_text_to(vgl_t *vgl, const char *str);
void vgl_arctext_to(vgl_t *vgl, const char *str, float radius, float rotate);

/* transform functions */
void vgl_transform_reset(vgl_t *vgl);
void vgl_translate(vgl_t *vgl, float tx, float ty);
void vgl_rotate(vgl_t *vgl, float angle);
void vgl_scale(vgl_t *vgl, float sx, float sy);
void vgl_shear(vgl_t *vgl, float sh, float sv);

/* stroke functions */
void vgl_stroke(vgl_t *vgl);
void vgl_close_path(vgl_t *vgl);
void vgl_fill(vgl_t *vgl);
void vgl_draw(vgl_t *vgl, vgl_surface_t *surface);

/* surface functions */
vgl_surface_t* vgl_surface_acquire(vgl_surface_t *surface);
void vgl_surface_release(vgl_surface_t *surface);

/* window clip functions */
void vgl_set_render_window(vgl_t *vgl, int x, int y, int width, int height);

#ifdef __cplusplus
}
#endif

#endif
