/*
 *   Copyright (c) 2023 Anton Kundenko <singaraiona@gmail.com>
 *   All rights reserved.
 */

#include "error.h"
#include "eval.h"
#include "heap.h"
#include <string.h>

// Error message stored directly in i64 (max 7 chars + null)
obj_p ray_err(lit_p msg) {
    obj_p obj = (obj_p)heap_alloc(sizeof(struct obj_t));
    obj->mmod = MMOD_INTERNAL;
    obj->type = TYPE_ERR;
    obj->rc = 1;
    obj->attrs = 0;
    obj->i64 = 0;
    strncpy((char *)&obj->i64, msg, 7);
    return obj;
}

lit_p ray_err_msg(obj_p err) {
    if (err == NULL_OBJ || err->type != TYPE_ERR)
        return "";
    return (lit_p)&err->i64;
}

obj_p ray_get_err(nil_t) {
    vm_p vm = VM;
    return vm ? vm->last_err : NULL_OBJ;
}

nil_t ray_set_err(obj_p err) {
    vm_p vm = VM;
    if (vm) {
        if (vm->last_err != NULL_OBJ)
            drop_obj(vm->last_err);
        vm->last_err = err;
    }
}

nil_t ray_clear_err(nil_t) {
    vm_p vm = VM;
    if (vm && vm->last_err != NULL_OBJ) {
        drop_obj(vm->last_err);
        vm->last_err = NULL_OBJ;
    }
    if (vm && vm->last_locs != NULL_OBJ) {
        drop_obj(vm->last_locs);
        vm->last_locs = NULL_OBJ;
    }
}
