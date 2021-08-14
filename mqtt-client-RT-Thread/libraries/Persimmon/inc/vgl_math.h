#ifndef __VGL_MATH_H
#define __VGL_MATH_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEG(angle)  ((angle) * (float)(180 / M_PI))
#define RAD(angle)  ((angle) * (float)(M_PI / 180))

int vgl_sqrt_i32(int x);
float vgl_sqrt(float x);
float vgl_sin(float x);
float vgl_cos(float x);

#ifdef __cplusplus
}
#endif

#endif
