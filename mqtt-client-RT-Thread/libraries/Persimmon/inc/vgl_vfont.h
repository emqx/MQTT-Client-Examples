#ifndef __VGL_VFONT_H
#define __VGL_VFONT_H

#include "vgl/vgl_stroke_line.h"

void vgl_stroke_text(vgl_stroke_line_t *vc, const char *str);
void vgl_stroke_arctext(vgl_stroke_line_t* vc, const char* str, float radius, float rotate);

#endif
