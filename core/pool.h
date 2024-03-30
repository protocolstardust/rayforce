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

#ifndef POOL_H
#define POOL_H

#include <pthread.h>
#include "rayforce.h"
#include "heap.h"

typedef obj_p (*task_fn)(raw_p, u64_t);

typedef struct
{
    task_fn fn;
    raw_p arg;
    u64_t len;
    obj_p result;
} task_t;

typedef struct
{
    u64_t id;
    pthread_mutex_t mutex;    // Mutex for condition variable
    pthread_cond_t has_task;  // Condition variable for task
    pthread_cond_t done_task; // Condition variable for task completion
    b8_t stop;                // Stop flag
    b8_t done;                // Done flag
    task_t task;              // Executor's task
    heap_p heap;              // Executor's heap
    pthread_t handle;         // Executor's thread handle
} executor_t;

typedef struct pool_t
{
    u64_t executors_count;  // Number of executors
    executor_t executors[]; // Array of executors
} *pool_p;

pool_p pool_new(u64_t executors_count);
nil_t pool_free(pool_p pool);
nil_t pool_run(pool_p pool, u64_t executor_id, task_fn fn, raw_p arg, u64_t len);
nil_t pool_wait(pool_p pool, u64_t executor_id);
obj_p pool_collect(pool_p pool, obj_p x);
u64_t pool_executors_count(pool_p pool);
nil_t pool_stop(pool_p pool);

#endif // POOL_H
