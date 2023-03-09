#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "symbols.h"
#include "storm.h"
#include "alloc.h"

/*
 * djb2
 * by Dan Bernstein
 * http://www.cse.yorku.ca/~oz/hash.html
 */
u64_t str_hash(null_t *val)
{
    str_t str = (str_t)val;
    u64_t hash = 5381;

    while (*str)
        hash = ((hash << 5) + hash) + *str++;

    return hash;
}

u64_t i64_hash(null_t *val)
{
    return (u64_t)val;
}

i32_t i64_cmp(null_t *a, null_t *b)
{
    return !((u64_t)a == (u64_t)b);
}

/*
This callback will be called on new buckets being added to a hash table.
In case of symbols this avoids having to copy the string every time.
*/
null_t *str_dup(null_t *key, null_t *val, bucket_t *bucket)
{
    str_t str = (str_t)key;
    str_t new_str = (str_t)storm_malloc(strlen(str) + 1);
    strcpy(new_str, str);
    bucket->key = new_str;
    bucket->val = val;
    return bucket->key;
}

symbols_t *symbols_create()
{
    symbols_t *table = (symbols_t *)storm_malloc(sizeof(symbols_t));

    table->str_to_id = ht_create(&str_hash, ((i32_t((*)(null_t *, null_t *)))strcmp));
    table->id_to_str = ht_create(&i64_hash, &i64_cmp);

    return table;
}

null_t symbols_free(symbols_t *symbols)
{
    ht_free(symbols->str_to_id);
    ht_free(symbols->id_to_str);
}

i64_t symbols_intern(symbols_t *symbols, str_t str)
{
    u64_t id = symbols->str_to_id->size;
    u64_t id_or_str = (u64_t)ht_insert_with(symbols->str_to_id, str, (null_t *)id, &str_dup);
    if (symbols->str_to_id->size == id)
        return id_or_str;

    ht_insert(symbols->id_to_str, (null_t *)id, (str_t)id_or_str);
    return id;
}

str_t symbols_get(symbols_t *symbols, i64_t key)
{
    return (str_t)ht_get(symbols->id_to_str, (null_t *)key);
}