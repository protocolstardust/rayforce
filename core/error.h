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
#define ERR_ATTR_CONTEXT 2   // Has context chain (informative errors)

// ============================================================================
// Error Codes (numeric, compact)
// ============================================================================
typedef enum {
    EC_OK = 0,
    EC_TYPE,    // Type mismatch
    EC_LEN,     // Length mismatch
    EC_ARITY,   // Wrong number of arguments
    EC_INDEX,   // Index out of bounds
    EC_RANGE,   // Value out of range
    EC_KEY,     // Key not found
    EC_VAL,     // Invalid value
    EC_ARG,     // Invalid argument
    EC_STACK,   // Stack overflow
    EC_EMPTY,   // Empty collection
    EC_NFOUND,  // Not found
    EC_IO,      // I/O error
    EC_SYS,     // System error
    EC_PARSE,   // Parse error
    EC_EVAL,    // Evaluation error
    EC_OOM,     // Out of memory
    EC_NYI,     // Not yet implemented
    EC_BAD,     // Bad/corrupt data
    EC_JOIN,    // Join error
    EC_INIT,    // Initialization error
    EC_UFLOW,   // Underflow
    EC_OFLOW,   // Overflow
    EC_RAISE,   // User-raised error
    EC_FMT,     // Format error
    EC_HEAP,    // Heap error
    EC_MAX
} err_code_t;

// ============================================================================
// Error Context Types (what info we're adding)
// ============================================================================
typedef enum {
    CTX_NONE = 0,
    CTX_EXPECTED,  // "expected {type}, got {type}" - val = (expected << 8) | actual
    CTX_ARGUMENT,  // "in argument '{name}'" - val = symbol
    CTX_FIELD,     // "in field '{name}'" - val = symbol
    CTX_FUNCTION,  // "in {name}" - val = symbol
    CTX_INDEX,     // "at index {n}" - val = index
    CTX_KEY,       // "for key '{k}'" - val = symbol or obj_p
    CTX_VALUE,     // "value was {v}" - val = i64 or type
    CTX_GOT,       // "got {type}" - val = type
    CTX_NEED,      // "need {n}" - val = count
    CTX_HAVE,      // "have {n}" - val = count
    CTX_MAX
} ctx_type_t;

// ============================================================================
// Error Context Entry (8 bytes - type + value)
// ============================================================================
typedef struct {
    i64_t val;  // Context value (type, symbol, index, etc.)
    u8_t type;  // ctx_type_t
    u8_t _pad[7];
} err_ctx_t;

// Maximum inline contexts in error object
#define ERR_CTX_MAX 4

// ============================================================================
// Error Creation Functions
// ============================================================================

// System error - captures errno/GetLastError and creates extended error
obj_p sys_error(lit_p code);

// Error access
lit_p ray_err_msg(obj_p err);
err_code_t ray_err_code(obj_p err);
i64_t ray_err_ctx_count(obj_p err);
err_ctx_t *ray_err_ctx(obj_p err, i64_t idx);

// Create error with context
obj_p ray_err_ctx1(err_code_t code, ctx_type_t t1, i64_t v1);
obj_p ray_err_ctx2(err_code_t code, ctx_type_t t1, i64_t v1, ctx_type_t t2, i64_t v2);
obj_p ray_err_ctx3(err_code_t code, ctx_type_t t1, i64_t v1, ctx_type_t t2, i64_t v2, ctx_type_t t3, i64_t v3);

// Helper macros for common patterns
// Note: mask types to 0xFF to handle negative atom types correctly
#define ERR_TYPE_EXPECTED(expected, actual) \
    ray_err_ctx1(EC_TYPE, CTX_EXPECTED, (((i64_t)(expected) & 0xFF) << 8) | ((actual) & 0xFF))

#define ERR_TYPE_ARG(arg_sym, expected, actual) \
    ray_err_ctx2(EC_TYPE, CTX_ARGUMENT, (arg_sym), CTX_EXPECTED, (((i64_t)(expected) & 0xFF) << 8) | ((actual) & 0xFF))

#define ERR_ARITY_EXPECTED(need, have) ray_err_ctx2(EC_ARITY, CTX_NEED, (need), CTX_HAVE, (have))

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
