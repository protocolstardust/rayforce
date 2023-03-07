#ifndef PARSE_H
#define PARSE_H

#include "../core/storm.h"

typedef struct parser_t
{
    str_t filename;
    str_t input;
    str_t current;
    i64_t line;
    i64_t column;
} __attribute__((aligned(16))) * parser_t;

parser_t new_parser();
nil_t free_parser(parser_t parser);

extern value_t parse(parser_t parser, str_t filename, str_t input);

#endif
