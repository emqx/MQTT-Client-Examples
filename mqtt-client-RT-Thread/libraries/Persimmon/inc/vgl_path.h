#ifndef __VGL_PATH_H
#define __VGL_PATH_H

#include "vgl/vgl_typedef.h"
#include "vgl/vgl_vector.h"
#include "vgl/vgl_matrix.h"

enum vgl_path_type {
    vgl_path_none,
    vgl_path_line,
    vgl_path_close,
    vgl_path_fill,
    vgl_path_move_to,
    vgl_path_line_to,
    vgl_path_rect_to,
    vgl_path_curve2_to,
    vgl_path_curve3_to,
    vgl_path_arc_to,
    vgl_path_text_to,
    vgl_path_arctext_to
};

struct vgl_path_vertex {
    enum vgl_path_type type;
};

struct vgl_path_vertex_move_to {
    struct vgl_path_vertex base;
    float x, y;
};

struct vgl_path_vertex_line_to {
    struct vgl_path_vertex base;
    float x, y;
};

struct vgl_path_vertex_rect_to {
    struct vgl_path_vertex base;
    float x, y;
};

struct vgl_path_vertex_curve2_to {
    struct vgl_path_vertex base;
    float x1, y1, x2, y2;
};

struct vgl_path_vertex_curve3_to {
    struct vgl_path_vertex base;
    float x1, y1, x2, y2, x3, y3;
};

struct vgl_path_vertex_arc_to {
    struct vgl_path_vertex base;
    float cx, cy, rx, ry;
    float angle_start, angle_end;
};

struct vgl_path_vertex_text_to {
    struct vgl_path_vertex base;
    char str[1]; /* scalable */
};

struct vgl_path_vertex_arctext_to {
    struct vgl_path_vertex base;
    float radius, rotate;
    char str[1]; /* scalable */
};

struct vgl_path {
    vgl_color_t color;
    vgl_surface_t *surface;
    vgl_matrix_t matrix;
    float line_width, font_size;
    vgl_vector_t list;
    uint8_t linecap;
    uint8_t alignment;
    uint8_t type;
};

struct vgl_t {
    vgl_vector_t path_list;
    float line_width, font_size;
    vgl_color_t color;
    vgl_surface_t *source_surface;
    vgl_matrix_t matrix;
    uint8_t linecap;
    uint8_t alignment;
    uint8_t render_type;
    int pos_x, pos_y, win_width, win_height;
};

#endif
