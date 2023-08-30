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

#ifndef BINARY_H
#define BINARY_H

#include "rayforce.h"
#include "ops.h"

obj_t ray_call_binary(u8_t attrs, binary_f f, obj_t x, obj_t y);
obj_t ray_set(obj_t x, obj_t y);
obj_t ray_write(obj_t x, obj_t y);
obj_t ray_dict(obj_t x, obj_t y);
obj_t ray_table(obj_t x, obj_t y);
obj_t ray_rand(obj_t x, obj_t y);
obj_t ray_add(obj_t x, obj_t y);
obj_t ray_sub(obj_t x, obj_t y);
obj_t ray_mul(obj_t x, obj_t y);
obj_t ray_div(obj_t x, obj_t y);
obj_t ray_mod(obj_t x, obj_t y);
obj_t ray_fdiv(obj_t x, obj_t y);
obj_t ray_like(obj_t x, obj_t y);
obj_t ray_eq(obj_t x, obj_t y);
obj_t ray_ne(obj_t x, obj_t y);
obj_t ray_lt(obj_t x, obj_t y);
obj_t ray_gt(obj_t x, obj_t y);
obj_t ray_le(obj_t x, obj_t y);
obj_t ray_ge(obj_t x, obj_t y);
obj_t ray_and(obj_t x, obj_t y);
obj_t ray_or(obj_t x, obj_t y);
obj_t ray_and(obj_t x, obj_t y);
obj_t ray_at(obj_t x, obj_t y);
obj_t ray_find(obj_t x, obj_t y);
obj_t ray_concat(obj_t x, obj_t y);
obj_t ray_filter(obj_t x, obj_t y);
obj_t ray_take(obj_t x, obj_t y);
obj_t ray_in(obj_t x, obj_t y);
obj_t ray_sect(obj_t x, obj_t y);
obj_t ray_except(obj_t x, obj_t y);
obj_t ray_cast(obj_t x, obj_t y);
obj_t ray_xasc(obj_t x, obj_t y);
obj_t ray_xdesc(obj_t x, obj_t y);
obj_t ray_enum(obj_t x, obj_t y);
obj_t ray_vecmap(obj_t x, obj_t y);
obj_t ray_listmap(obj_t x, obj_t y);

#endif
