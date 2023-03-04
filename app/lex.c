#include "lex.h"
#include "../core/storm.h"
#include "../core/alloc.h"

lexer_t new_lexer(str_t source)
{
    lexer_t lexer;

    lexer = (lexer_t)storm_malloc(sizeof(struct lexer_t));
    lexer->source = source;
    lexer->line = 1;

    return lexer;
}
