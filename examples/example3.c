#include "../arena.h"
#include <stdio.h>

static struct arena* ctx_arena = NULL;

void* ctx_alloc(size_t size)
{
    return arena_alloc(ctx_arena, size);
}

void init_context(void)
{
    ctx_arena = arena_create();
}

void free_context(void)
{
    arena_destroy(ctx_arena);
}

int main(void)
{
    init_context();
    
    double* data = ctx_alloc(5 * sizeof(double));
    for (int i = 0; i < 5; i++) {
        data[i] = i * 1.5;
        printf("data[%d] = %.2f\n", i, data[i]);
    }
    
    free_context();
    return 0;
}
