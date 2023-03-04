#include "format.h"
#include "storm.h"
#include <stdio.h>
#include <string.h>
#include "alloc.h"

#define MAX_i64_t_WIDTH 20
#define MAX_ROW_WIDTH MAX_i64_t_WIDTH * 2

result_t nil_fmt(str_t *buffer)
{
    *buffer = (str_t)storm_malloc(4);
    strncpy(*buffer, "nil", 4);
    return Ok;
}

result_t scalar_i64_fmt(str_t *buffer, value_t value)
{
    *buffer = (str_t)storm_malloc(MAX_i64_t_WIDTH);
    if (snprintf(*buffer, MAX_i64_t_WIDTH, "%lld", value->i64_t_value) < 0)
    {
        return FormatError;
    }
    return Ok;
}

result_t vector_i64_fmt(str_t *buffer, value_t value)
{
    str_t buf;
    i64_t count, remains, len;

    *buffer = (str_t)storm_malloc(MAX_ROW_WIDTH + 4);
    buf = *buffer;
    strncpy(buf, "[", 2);
    buf += 1;

    count = value->list_value.len < 1 ? 0 : value->list_value.len - 1;
    remains = MAX_ROW_WIDTH;
    len = 0;

    for (i64_t i = 0; i < count; i++)
    {
        remains = MAX_ROW_WIDTH - (buf - (*buffer));
        len = snprintf(buf, remains, "%lld, ", ((i64_t *)value->list_value.ptr)[i]);
        if (len < 0)
        {
            return FormatError;
        }
        if (remains < len)
        {
            buf = (*buffer) + MAX_ROW_WIDTH - 1;
            break;
        }
        buf += len;
    }

    remains = MAX_ROW_WIDTH - (buf - (*buffer));
    if (value->list_value.len > 0 && remains > 0)
    {
        len = snprintf(buf, remains, "%lld", ((i64_t *)value->list_value.ptr)[count]);
        if (len < 0)
        {
            return FormatError;
        }
        if (remains < len)
        {
            buf = (*buffer) + MAX_ROW_WIDTH - 1;
        }
        else
        {
            buf += len;
        }
    }

    if (remains < len)
    {
        strncpy(buf, "..", 3);
        buf += 2;
    }

    strncpy(buf, "]", 2);
    return Ok;
}

extern result_t value_fmt(str_t *buffer, value_t value)
{
    switch (value->type)
    {
    case -TYPE_i64:
    {
        return scalar_i64_fmt(buffer, value);
    }
    case TYPE_i64:
    {
        return vector_i64_fmt(buffer, value);
    }
    default:
    {
        return InvalidType;
    }
    }
}

extern nil_t result_fmt(str_t *buffer, result_t result_t)
{
    switch (result_t)
    {
    case Ok:
    {
        *buffer = (str_t)storm_malloc(3);
        strncpy(*buffer, "Ok", 3);
        break;
    }
    case FormatError:
    {
        *buffer = (str_t)storm_malloc(12);
        strncpy(*buffer, "FormatError", 12);
        break;
    }
    case InvalidType:
    {
        *buffer = (str_t)storm_malloc(12);
        strncpy(*buffer, "InvalidType", 12);
        break;
    }
    default:
    {
        *buffer = (str_t)storm_malloc(10);
        strncpy(*buffer, "Unknown", 10);
        break;
    }
    }
}
