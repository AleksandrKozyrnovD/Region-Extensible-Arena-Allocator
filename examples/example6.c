#include "../arena.h"
#include <stdio.h>

int main(void)
{
    struct arena* arena = arena_create();
    

    size_t checkpoint = arena_get_pos(arena);    
    int* a = arena_alloc(arena, sizeof(int));
    *a = 10;
    printf("Before rewind: %d\n", *a);

    for (int i = 0; i < 3; i++) {
        void* p = arena_alloc(arena, 4096);
        (void)p;
    }
    int *c = arena_alloc(arena, sizeof(int));
    *c = 1000;
    printf("After massive allocation: %d & %d\n", *a, *c);
    
    arena_set_pos_back(arena, checkpoint);
    
    int* b = arena_alloc(arena, sizeof(int));
    *b = 100;
    printf("After rewind: %d & %d & %d\n", *a, *b, *c);
    
    arena_destroy(arena);
    return 0;
}
