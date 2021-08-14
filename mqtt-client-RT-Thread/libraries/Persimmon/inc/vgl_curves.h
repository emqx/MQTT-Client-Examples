#ifndef __VGL_CURVES_H
#define __VGL_CURVES_H

#include "vgl_typedef.h"
#include "vgl_rasterizer.h"

void vgl_stroke_curve2_to(vgl_stroke_line_t *vc,
    float x1, float y1, float x2, float y2);
void vgl_stroke_curve3_to(vgl_stroke_line_t *vc,
    float x1, float y1, float x2, float y2, float x3, float y3);

#endif
