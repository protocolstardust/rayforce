
#include "lex.h"
#include "parse.h"
#include "../core/storm.h"
#include "../core/alloc.h"

parser_t new_parser(str filename, lexer_t lexer)
{
    parser_t parser;

    parser = (parser_t)a0_malloc(sizeof(struct Parser));
    parser->filename = filename;
    parser->lexer = lexer;
    return parser;
}

g0 _parse(parser_t parser)
{
    UNUSED(parser);
    return new_scalar_i64(123);
}

extern g0 parse(str filename, str input)
{
    lexer_t lexer;
    parser_t parser;
    g0 result;

    lexer = new_lexer(input);
    parser = new_parser(filename, lexer);

    result = _parse(parser);

    a0_free(parser);
    a0_free(lexer);

    return result;
}