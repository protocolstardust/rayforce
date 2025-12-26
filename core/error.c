/*
 *   Copyright (c) 2023 Anton Kundenko <singaraiona@gmail.com>
 *   All rights reserved.
 */

#include "error.h"
#include "heap.h"
#include "string.h"
#include "def.h"

// Platform-specific errno access (internal helper)
static i32_t sys_errno(nil_t) {
#ifdef OS_WINDOWS
    return (i32_t)GetLastError();
#else
    return errno;
#endif
}

// Platform-specific strerror (internal helper)
static lit_p sys_strerror(i32_t errnum, str_p buf, i64_t buflen) {
#ifdef OS_WINDOWS
    if (errnum == 0)
        errnum = (i32_t)GetLastError();
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errnum,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, (DWORD)buflen, NULL);
    // Remove trailing newlines
    i64_t len = strlen(buf);
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r'))
        buf[--len] = '\0';
    return buf;
#else
    if (errnum == 0)
        errnum = errno;
#if defined(_GNU_SOURCE) && !defined(OS_MACOS)
    // GNU version returns char*
    return strerror_r(errnum, buf, buflen);
#else
    // XSI/POSIX version returns int
    if (strerror_r(errnum, buf, buflen) == 0)
        return buf;
    snprintf(buf, buflen, "Unknown error %d", errnum);
    return buf;
#endif
#endif
}

// Extended error with variable length message stored in raw[] (internal helper)
static obj_p ray_err_ext(lit_p code, lit_p msg) {
    i64_t code_len = strlen(code);
    i64_t msg_len = msg ? strlen(msg) : 0;
    i64_t total_len = code_len + (msg_len > 0 ? 2 + msg_len : 0);  // "code: msg" or just "code"

    obj_p obj = (obj_p)heap_alloc(sizeof(struct obj_t) + total_len + 1);
    obj->mmod = MMOD_INTERNAL;
    obj->type = TYPE_ERR;
    obj->rc = 1;
    obj->attrs = ERR_ATTR_EXTENDED;
    obj->len = total_len;

    str_p dst = (str_p)(obj + 1);
    memcpy(dst, code, code_len);
    if (msg_len > 0) {
        dst[code_len] = ':';
        dst[code_len + 1] = ' ';
        memcpy(dst + code_len + 2, msg, msg_len);
    }
    dst[total_len] = '\0';

    return obj;
}

// Error creation - if msg is "sys", capture errno and create extended error
obj_p ray_err(lit_p msg) {
    // Check if this is a system error request
    if (strcmp(msg, ERR_SYS) == 0) {
        char buf[256];
        i32_t errnum = sys_errno();
        if (errnum != 0) {
            sys_strerror(errnum, buf, sizeof(buf));
            return ray_err_ext(msg, buf);
        }
    }

    // Default: short error stored in i64 (max 7 chars)
    obj_p obj = (obj_p)heap_alloc(sizeof(struct obj_t));
    obj->mmod = MMOD_INTERNAL;
    obj->type = TYPE_ERR;
    obj->rc = 1;
    obj->attrs = ERR_ATTR_SHORT;
    obj->i64 = 0;
    strncpy((char *)&obj->i64, msg, 7);
    return obj;
}

// Error code names for display (indexed by err_code_t)
static lit_p err_code_names[] = {
    "ok",      // EC_OK
    "type",    // EC_TYPE
    "length",  // EC_LEN
    "arity",   // EC_ARITY
    "index",   // EC_INDEX
    "range",   // EC_RANGE
    "key",     // EC_KEY
    "val",     // EC_VAL
    "arg",     // EC_ARG
    "stack",   // EC_STACK
    "empty",   // EC_EMPTY
    "nfound",  // EC_NFOUND
    "io",      // EC_IO
    "sys",     // EC_SYS
    "parse",   // EC_PARSE
    "eval",    // EC_EVAL
    "oom",     // EC_OOM
    "nyi",     // EC_NYI
    "bad",     // EC_BAD
    "join",    // EC_JOIN
    "init",    // EC_INIT
    "uflow",   // EC_UFLOW
    "oflow",   // EC_OFLOW
    "raise",   // EC_RAISE
    "fmt",     // EC_FMT
    "heap",    // EC_HEAP
};

