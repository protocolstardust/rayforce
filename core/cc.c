/*
 *   Copyright (c) 2023 Anton Kundenko <singaraiona@gmail.com>
 *   All rights reserved.

 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:

 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.

 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */

#include <stdio.h>
#include "cc.h"
#include "vm.h"
#include "alloc.h"
#include "format.h"
#include "util.h"

#define push_opcode(x)                             \
    if (offset >= len)                             \
    {                                              \
        len *= 2;                                  \
        code = (str_t)rayforce_realloc(code, len); \
    }                                              \
    code[offset++] = x;

#define push_object(x)                                                   \
    if (offset + sizeof(rf_object_t) >= len)                             \
    {                                                                    \
        len *= 2;                                                        \
        code = (str_t)rayforce_realloc(code, len * sizeof(rf_object_t)); \
    }                                                                    \
    *(rf_object_t *)(code + offset) = x;                                 \
    offset += sizeof(rf_object_t)

str_t cc_compile(rf_object_t object)
{
    u32_t len = 2 * sizeof(rf_object_t), offset = 0;
    str_t code = (str_t)rayforce_malloc(len);

    push_opcode(VM_PUSH);
    push_object(object);
    push_opcode(VM_HALT);

    return code;
}

str_t cc_code_fmt(str_t code)
{
    i32_t p = 0, c = 0;
    str_t ip = code;
    str_t s = str_fmt(0, "-- code:\n");

    p = strlen(s);

    while (*ip != VM_HALT)
    {
        switch (*ip++)
        {
        case VM_PUSH:
            p += str_fmt_into(0, p, &s, "%.4d: push %p\n", c++, ((rf_object_t *)(ip + 1)));
            ip += sizeof(rf_object_t);
            break;
        case VM_POP:
            p += str_fmt_into(0, p, &s, "%.4d: pop\n", c++);
            break;
        case VM_ADD:
            p += str_fmt_into(0, p, &s, "%.4d: add\n", c++);
            break;
        default:
            p += str_fmt_into(0, p, &s, "%.4d: unknown %d\n", c++, *ip);
            break;
        }
    }

    str_fmt_into(0, p, &s, "%.4d: halt", c++);

    return s;
}