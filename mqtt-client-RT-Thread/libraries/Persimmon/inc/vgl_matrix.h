#ifndef __VGL_MATRIX_H
#define __VGL_MATRIX_H

typedef struct vgl_matrix_t {
    float matrix[3][2];
} vgl_matrix_t;

void vgl_matrix_identity(vgl_matrix_t *matrix);
int vgl_matrix_is_identity(vgl_matrix_t *matrix);
void vgl_matrix_translate(vgl_matrix_t *matrix, float tx, float ty);
void vgl_matrix_rotate(vgl_matrix_t *matrix, float angle);
void vgl_matrix_scale(vgl_matrix_t *matrix, float sx, float sy);
void vgl_matrix_shear(vgl_matrix_t *matrix, float sh, float sv);
vgl_matrix_t vgl_martix_product(vgl_matrix_t *src1, vgl_matrix_t *src2);
void vgl_matrix_transform(const vgl_matrix_t *matrix, float x, float y, float *tx, float *ty);

#endif
