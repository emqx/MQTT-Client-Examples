#ifndef vgl_VECTOR_H
#define vgl_VECTOR_H

typedef struct vgl_vector {
    int capacity, size;
    void *data, *end;
} vgl_vector_t, vgl_stack_t;

/* =============================== defines =============================== */
#define vgl_vector_data(vector)         ((vector)->data)
#define vgl_vector_first(vector)        ((vector)->data)
#define vgl_vector_isend(vector, item)  ((item) > (vector)->end)
#define vgl_vector_end(vector)          ((vector)->end)
#define vgl_vector_capacity(vector)     ((vector)->capacity)
#define vgl_stack_init(stack, size)     vgl_vector_init(stack, size)
#define vgl_stack_delete(stack)         vgl_vector_delete(stack)
#define vgl_stack_clear(stack)          vgl_vector_clear(stack)
#define vgl_stack_push(stack, data)     vgl_vector_append(stack, data)
#define vgl_stack_pop(stack)            vgl_vector_remove_end(stack)
#define vgl_stack_top(stack)            vgl_vector_end(stack)
#define vgl_stack_base(stack)           vgl_vector_first(stack)
#define vgl_stack_count(stack)          vgl_vector_count(stack)
#define vgl_stack_isempty(stack)        vgl_vector_isempty(stack)

/* ========================== function extern ========================== */
void vgl_vector_init(vgl_vector_t *vector, int size);
void vgl_vector_delete(vgl_vector_t *vector);
int vgl_vector_count(vgl_vector_t *vector);
int vgl_vector_isempty(vgl_vector_t *vector);
void* vgl_vector_at(vgl_vector_t *vector, int index);
void vgl_vector_append(vgl_vector_t *vector, void *data);
void vgl_vector_append_c(vgl_vector_t *vector, void *data);
void vgl_vector_remove_end(vgl_vector_t *vector);
void vgl_vector_resize(vgl_vector_t *vector, int count);
void vgl_vector_clear(vgl_vector_t *vector);
void* vgl_vector_release(vgl_vector_t *vector);
int vgl_nextsize(int value);

#endif
