/*
 *   Copyright (c) 2023 Anton Kundenko <singaraiona@gmail.com>
 *   All rights reserved.
 */

#ifndef ERROR_H
#define ERROR_H

#include "rayforce.h"
#include "nfo.h"

// Error attrs flags
#define ERR_ATTR_SHORT 0     // Message stored in i64 (max 7 chars)
#define ERR_ATTR_EXTENDED 1  // Message stored in raw[] (variable length)

// System error - captures errno/GetLastError and creates extended error
obj_p sys_error(lit_p code);

// Error access
lit_p ray_err_msg(obj_p err);

#define PANIC(...)                                            \
    do {                                                      \
        fprintf(stderr, "panic %s:%d: ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                         \
        exit(1);                                              \
    } while (0)

typedef struct loc_t {
    span_t span;
    obj_p file;
    obj_p source;
} loc_t;

#endif  // ERROR_H
