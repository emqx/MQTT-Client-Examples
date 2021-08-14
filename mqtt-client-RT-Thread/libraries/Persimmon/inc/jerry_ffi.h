
#ifndef JERRY_FFI_H__
#define JERRY_FFI_H__

#include "stdint.h"
#include "stdbool.h"
#include <rtthread.h>
#include <jerry_util.h>
#include <jerry_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Maximum number of word-sized args to ffi-ed function. If at least one
 * of the args is double, only 2 args are allowed.
 */
#define FFI_MAX_ARGS_CNT 6

typedef void(*ffi_fn_t)(void);

enum ffi_arg_type
{
    FFI_CTYPE_WORD,
    FFI_CTYPE_BOOL,
    FFI_CTYPE_FLOAT,
    FFI_CTYPE_DOUBLE,
    FFI_CTYPE_STRING,
    FFI_CTYPE_OBJECT,
    FFI_CTYPE_VOID,
    FFI_CTYPE_BUFFER
};

enum ffi_arg_mode
{
    FFI_ARG_IN = 0x01,
    FFI_ARG_OUT = 0x02,
    FFI_ARG_IN_OUT = 0x03,
};

struct ffi_arg
{
    enum ffi_arg_type ctype;
    union
    {
        int i;
        double d;
        float f;
    } v;
};

int ffi_call(ffi_fn_t *func, int nargs, struct ffi_arg *res,
             struct ffi_arg *args);

void ffi_set_word(struct ffi_arg *arg, int v);
void ffi_set_bool(struct ffi_arg *arg, bool v);
void ffi_set_ptr(struct ffi_arg *arg, void *v);
void ffi_set_double(struct ffi_arg *arg, double v);
void ffi_set_float(struct ffi_arg *arg, float v);


struct js_ffi_object_arg
{
    char *name;
    enum ffi_arg_type type;
    int obj_cnt;
    struct js_ffi_object_arg *object_args;
};

struct js_ffi_arg_type
{
    enum ffi_arg_type type;
    enum ffi_arg_mode arg_mode;
    int obj_cnt;
    struct js_ffi_object_arg *object_args;
};

struct js_ffi_info
{
    char *name;
    int args_cnt;
    ffi_fn_t cfunc;
    enum ffi_arg_type ffi_ret_type;
    struct js_ffi_arg_type *ffi_args_type;
};

jerry_value_t js_create_ffi_object(struct js_ffi_info *ffi_info, int ffi_info_cnt);

#ifdef __cplusplus
}
#endif

#endif
