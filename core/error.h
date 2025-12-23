/*
 *   Copyright (c) 2023 Anton Kundenko <singaraiona@gmail.com>
 *   All rights reserved.
 */

#ifndef ERROR_H
#define ERROR_H

#include "rayforce.h"
#include "nfo.h"

// Error creation - msg stored directly in i64 (max 7 chars + null)
obj_p ray_err(lit_p msg);

// Error access
lit_p ray_err_msg(obj_p err);
obj_p ray_get_err(nil_t);
nil_t ray_set_err(obj_p err);
nil_t ray_clear_err(nil_t);

// Error macros
#define ERR(msg) ray_err(msg)
#define THROW(code, ...) return ray_err(code)
#define THROW_S(code, msg) return ray_err(msg)
#define error_str(code, msg) ray_err(msg)
#define error_obj(code, msg) ray_err(code)
#define ray_error(code, ...) ray_err(code)
#define THROW_TYPE1(fn, t) return ray_err(E_TYPE)
#define THROW_TYPE2(fn, t1, t2) return ray_err(E_TYPE)

#define PANIC(...)                                            \
    do {                                                      \
        fprintf(stderr, "panic %s:%d: ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                         \
        fprintf(stderr, "\n");                                \
        exit(1);                                              \
    } while (0)

typedef struct loc_t {
    span_t span;
    obj_p file;
    obj_p source;
} loc_t;

#endif  // ERROR_H
