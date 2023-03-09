#ifndef HASH_H
#define HASH_H

#include "storm.h"

#define DEFAULT_SIZE 4096 * 1024

typedef struct bucket_t
{
    null_t *key;
    null_t *val;
    struct bucket_t *next;
} bucket_t;

typedef struct hash_table_t
{
    u64_t cap;  // Total capacity of the table (always a multiplier of DEFAULT_SIZE)
    u64_t size; // Actual size of the table in elements
    bucket_t **buckets;
    u64_t (*hasher)(null_t *a);
    i32_t (*compare)(null_t *a, null_t *b);
} hash_table_t;

hash_table_t *ht_create(u64_t (*hasher)(null_t *a), i32_t (*compare)(null_t *a, null_t *b));
null_t ht_free(hash_table_t *table);

null_t *ht_insert(hash_table_t *table, null_t *key, null_t *val);
null_t *ht_insert_with(hash_table_t *table, null_t *key, null_t *val, null_t *(*func)(null_t *key, null_t *val, bucket_t *bucket));
null_t *ht_get(hash_table_t *table, null_t *key);

#endif
