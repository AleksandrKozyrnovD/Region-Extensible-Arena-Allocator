#include "../arena.h"
#include <stdio.h>

int main(void)
{
    struct arena* arena = arena_create();
    
    size_t checkpoint = arena_get_pos(arena);
    
    int* a = arena_alloc(arena, sizeof(int));
    *a = 42;
    printf("Before rewind: %d\n", *a);
    
    arena_set_pos_back(arena, checkpoint);
    
    int* b = arena_alloc(arena, sizeof(int));
    *b = 100;
    printf("After rewind: %d & %d\n", *b, *a);
    
    arena_destroy(arena);
    return 0;
}
