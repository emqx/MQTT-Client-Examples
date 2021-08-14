#ifndef __VGL_RASTERIZER_CELL_H
#define __VGL_RASTERIZER_CELL_H

void vgl_cells_pool_init(vgl_rasterizer_t *ras);
void vgl_cells_pool_delete(vgl_rasterizer_t *ras);
void vgl_remove_all_cells(vgl_rasterizer_t *ras);
void vgl_set_curr_cell(vgl_rasterizer_t *ras, int ex, int ey);
void vgl_rasterizer_sort_cells(vgl_rasterizer_t *ras);

typedef struct vgl_cell_t {
#if VGL_SUBPIXEL_SHIFT <= 2
    short x, y;
    signed char cover;
    signed char area;
#elif VGL_SUBPIXEL_SHIFT <= 6
    int x, y;
    signed short cover;
    signed short area;
#else
    int x, y;
    int cover;
    int area;
#endif
    struct vgl_cell_t *next;
} vgl_cell_t;

typedef struct vgl_cells_sorted_y {
    int start;
    int num;
} vgl_cells_sorted_y;

#endif
