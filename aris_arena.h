/*
aris_arena.h - v0.01 - Dylaris 2025
===================================================

BRIEF:
  Arena allocator implementation for C language (C99 required)

NOTICE:
  This implementation uses C99 flexible array members
  and is not compatible with C++.

USAGE:
  In exactly one source file, define the implementation macro
  before including this header:
  ```
    #define ARIS_ARENA_IMPLEMENTATION
    #include "aris_arena.h"
  ```
  In other files, just include the header without the macro.

LICENSE:
  See the end of this file for further details.
*/

#ifndef ARIS_ARENA_H
#define ARIS_ARENA_H

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#error "This library requires C99 or later standard"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct aris_arena_block {
    struct aris_arena_block *next;
    size_t alloc_size;
    size_t used_size;
    uint8_t buffer[];
} aris_arena_block ;

typedef struct aris_arena {
    aris_arena_block *first;
    aris_arena_block *last;
    size_t prealloc_size;
} aris_arena;

#ifndef ARIS_ARENA_PRALLOC_SIZE
#define ARIS_ARENA_PRALLOC_SIZE 1024
#endif

void aris_arena_init(aris_arena *arena, size_t prealloc_size);
void aris_arena_free(aris_arena *arena);
void aris_arena_rewind(aris_arena *arena);
void *aris_arena_alloc(aris_arena *arena, size_t size);
void aris_arena_dump(aris_arena *arena);
#define aris_arena_reset(arena) aris_arena_init(arena, (size_t)0)

#endif /* ARIS_ARENA_H */

#ifdef ARIS_ARENA_IMPLEMENTATION

static aris_arena_block *aris__arena_block_create(aris_arena *arena, size_t size)
{
#define aris__arena_allign_up(n, k) (((n) + (k) - 1) & ~((k) - 1))
    aris_arena_block *block;
    size_t allign_size;

    allign_size = aris__arena_allign_up(size, sizeof(void*));

    block = malloc(sizeof(aris_arena_block) + allign_size);
    if (!block) return NULL;

    block->next = NULL;
    block->alloc_size = allign_size;
    block->used_size = 0;

    if (arena->last) arena->last->next = block;
    arena->last = block;
    if (!arena->first) arena->first = arena->last;

    return block;
#undef aris__arena_allign_up
}

static void *aris__arena_block_alloc(aris_arena_block *block, size_t size)
{
    if (size > block->alloc_size - block->used_size) return NULL;

    block->used_size += size;
    return block->buffer + (block->used_size - size);
}

void aris_arena_init(aris_arena *arena, size_t prealloc_size)
{
    arena->first = NULL;
    arena->last = NULL;
    arena->prealloc_size = prealloc_size;
}

void aris_arena_free(aris_arena *arena)
{
    if (!arena) return;

    aris_arena_block *current = arena->first;
    while (current) {
        aris_arena_block *tmp = current->next;
        free(current);
        current = tmp;
    }

    aris_arena_reset(arena);
}

void aris_arena_rewind(aris_arena *arena)
{
    for (aris_arena_block *block = arena->first; block; block = block->next) {
        block->used_size = 0;
    }
}

void *aris_arena_alloc(aris_arena *arena, size_t size)
{
    aris_arena_block *block;

    for (block = arena->first; block; block = block->next) {
        void *p_alloc = aris__arena_block_alloc(block, size);
        if (p_alloc) return p_alloc;
    }

    block = aris__arena_block_create(arena,
                 size > (ARIS_ARENA_PRALLOC_SIZE >> 1)
                 ? (size << 1) : ARIS_ARENA_PRALLOC_SIZE);
    if (!block) return NULL;

    return aris__arena_block_alloc(block, size);
}

void aris_arena_dump(aris_arena *arena)
{
    size_t id = 1;
    for (aris_arena_block *block = arena->first; block; block = block->next) {
        printf("Buffer <%zu>: \n"
               "  Allocated: %zu\n"
               "  Used:      %zu\n",
               id,
               block->alloc_size,
               block->used_size);
        id++;
    }
}

#endif /* ARIS_ARENA_IMPLEMENTATION */

#ifdef ARIS_ARENA_STRIP_PREFIX

#define arena_init   aris_arena_init
#define arena_free   aris_arena_free
#define arena_rewind aris_arena_rewind
#define arena_alloc  aris_arena_alloc
#define arena_dump   aris_arena_dump
#define aerna_reset  aris_arena_reset

#endif /* ARIS_ARENA_STRIP_PREFIX */

/*
------------------------------------------------------------------------------
This software is available under MIT license.
------------------------------------------------------------------------------
Copyright (c) 2025 Dylaris
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
