#define ARIS_ARENA_IMPLEMENTATION
#include "aris_arena.h"

int main(void)
{
    aris_arena arena;
    aris_arena_init(&arena, ARIS_ARENA_PRALLOC_SIZE);

    void *p1 = aris_arena_alloc(&arena, 256);
    void *p2 = aris_arena_alloc(&arena, 256);
    void *p3 = aris_arena_alloc(&arena, 1024);
    void *p4 = aris_arena_alloc(&arena, 256);
    void *p5 = aris_arena_alloc(&arena, 768);
    void *p6 = aris_arena_alloc(&arena, 128);

    printf("p1: %p\n", p1);
    printf("p2: %p\n", p2);
    printf("p3: %p\n", p3);
    printf("p4: %p\n", p4);
    printf("p5: %p\n", p5);
    printf("p6: %p\n", p6);

    aris_arena_dump(&arena);

    aris_arena_rewind(&arena);

    void *p7 = aris_arena_alloc(&arena, 768);
    void *p8 = aris_arena_alloc(&arena, 128);

    printf("p7: %p\n", p7);
    printf("p8: %p\n", p8);

    aris_arena_dump(&arena);

    aris_arena_free(&arena);
    return 0;
}
