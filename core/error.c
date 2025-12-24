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

lit_p ray_err_msg(obj_p err) {
    if (err == NULL_OBJ || err->type != TYPE_ERR)
        return "";
    if (err->attrs == ERR_ATTR_EXTENDED)
        return (lit_p)(err + 1);
    return (lit_p)&err->i64;
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

