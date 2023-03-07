#include <stdio.h>
#include "parse.h"
#include "../core/storm.h"
#include "../core/alloc.h"
#include "../core/format.h"
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

parser_t new_parser()
{
    parser_t parser;

    parser = (parser_t)storm_malloc(sizeof(struct parser_t));
    return parser;
}

nil_t free_parser(parser_t parser)
{
    storm_free(parser);
}

u8_t is_whitespace(u8_t c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

u8_t is_digit(u8_t c)
{
    return c >= '0' && c <= '9';
}

u8_t is_alpha(u8_t c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

u8_t is_alphanum(u8_t c)
{
    return is_alpha(c) || is_digit(c);
}

u8_t at_eof(u8_t c)
{
    return c == '\n';
}

value_t next(parser_t parser)
{
    str_t *current = &parser->current;

    if (at_eof(**current))
        return NULL;

    while (is_whitespace(**current))
        (*current)++;

    if ((**current) == '-' || is_digit(**current))
    {
        str_t end;
        i64_t num;
        value_t res;
        f64_t dnum;

        errno = 0;

        num = strtoll(*current, &end, 10);

        if ((num == LONG_MAX || num == LONG_MIN) && errno == ERANGE)
            return new_error(ERR_PARSE, "Number out of range");

        if (end == *current)
            return new_error(ERR_PARSE, "Invalid number");

        // try double instead
        if (*end == '.')
        {
            dnum = strtod(*current, &end);

            if (errno == ERANGE)
                return new_error(ERR_PARSE, "Number out of range");

            res = new_scalar_f64(dnum);
        }
        else
        {

            res = new_scalar_i64(num);
        }

        *current = end;

        return res;
    }

    return NULL;
}

value_t parse_program(parser_t parser)
{
    str_t err_msg;
    value_t token;

    // do
    // {
    token = next(parser);
    // } while (token != NULL);

    if (!at_eof(*parser->current))
        return new_error(ERR_PARSE, str_fmt("Unexpected token: %s", parser->current));

    return token;
}

extern value_t parse(parser_t parser, str_t filename, str_t input)
{
    value_t result_t;

    parser->filename = filename;
    parser->input = input;
    parser->current = input;

    result_t = parse_program(parser);

    return result_t;
}
