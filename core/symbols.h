#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "storm.h"
#include "hash.h"

typedef struct symbols_t
{
    hash_table_t *str_to_id;
    hash_table_t *id_to_str;
} symbols_t;

i64_t symbols_intern(symbols_t *symbols, str_t str);
str_t symbols_get(symbols_t *symbols, i64_t key);

symbols_t *symbols_create();
null_t symbols_free(symbols_t *symbols);

#endif