lit_p ray_err_msg(obj_p err) {
    if (err == NULL_OBJ || err->type != TYPE_ERR)
        return "";
    if (err->attrs == ERR_ATTR_EXTENDED)
        return (lit_p)(err + 1);
    if (err->attrs == ERR_ATTR_CONTEXT) {
        // Context error - code is in first byte of i64
        u8_t code = (u8_t)(err->i64 & 0xFF);
        if (code < EC_MAX)
            return err_code_names[code];
        return "error";
    }
    return (lit_p)&err->i64;
}

// Get error code from context error
err_code_t ray_err_code(obj_p err) {
    if (err == NULL_OBJ || err->type != TYPE_ERR)
        return EC_OK;
    if (err->attrs == ERR_ATTR_CONTEXT)
        return (err_code_t)(err->i64 & 0xFF);
    return EC_OK;  // Legacy errors don't have numeric codes
}

// Get context count
i64_t ray_err_ctx_count(obj_p err) {
    if (err == NULL_OBJ || err->type != TYPE_ERR || err->attrs != ERR_ATTR_CONTEXT)
        return 0;
    return (err->i64 >> 8) & 0xFF;
}

// Get context entry by index
err_ctx_t *ray_err_ctx(obj_p err, i64_t idx) {
    if (err == NULL_OBJ || err->type != TYPE_ERR || err->attrs != ERR_ATTR_CONTEXT)
        return NULL;
    i64_t count = (err->i64 >> 8) & 0xFF;
    if (idx < 0 || idx >= count)
        return NULL;
    return ((err_ctx_t *)(err + 1)) + idx;
}

// Create error with 1 context entry
obj_p ray_err_ctx1(err_code_t code, ctx_type_t t1, i64_t v1) {
    obj_p obj = (obj_p)heap_alloc(sizeof(struct obj_t) + sizeof(err_ctx_t));
    obj->mmod = MMOD_INTERNAL;
    obj->type = TYPE_ERR;
    obj->rc = 1;
    obj->attrs = ERR_ATTR_CONTEXT;
    obj->i64 = (i64_t)code | (1LL << 8);  // code + count

    err_ctx_t *ctx = (err_ctx_t *)(obj + 1);
    ctx->type = t1;
    ctx->val = v1;
    memset(ctx->_pad, 0, sizeof(ctx->_pad));

    return obj;
}

// Create error with 2 context entries
obj_p ray_err_ctx2(err_code_t code, ctx_type_t t1, i64_t v1, ctx_type_t t2, i64_t v2) {
    obj_p obj = (obj_p)heap_alloc(sizeof(struct obj_t) + 2 * sizeof(err_ctx_t));
    obj->mmod = MMOD_INTERNAL;
    obj->type = TYPE_ERR;
    obj->rc = 1;
    obj->attrs = ERR_ATTR_CONTEXT;
    obj->i64 = (i64_t)code | (2LL << 8);  // code + count

    err_ctx_t *ctx = (err_ctx_t *)(obj + 1);
    ctx[0].type = t1;
    ctx[0].val = v1;
    memset(ctx[0]._pad, 0, sizeof(ctx[0]._pad));
    ctx[1].type = t2;
    ctx[1].val = v2;
    memset(ctx[1]._pad, 0, sizeof(ctx[1]._pad));

    return obj;
}

// Create error with 3 context entries
obj_p ray_err_ctx3(err_code_t code, ctx_type_t t1, i64_t v1, ctx_type_t t2, i64_t v2, ctx_type_t t3, i64_t v3) {
    obj_p obj = (obj_p)heap_alloc(sizeof(struct obj_t) + 3 * sizeof(err_ctx_t));
    obj->mmod = MMOD_INTERNAL;
    obj->type = TYPE_ERR;
    obj->rc = 1;
    obj->attrs = ERR_ATTR_CONTEXT;
    obj->i64 = (i64_t)code | (3LL << 8);  // code + count

    err_ctx_t *ctx = (err_ctx_t *)(obj + 1);
    ctx[0].type = t1;
    ctx[0].val = v1;
    memset(ctx[0]._pad, 0, sizeof(ctx[0]._pad));
    ctx[1].type = t2;
    ctx[1].val = v2;
    memset(ctx[1]._pad, 0, sizeof(ctx[1]._pad));
    ctx[2].type = t3;
    ctx[2].val = v3;
    memset(ctx[2]._pad, 0, sizeof(ctx[2]._pad));

    return obj;
}

// System error - captures current errno/GetLastError
obj_p sys_error(lit_p code) {
    char buf[256];
    i32_t errnum = sys_errno();

    if (errnum == 0)
        return ray_err(code);

    sys_strerror(errnum, buf, sizeof(buf));
    return ray_err_ext(code, buf);
}

