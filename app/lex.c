#include "lex.h"
#include "../core/storm.h"
#include "../core/alloc.h"

lexer_t new_lexer(str source)
{
    lexer_t lexer;

    lexer = (lexer_t)a0_malloc(sizeof(struct Lexer));
    lexer->source = source;
    lexer->line = 1;

    return lexer;
}
