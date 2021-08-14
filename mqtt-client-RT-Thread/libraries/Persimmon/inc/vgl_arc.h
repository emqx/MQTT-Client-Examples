#ifndef __VGL_ARC_H
#define __VGL_ARC_H

#include "vgl_typedef.h"
#include "vgl_rasterizer.h"

void vgl_stroke_arc(vgl_stroke_line_t *vc, float cx, float cy,
    float rx, float ry, float angle_start, float angle_end);

#endif
