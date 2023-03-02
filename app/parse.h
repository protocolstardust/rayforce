#ifndef PARSE_H
#define PARSE_H

#include "../core/storm.h"
#include "lex.h"

extern g0 parse(str filename, str input);

typedef struct Parser
{
    str filename;
    lexer_t lexer;
} __attribute__((aligned(16))) * parser_t;

#endif
