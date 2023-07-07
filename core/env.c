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

#include <stdarg.h>
#include "env.h"
#include "dict.h"
#include "unary.h"
#include "binary.h"
#include "guid.h"
#include "runtime.h"
#include "format.h"

#define reg(r, n, t, o)                                             \
    {                                                               \
        rf_object_t k = symboli64(intern_keyword(n, strlen(n)));    \
        dict_set(r, &k, (rf_object_t){.type = t, .i64 = (i64_t)o}); \
    };

rf_object_t rf_env()
{
    return rf_object_clone(&runtime_get()->env.variables);
}

rf_object_t rf_memstat()
{
    rf_object_t keys, vals;
    memstat_t stat = rf_alloc_memstat();

    keys = vector_symbol(3);
    as_vector_symbol(&keys)[0] = symbol("total").i64;
    as_vector_symbol(&keys)[1] = symbol("used ").i64;
    as_vector_symbol(&keys)[2] = symbol("free ").i64;

    vals = list(3);
    as_list(&vals)[0] = i64(stat.total);
    as_list(&vals)[1] = i64(stat.used);
    as_list(&vals)[2] = i64(stat.free);

    return dict(keys, vals);
}

// clang-format off
null_t init_functions(rf_object_t *functions)
{
    // Unary
    reg(functions, "type",        TYPE_UNARY,    rf_type);
    reg(functions, "til",         TYPE_UNARY,    rf_til);
    // REC(functions,  "trace",       OP_TRACE);
    reg(functions,  "distinct",   TYPE_UNARY,    rf_distinct);
    reg(functions,  "group",      TYPE_UNARY,    rf_group);
    reg(functions,  "sum",        TYPE_UNARY,    rf_sum);
    reg(functions,  "avg",        TYPE_UNARY,    rf_avg);
    reg(functions,  "min",        TYPE_UNARY,    rf_min);
    reg(functions,  "max",        TYPE_UNARY,    rf_max);
    reg(functions,  "count",      TYPE_UNARY,    rf_count);
    reg(functions,  "not",        TYPE_UNARY,    rf_not);
    reg(functions,  "iasc",       TYPE_UNARY,    rf_iasc);
    reg(functions,  "idesc",      TYPE_UNARY,    rf_idesc);
    reg(functions,  "asc",        TYPE_UNARY,    rf_asc);
    reg(functions,  "desc",       TYPE_UNARY,    rf_desc);
    reg(functions,  "flatten",    TYPE_UNARY,    vector_flatten);
    reg(functions,  "guid",       TYPE_UNARY,    rf_guid_generate);
    reg(functions,  "neg",        TYPE_UNARY,    rf_neg);
    reg(functions,  "where",      TYPE_UNARY,    rf_where);
      
    // Binary         
    reg(functions, "==",          TYPE_BINARY,   rf_eq);
    reg(functions, "<",           TYPE_BINARY,   rf_lt);
    reg(functions, ">",           TYPE_BINARY,   rf_gt);
    reg(functions, "<=",          TYPE_BINARY,   rf_le);
    reg(functions, ">=",          TYPE_BINARY,   rf_ge);
    reg(functions, "!=",          TYPE_BINARY,   rf_ne);
    reg(functions, "and",         TYPE_BINARY,   rf_and);
    reg(functions, "or",          TYPE_BINARY,   rf_or);
    reg(functions,  "+",          TYPE_BINARY,   rf_add);
    reg(functions, "-",           TYPE_BINARY,   rf_sub);
    reg(functions, "*",           TYPE_BINARY,   rf_mul);
    reg(functions, "/",           TYPE_BINARY,   rf_div);
    reg(functions, "%",           TYPE_BINARY,   rf_mod);
    reg(functions, "div",         TYPE_BINARY,   rf_fdiv);
    reg(functions, "like",        TYPE_BINARY,   rf_like);
    reg(functions, "dict",        TYPE_BINARY,   rf_dict);
    reg(functions, "table",       TYPE_BINARY,   rf_table);
    reg(functions, "get",         TYPE_BINARY,   rf_get);
    reg(functions, "find",        TYPE_BINARY,   rf_find);
    reg(functions, "concat",      TYPE_BINARY,   rf_concat);
    reg(functions, "filter",      TYPE_BINARY,   rf_filter);
    reg(functions, "take",        TYPE_BINARY,   rf_take);
    reg(functions, "in",          TYPE_BINARY,   rf_in);
    reg(functions, "sect",        TYPE_BINARY,   rf_sect);
    reg(functions, "except",      TYPE_BINARY,   rf_except);
    reg(functions, "rand",        TYPE_BINARY,   rf_rand);
     
    // Lambdas       
    // reg(functions, "env",         rf_env);
    // reg(functions, "memstat",     rf_memstat);
    reg(functions,  "list",      -TYPE_LAMBDA,   rf_list);
    reg(functions, "format",     -TYPE_LAMBDA,   rf_format);
    reg(functions, "print",      -TYPE_LAMBDA,   rf_print);
    reg(functions, "println",    -TYPE_LAMBDA,   rf_println);
}    
    
