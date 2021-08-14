#ifndef __VGL_STROKE_LINE_H
#define __VGL_STROKE_LINE_H

#include "vgl_typedef.h"
#include "vgl_rasterizer.h"

typedef struct {
    float x;    // the vertex x coordinat
    float y;    // the vertex y coordinat
    float dist; // the distance to the last vertex
} vgl_vertex_t;

/******************************************************************************
 *                      +. v1
 *                     /  \
 *                    /    + vl2
 *                   /    . \
 *                  /    .   + v2
 *                 /    .    /
 *        v1 (old) +   .    /
 *                  \ .    /
 *               vl1 +    /
 *                    \  /
 *            v2 (old)  *
 ******************************************************************************/
struct vgl_stroke_line_t {
    vgl_vertex_t vl1, vl2; // vertex line 1, vertex line 2
    vgl_vertex_t v1, v2;   // the vertex of the line-cap
    vgl_vertex_t vs1, vs2; // vs1 and vs2 at the beginning of the path
    vgl_vertex_t v1s, v2s; // the vertex of the line-cap (first outline)
    float x, y;            // move to (x, y)
    float width;
    uint8_t cmd;
    uint8_t linecap;
    vgl_rasterizer_t *ras;
};

void vgl_stroke_move_to(vgl_stroke_line_t *vc, float x, float y);
void vgl_stroke_line_to(vgl_stroke_line_t *vc, float x, float y);
void vgl_stroke_rect_to(vgl_stroke_line_t *vc, float x, float y);
void vgl_vertex_line_to(vgl_stroke_line_t *vc, float x, float y);
void vgl_stoke_path_end(vgl_stroke_line_t *vc);
void vgl_stroke_line_init(vgl_rasterizer_t *ras, vgl_stroke_line_t *vc);
int vgl_stroke_line_is_invalid(vgl_stroke_line_t *vc);

#endif
