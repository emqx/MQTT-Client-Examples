#ifndef __VGL_TYPEDEF_H
#define __VGL_TYPEDEF_H

#include <stdint.h>
#include <assert.h>

/* config */

#include <rtthread.h>

/* #define VGL_DEBUG */
/* #define VGL_SWAP_RB_CHANNEL */

#ifndef VGL_SUBPIXEL_SHIFT
  #define VGL_SUBPIXEL_SHIFT      4
#endif

#ifdef VGL_DEBUG
  #ifndef vgl_assert
    #define vgl_assert(e)         assert(e)
  #endif
#else
  #ifdef vgl_assert
    #undef vgl_assert
  #endif
  #define vgl_assert(e)           ((void)0)
#endif

#ifndef vgl_malloc
  #define vgl_malloc(size)        rt_malloc(size)
#endif

#ifndef vgl_realloc
  #define vgl_realloc(ptr, size)  rt_realloc(ptr, size)
#endif

#ifndef vgl_free
  #define vgl_free(ptr)           rt_free(ptr)
#endif

/* type defines */
typedef uint32_t vgl_color_t;
typedef struct vgl_surface_t vgl_surface_t;
typedef struct vgl_render_t vgl_render_t;

typedef struct {
    int x, y, width, height;
} vgl_rect_t;

typedef enum {
    vgl_linecap_butt,
    vgl_linecap_square,
    vgl_linecap_round,
    vgl_line_close_path,
    vgl_line_fill_path,
    vgl_line_fill_direct_path,
} vgl_linecap_t;

typedef enum {
    vgl_align_normal = 0x0,
    vgl_align_left = 0x1,
    vgl_align_right = 0x2,
    vgl_align_hcenter = 0x3,
    vgl_align_bottom = 0x4,
    vgl_align_top = 0x8,
    vgl_align_vcenter = 0xc,
    vgl_align_center = vgl_align_hcenter | vgl_align_vcenter
} vgl_alignment_t;

#endif
