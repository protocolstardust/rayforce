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

#include "bitspire.h"
#include "alloc.h"

/*
 * Each vector capacity is always factor of 8
 * This allows to avoid storing capacity in vector
 */
#define CAPACITY_FACTOR 8
#define alignup(x, a) (((x) + (a)-1) & ~((a)-1))
#define capacity(x) (alignup(x, CAPACITY_FACTOR))

extern value_t vector(i8_t type, u8_t size_of_val, i64_t len)
{
    value_t v = {
        .type = type,
        .list = {
            .ptr = NULL,
            .len = len,
        },
    };

    if (len == 0)
        return v;

    v.list.ptr = bitspire_malloc(capacity(size_of_val * len));

    return v;
}

extern null_t vector_i64_push(value_t *vector, i64_t value)
{
    i64_t len = vector->list.len;
    i64_t cap = capacity(len);

    if (cap == 0)
        vector->list.ptr = bitspire_malloc(CAPACITY_FACTOR);

    else if (cap == len)
        vector->list.ptr = bitspire_realloc(vector->list.ptr, cap);

    ((i64_t *)(vector->list.ptr))[vector->list.len++] = value;
}

extern i64_t vector_i64_pop(value_t *vector)
{
    return ((i64_t *)(vector->list.ptr))[vector->list.len--];
}

extern null_t vector_f64_push(value_t *vector, f64_t value)
{
    i64_t len = vector->list.len;
    i64_t cap = capacity(len);

    if (cap == len)
        vector->list.ptr = bitspire_realloc(vector->list.ptr, cap);

    ((f64_t *)(vector->list.ptr))[vector->list.len++] = value;
}

extern f64_t vector_f64_pop(value_t *vector)
{
    return ((f64_t *)(vector->list.ptr))[vector->list.len--];
}
