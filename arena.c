#include "arena.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct arena *arena_create(void)
{
    struct arena *arena = (struct arena *)malloc(sizeof(struct arena));
    if (!arena)
        return NULL;

    rwlock_init(&arena->lock);
    arena->head = NULL;
    arena->current = NULL;
    arena->region_size = DEFAULT_REGION_SIZE;
    arena->alignment = DEFAULT_ALINGMENT;
    arena->offset = 0;
    arena->total_offset = 0;
    return arena;
}

void arena_destroy(struct arena *arena)
{
    struct arena_region *region = arena->head;
    while (region)
    {
        struct arena_region *next = region->next;
        free(region->base);
        free(region);
        region = next;
    }
    free(arena);
}

static size_t align_offset(size_t offset, size_t alingment)
{
    return (offset + alingment - 1) & ~(alingment - 1);
}

void *arena_alloc(struct arena *arena, size_t size)
{
    if (!arena || !size)
        return NULL;

    size_t aligned_offset = 0;
    write_lock(&arena->lock);
    if (arena->current)
    {
        aligned_offset = align_offset(arena->offset, arena->alignment);
        if (aligned_offset + size <= arena->current->size)
        {
            size_t diff = aligned_offset + size - arena->current->used;
            void *ptr = arena->current->base + aligned_offset;
            arena->offset = aligned_offset + size;
            arena->current->used = arena->offset;
            arena->total_offset += diff;
            write_unlock(&arena->lock);
            return ptr;
        }
    }

    size_t required_size = align_offset(size, arena->alignment);
    size_t region_size = arena->region_size;
    if (required_size > region_size)
    {
        write_unlock(&arena->lock);
        return NULL;
    }

    if (arena->current && arena->current->next)
    {
        arena->current = arena->current->next;
        aligned_offset = align_offset(0, arena->alignment);
        arena->offset = aligned_offset + size;
        arena->current->used = arena->offset;
        arena->total_offset += size;
        write_unlock(&arena->lock);
        return arena->current->base + aligned_offset;
    }
    struct arena_region *region = (struct arena_region *)malloc(sizeof(struct arena_region));
    if (!region)
    {
        write_unlock(&arena->lock);
        return NULL;
    }
    region->base = (char *)malloc(region_size);
    if (!region->base)
    {
        free(region);
        write_unlock(&arena->lock);
        return NULL;
    }
    region->size = region_size;
    region->used = 0;
    region->next = NULL;
    region->prev = NULL;
    if (!arena->head)
    {
        arena->head = region;
    }
    else
    {
        arena->total_offset += arena->current->size - arena->offset;
        arena->current->used = arena->offset;
        arena->current->next = region;
        region->prev = arena->current;
    }
    arena->current = region;

    aligned_offset = align_offset(0, arena->alignment);
    arena->offset = aligned_offset + size;
    region->used = arena->offset;
    arena->total_offset += size;
    write_unlock(&arena->lock);

    return region->base + aligned_offset;
}

void *arena_calloc(struct arena *arena, size_t size)
{
    void *ptr = arena_alloc(arena, size);
    write_lock(&arena->lock);
    if (ptr)
        memset(ptr, 0, size);
    write_unlock(&arena->lock);
    return ptr;
}

void arena_free(struct arena *arena, size_t size)
{
    size_t freed_in_current;
    if (!arena || size == 0)
        return;
    size = (size > arena->total_offset) ? arena->total_offset : size;
    write_lock(&arena->lock);
    while (size > 0)
    {
        if (!arena->current)
        {
            arena->total_offset = 0;
            arena->offset = 0;
            arena->current = arena->head;
            break;
        }

        if (size <= arena->offset)
        {
            size_t already_freed = arena->region_size - arena->offset;
            size = already_freed > size
                ? 0
                : size - already_freed;
            arena->offset -= size;
            arena->total_offset -= size;
            arena->current->used = arena->offset;
            break;
        }
        else
        {
            freed_in_current = arena->offset;
            arena->total_offset -= freed_in_current;
            size -= freed_in_current;
            arena->current->used = 0;

            arena->current = arena->current->prev;
            if (arena->current)
            {
                arena->offset = arena->current->used;
            }
            else
            {
                arena->offset = 0;
            }
        }
    }
    write_unlock(&arena->lock);
}

size_t arena_get_pos(struct arena *arena)
{
    read_lock(&arena->lock);
    size_t result = arena ? arena->total_offset : 0;
    read_unlock(&arena->lock);
    return result;
}

void arena_set_pos_back(struct arena *arena, size_t pos)
{
    if (!arena || pos > arena->total_offset)
        return;

    size_t to_free = arena->total_offset - pos;
    arena_free(arena, to_free);
}

void arena_clear(struct arena *arena)
{
    arena_set_pos_back(arena, 0);
}

void print_region(struct arena_region *region)
{
    if (!region)
    {
        printf("NULL\n");
        return;
    }
    printf("region base = %p, size = %zu, used = %zu\n", region->base, region->size, region->used);
}

void print_regions(struct arena *arena)
{
    struct arena_region *region = arena->head;
    read_lock(&arena->lock);
    while (region)
    {
        print_region(region);
        region = region->next;
    }
    read_unlock(&arena->lock);
}
