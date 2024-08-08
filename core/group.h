/*
 *   Copyright (c) 2024 Anton Kundenko <singaraiona@gmail.com>
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

#ifndef GROUP_H
#define GROUP_H

#include "rayforce.h"
#include "hash.h"

// #define L1_CACHE_SIZE (64ull * 1024ull / 2)          // 64 KB
// #define L2_CACHE_SIZE (1024ull * 1024ull / 2)        // 1 MB
// #define L3_CACHE_SIZE (8ull * 1024ull * 1024ull / 2) // 8 MB

#define L1_CACHE_SIZE (32768 / 2)   // 32KB L1 cache per core, divided by two because hyperthreading
#define L2_CACHE_SIZE (1048576 / 2) // 1MB L2 cache per core, divided by two because hyperthreading
#define L3_CACHE_SIZE (1572864 / 2) // 1.5MB L3 cache per core (shared), divided by two because hyperthreading

obj_p group_map(obj_p val, obj_p index);
obj_p group_build_index(i64_t keys[], u64_t len, hash_f hash, cmp_f cmp, raw_p seed);

#endif // GROUP_H
