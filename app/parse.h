#ifndef PARSE_H
#define PARSE_H

#include "../core/storm.h"
#include "lex.h"

extern value_t parse(str_t filename, str_t input);

typedef struct parser_t
{
    str_t filename;
    lexer_t lexer;
} __attribute__((aligned(16))) * parser_t;

#endif