null_t init_typenames(i64_t *typenames)    
{
    // typenames[-TYPE_BOOL      + TYPE_OFFSET] = symbol("bool").i64;
    // typenames[-TYPE_I64       + TYPE_OFFSET] = symbol("i64").i64;
    // typenames[-TYPE_F64       + TYPE_OFFSET] = symbol("f64").i64;
    // typenames[-TYPE_SYMBOL    + TYPE_OFFSET] = symbol("symbol").i64;
    // typenames[-TYPE_TIMESTAMP + TYPE_OFFSET] = symbol("timestamp").i64;
    // typenames[-TYPE_GUID      + TYPE_OFFSET] = symbol("guid").i64;
    // typenames[-TYPE_CHAR      + TYPE_OFFSET] = symbol("char").i64;
    // typenames[ TYPE_NULL      + TYPE_OFFSET] = symbol("Null").i64;
    // typenames[ TYPE_BOOL      + TYPE_OFFSET] = symbol("Bool").i64;
    // typenames[ TYPE_I64       + TYPE_OFFSET] = symbol("I64").i64;
    // typenames[ TYPE_F64       + TYPE_OFFSET] = symbol("F64").i64;
    // typenames[ TYPE_SYMBOL    + TYPE_OFFSET] = symbol("Symbol").i64;
    // typenames[ TYPE_TIMESTAMP + TYPE_OFFSET] = symbol("Timestamp").i64;
    // typenames[ TYPE_GUID      + TYPE_OFFSET] = symbol("Guid").i64;
    // typenames[ TYPE_CHAR      + TYPE_OFFSET] = symbol("Char").i64;
    // typenames[ TYPE_LIST      + TYPE_OFFSET] = symbol("List").i64;
    // typenames[ TYPE_DICT      + TYPE_OFFSET] = symbol("Dict").i64;
    // typenames[ TYPE_TABLE     + TYPE_OFFSET] = symbol("Table").i64;
    // typenames[ TYPE_LAMBDA  + TYPE_OFFSET] = symbol("lambda").i64;
    // typenames[ TYPE_ERROR     + TYPE_OFFSET] = symbol("Error").i64;
}


null_t init_kw_symbols()
{
    assert(intern_symbol("",       0)  == NULL_SYM);
    assert(intern_keyword("time",  4)  == KW_TIME);
    assert(intern_keyword("`",     1)  == KW_QUOTE);
    assert(intern_keyword("set",   3)  == KW_SET);
    assert(intern_keyword("let",   3)  == KW_LET);
    assert(intern_keyword("fn",    2)  == KW_FN);
    assert(intern_keyword("self",  4)  == KW_SELF);
    assert(intern_keyword("if",    2)  == KW_IF);
    assert(intern_keyword("try",   3)  == KW_TRY);
    assert(intern_keyword("catch", 5)  == KW_CATCH);
    assert(intern_keyword("throw", 5)  == KW_THROW);
    assert(intern_keyword("map",   3)  == KW_MAP);
    assert(intern_keyword("select",6)  == KW_SELECT);
    assert(intern_keyword("from",  4)  == KW_FROM);
    assert(intern_keyword("where", 5)  == KW_WHERE);
    assert(intern_keyword("by",    2)  == KW_BY);
    assert(intern_keyword("order", 5)  == KW_ORDER);
}
// clang-format on

env_t create_env()
{
    rf_object_t functions = dict(vector_symbol(0), list(0));
    rf_object_t variables = dict(vector_symbol(0), list(0));

    init_kw_symbols();

    init_functions(&functions);

    env_t env = {
        .functions = functions,
        .variables = variables,
    };

    init_typenames(env.typenames);

    return env;
}

null_t free_env(env_t *env)
{
    rf_object_free(&env->variables);
    rf_object_free(&env->functions);
}

i64_t env_get_typename_by_type(env_t *env, type_t type)
{
    return env->typenames[type + TYPE_OFFSET];
}

type_t env_get_type_by_typename(env_t *env, i64_t name)
{
    for (i32_t i = 0; i < MAX_TYPE; i++)
        if (env->typenames[i] == name)
            return i - TYPE_OFFSET;

    return TYPE_NONE;
}

str_t env_get_typename(type_t type)
{
    env_t *env = &runtime_get()->env;
    i64_t name = env_get_typename_by_type(env, type);

    return symbols_get(name);
}