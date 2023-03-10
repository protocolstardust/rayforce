#ifndef MMAP_H
#define MMAP_H

#define PAGE_SIZE 4096
#define MAP_ANONYMOUS 0x20

#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

// munmap(GLOBAL_A0, sizeof(struct alloc_t));
// a0 alloc;

// alloc = (a0)mmap(NULL, sizeof(struct A0),
//                  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
