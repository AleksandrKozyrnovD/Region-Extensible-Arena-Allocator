#include "../arena.h"
#include <stdio.h>

int main(void)
{
    struct arena* arena = arena_create();
    
    int* nums = arena_alloc(arena, 3 * sizeof(int));
    nums[0] = 10;
    nums[1] = 20;
    nums[2] = 30;
    
    char* str = arena_calloc(arena, 12);
    snprintf(str, 12, "Hello %d", nums[1]);
    
    printf("Numbers: %d, %d, %d\n", nums[0], nums[1], nums[2]);
    printf("String: %s\n", str);
    
    arena_destroy(arena);
    return 0;
}
