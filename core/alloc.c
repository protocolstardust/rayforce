#include <stdio.h>

#include "alloc.h"
#include "storm.h"

#include <assert.h>

// Global allocator reference
alloc_t GLOBAL_A0 = NULL;

extern null_t *storm_malloc(i32_t size)
{
    return malloc(size);
}

extern null_t storm_free(null_t *block)
{
    free(block);
}

extern null_t *storm_realloc(null_t *ptr, i32_t size)
{
    return realloc(ptr, size);
}

extern null_t storm_alloc_init()
{
    alloc_t alloc;

    alloc = (alloc_t)mmap(NULL, sizeof(struct alloc_t),
                          PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    alloc->symbols = symbols_create();

    GLOBAL_A0 = alloc;
}

extern null_t storm_alloc_deinit()
{
    alloc_t alloc = alloc_get();
    symbols_free(alloc->symbols);
    // munmap(GLOBAL_A0, sizeof(struct alloc_t));
}

extern alloc_t alloc_get()
{
    return GLOBAL_A0;
}
