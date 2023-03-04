
#include "lex.h"
#include "parse.h"
#include "../core/storm.h"
#include "../core/alloc.h"

parser_t new_parser(str_t filename, lexer_t lexer)
{
    parser_t parser;

    parser = (parser_t)storm_malloc(sizeof(struct parser_t));
    parser->filename = filename;
    parser->lexer = lexer;
    return parser;
}

value_t _parse(parser_t parser)
{
    UNUSED(parser);
    return new_scalar_i64(123);
}

extern value_t parse(str_t filename, str_t input)
{
    lexer_t lexer;
    parser_t parser;
    value_t result_t;

    lexer = new_lexer(input);
    parser = new_parser(filename, lexer);

    result_t = _parse(parser);

    storm_free(parser);
    storm_free(lexer);

    return result_t;
}