#define ARENA_IMPLEMENTATION
#include "arena.h"

int main(void)
{
    Arena arena;
    arena_init(&arena, ARENA_PRALLOC_SIZE);

    void *p1 = arena_alloc(&arena, 256);
    void *p2 = arena_alloc(&arena, 256);
    void *p3 = arena_alloc(&arena, 1024);
    void *p4 = arena_alloc(&arena, 256);
    void *p5 = arena_alloc(&arena, 768);
    void *p6 = arena_alloc(&arena, 128);

    printf("p1: %p\n", p1);
    printf("p2: %p\n", p2);
    printf("p3: %p\n", p3);
    printf("p4: %p\n", p4);
    printf("p5: %p\n", p5);
    printf("p6: %p\n", p6);

    arena_dump(&arena);

    arena_rewind(&arena);

    void *p7 = arena_alloc(&arena, 768);
    void *p8 = arena_alloc(&arena, 128);

    printf("p7: %p\n", p7);
    printf("p8: %p\n", p8);

    arena_dump(&arena);

    arena_free(&arena);
    return 0;
}
