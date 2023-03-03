#ifndef ALLOC_H_
#define ALLOC_H_

#include "storm.h"

#define PAGE_SIZE 4096

#define MIN_ORDER 2
#define MAX_ORDER 10

#define POOL_SIZE (PAGE_SIZE * (1 << MAX_ORDER))
#define BLOCK_SIZE(i) (1 << (i))

#define POOL_BASE ((i64)GLOBAL_A0->pool)
#define MEM_OFFSET(b) ((i64)b - POOL_BASE)
#define BUDDY_OF(b, i) ((void *)((MEM_OFFSET(b) ^ (1 << (i))) + POOL_BASE))

typedef struct A0
{
    void *freelist[MAX_ORDER + 2];
    i8 pool[POOL_SIZE];
} __attribute__((aligned(PAGE_SIZE))) * a0;

CASSERT(sizeof(struct A0) % PAGE_SIZE == 0, alloc_h)

extern void *a0_malloc(int size);
extern void a0_free(void *block);

extern void a0_init();
extern void a0_deinit();

#endif
