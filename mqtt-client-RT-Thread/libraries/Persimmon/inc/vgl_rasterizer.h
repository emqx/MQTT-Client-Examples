#ifndef __VGL_RASTERIZER_H
#define __VGL_RASTERIZER_H

#include "vgl_typedef.h"

typedef struct vgl_stroke_line_t vgl_stroke_line_t;

typedef struct {
    int ex, ey;
    int min_ex, max_ex;
    int min_ey, max_ey;
    int area;
    int cover;
    int pos_x, pos_y;
    struct vgl_cell_t **ycells, **sorted_cells;
    struct vgl_tcell_pool *cells_pool;
    struct vgl_tcell_pool *cells;
    struct vgl_cells_sorted_y *sorted_y;
    int num_cells;
    vgl_render_t *render;
    struct vgl_matrix_t *matrix;
    int sorted;
} vgl_rasterizer_t;

#define vgl_ras_set_render(ras, ren)        ((ras)->render = (ren))
#define vgl_ras_pos(ras, x, y)              { (x) += (ras)->pos_x; (y) += (ras)->pos_y; }

vgl_rasterizer_t* vgl_rasterizer_new(vgl_surface_t *surface);
void vgl_rasterizer_free(vgl_rasterizer_t *ras);
void vgl_rasterizer_set_window(vgl_rasterizer_t *ras, int x, int y, int width, int height);
void vgl_ras_sweep_scanlines(vgl_rasterizer_t *ras);
void vgl_ras_render_outline(vgl_rasterizer_t *ras, int x1, int y1, int x2, int y2);

#endif
