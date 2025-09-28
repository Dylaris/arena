/*
Arena.h - v0.01 - Dylaris 2025
===================================================

BRIEF:
  Arena allocator implementation for C language (C99 required)
  (compatible with C++)

USAGE:
  In exactly one source file, define the implementation macro
  before including this header:
  ```
    #define ARENA_IMPLEMENTATION
    #include "arena.h"
  ```
  In other files, just include the header without the macro.

LICENSE:
  See the end of this file for further details.
*/

#ifndef ARENA_H
#define ARENA_H

#if !defined(__cplusplus) && (!defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L)
#error "This library requires C99 or later standard"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct Arena_Block {
    struct Arena_Block *next;
    size_t alloc_size;
    size_t used_size;
    uint8_t buffer[];
} Arena_Block ;

typedef struct Arena {
    Arena_Block *first;
    Arena_Block *last;
    size_t prealloc_size;
} Arena;

#ifndef ARENA_PRALLOC_SIZE
#define ARENA_PRALLOC_SIZE 1024
#endif

void arena_init(Arena *arena, size_t prealloc_size);
void arena_free(Arena *arena);
void arena_rewind(Arena *arena);
void *arena_alloc(Arena *arena, size_t size);
void arena_dump(Arena *arena);
#define arena_reset(arena) arena_init(arena, (size_t)0)

#endif /* ARENA_H */

#ifdef ARENA_IMPLEMENTATION

static Arena_Block *arena__block_create(Arena *arena, size_t size)
{
#define arena__allign_up(n, k) (((n) + (k) - 1) & ~((k) - 1))
    Arena_Block *block;
    size_t allign_size;

    allign_size = arena__allign_up(size, sizeof(void*));

    block = (Arena_Block*)malloc(sizeof(Arena_Block) + allign_size);
    if (!block) return NULL;

    block->next = NULL;
    block->alloc_size = allign_size;
    block->used_size = 0;

    if (arena->last) arena->last->next = block;
    arena->last = block;
    if (!arena->first) arena->first = arena->last;

    return block;
#undef arena__allign_up
}

static void *arena__block_alloc(Arena_Block *block, size_t size)
{
    if (size > block->alloc_size - block->used_size) return NULL;

    block->used_size += size;
    return block->buffer + (block->used_size - size);
}

void arena_init(Arena *arena, size_t prealloc_size)
{
    arena->first = NULL;
    arena->last = NULL;
    arena->prealloc_size = prealloc_size;
}

void arena_free(Arena *arena)
{
    if (!arena) return;

    Arena_Block *current = arena->first;
    while (current) {
        Arena_Block *tmp = current->next;
        free(current);
        current = tmp;
    }

    arena_reset(arena);
}

void arena_rewind(Arena *arena)
{
    for (Arena_Block *block = arena->first; block; block = block->next) {
        block->used_size = 0;
    }
}

void *arena_alloc(Arena *arena, size_t size)
{
    Arena_Block *block;

    for (block = arena->first; block; block = block->next) {
        void *p_alloc = arena__block_alloc(block, size);
        if (p_alloc) return p_alloc;
    }

    block = arena__block_create(arena,
                 size > (ARENA_PRALLOC_SIZE >> 1)
                 ? (size << 1) : ARENA_PRALLOC_SIZE);
    if (!block) return NULL;

    return arena__block_alloc(block, size);
}

void arena_dump(Arena *arena)
{
    size_t id = 1;
    for (Arena_Block *block = arena->first; block; block = block->next) {
        printf("Buffer <%zu>: \n"
               "  Allocated: %zu\n"
               "  Used:      %zu\n",
               id,
               block->alloc_size,
               block->used_size);
        id++;
    }
}

#endif /* ARENA_IMPLEMENTATION */

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
