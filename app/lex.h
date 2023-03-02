#ifndef LEX_H
#define LEX_H

#include "../core/storm.h"

enum Token
{
    Lparen,    // (
    Rparen,    // )
    Lbrace,    // {
    Rbrace,    // }
    Lbracket,  // [
    Rbracket,  // ]
    Comma,     // ,
    Dot,       // .
    Minus,     // -
    Plus,      // +
    Semicolon, // ;
    Slash,     // /
    Star,      // *
    Bang,      // !
    BangEqual, // !=
};

typedef struct Lexer
{
    str source;
    i64 line;
    i64 column;
} __attribute__((aligned(16))) * lexer_t;

lexer_t new_lexer(str source);

#endif
