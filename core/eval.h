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

#ifndef EVAL_H
#define EVAL_H

#include <setjmp.h>
#include "chrono.h"
#include "rayforce.h"
#include "lambda.h"
#include "mmap.h"
#include "nfo.h"
#include "util.h"
#include "heap.h"

#define VM_STACK_SIZE 1024

typedef enum op_type_t {
    OP_RET = 0,
    OP_PUSHC,
    OP_DUP,
    OP_POP,
    OP_JMPZ,
    OP_JMP,
    OP_DEREF,
    OP_CALF1,
    OP_CALF2,
    OP_CALF0,
    OP_CALFN,
    OP_CALFS,
    OP_CALFD,
} op_type_t;

struct pool_t;

typedef struct ctx_t {
    obj_p fn;   // (self) function pointer
    obj_p env;  // current environment frame
    i64_t fp;   // frame pointer
    i64_t ip;   // instruction pointer
} ctx_t;

typedef struct vm_t {
    obj_p fn;                                              // (self) function pointer
    obj_p env;                                             // current environment frame
    i64_t id;                                              // VM id
    i64_t fp;                                              // frame pointer
    i64_t sp;                                              // program stack pointer
    i64_t rp;                                              // return stack pointer
    heap_p heap;                                           // heap pointer
    struct pool_t *pool;                                   // pool pointer
    obj_p ps[VM_STACK_SIZE] __attribute__((aligned(32)));  // program stack
    ctx_t rs[VM_STACK_SIZE] __attribute__((aligned(32)));  // return stack
} __attribute__((aligned(32))) * vm_p;

// timeit_t timeit;                                       // Timeit spans.

extern __thread vm_p __VM;

vm_p vm_create(i64_t id);
nil_t vm_destroy(vm_p vm);
nil_t vm_set(vm_p vm);
vm_p vm_current(nil_t);

obj_p call(obj_p obj, i64_t arity);
obj_p *resolve(i64_t sym);
obj_p amend(obj_p sym, obj_p val);
obj_p mount_env(obj_p obj);
obj_p unmount_env(i64_t n);
obj_p eval(obj_p obj);
obj_p ray_parse_str(i64_t fd, obj_p str, obj_p file);
obj_p ray_eval_str(obj_p str, obj_p file);
obj_p ray_raise(obj_p obj);
obj_p ray_return(obj_p *x, i64_t n);
nil_t error_add_loc(obj_p err, i64_t id, ctx_t *ctx);

// TODO: replace with correct functions
obj_p vm_env_get(nil_t);
nil_t vm_env_set(vm_p vm, obj_p env);
nil_t vm_env_unset(vm_p vm);

inline __attribute__((always_inline)) nil_t vm_stack_push(obj_p val) { __VM->ps[__VM->sp++] = val; }
inline __attribute__((always_inline)) obj_p vm_stack_pop(nil_t) { return __VM->ps[--__VM->sp]; }
inline __attribute__((always_inline)) obj_p vm_stack_at(i64_t n) { return __VM->ps[__VM->sp - n - 1]; }
inline __attribute__((always_inline)) obj_p *vm_stack_peek(i64_t n) { return &__VM->ps[__VM->sp - n - 1]; }

obj_p ray_exit(obj_p *x, i64_t n);
b8_t ray_is_main_thread(nil_t);

#endif  // EVAL_H
