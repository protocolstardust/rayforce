#include "storm.h"
#include "alloc.h"

value_t til(i64_t count)
{
    i64_t *vec;

    vec = (i64_t *)storm_malloc(count * sizeof(i64_t));
    for (i64_t i = 0; i < count; i++)
    {
        vec[i] = i;
    }
    return new_vector_i64(vec, count);
}