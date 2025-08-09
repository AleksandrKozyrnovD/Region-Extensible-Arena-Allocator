#define __ARENADEBUG__
#include "../arena.h"
#include <stdio.h>

int main(void)
{
    struct arena* arena = arena_create();
    if (!arena)
        return 1;
    
    // First use
    long* arr1 = arena_calloc(arena, 5 * sizeof(long));
    arr1[3] = 12345;
    printf("First allocation: %ld\n", arr1[3]);
    
    arena_clear(arena);
    
    // Second use
    float* arr2 = arena_alloc(arena, 5 * sizeof(float));
    if (!arr2)
        return 1;
    arr2[0] = 3.14159f;
    printf("After clear: %.5f, %ld\n", arr2[0], arr1[3]);
    
    arena_destroy(arena);
    return 0;
}
