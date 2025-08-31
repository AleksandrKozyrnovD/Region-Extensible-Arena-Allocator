#ifndef __MEMORYZONE_H__
#define __MEMORYZONE_H__

#include <stddef.h>

#define DEFAULT_REGION_SIZE 4096
#define DEFAULT_ALINGMENT 1

#ifdef __cplusplus
extern "C" {
#endif

struct arena_region
{
    struct arena_region *next, *prev;
    char *base;
    size_t size;
    size_t used;
};

struct arena
{
    struct arena_region *head;
    struct arena_region *current;
    size_t region_size;
    size_t alignment;
    size_t offset;
    size_t total_offset;
};

#ifdef __ARENADEBUG__
void print_region(struct arena_region *region);
void print_regions(struct arena *arena);
#endif

struct arena *arena_create(void);
void arena_destroy(struct arena *zone);

void *arena_alloc(struct arena *arena, size_t size);
void *arena_calloc(struct arena *arena, size_t size);

void arena_free(struct arena *arena, size_t size);
size_t arena_get_pos(struct arena *arena);

void arena_set_pos_back(struct arena *arena, size_t pos);
void arena_clear(struct arena *arena);

#ifdef __ARENA_IMPL__

#include "arena.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct arena *arena_create(void)
{
    struct arena *arena = (struct arena *)malloc(sizeof(struct arena));
    if (!arena)
        return NULL;
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
            return ptr;
        }
    }

    size_t required_size = align_offset(size, arena->alignment);
    size_t region_size = arena->region_size;
    if (required_size > region_size)
    {
        return NULL;
    }

    if (arena->current && arena->current->next)
    {
        arena->current = arena->current->next;
        aligned_offset = align_offset(0, arena->alignment);
        arena->offset = aligned_offset + size;
        arena->current->used = arena->offset;
        arena->total_offset += size;
        return arena->current->base + aligned_offset;
    }
    struct arena_region *region = (struct arena_region *)malloc(sizeof(struct arena_region));
    if (!region)
        return NULL;
    region->base = (char *)malloc(region_size);
    if (!region->base)
    {
        free(region);
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

    return region->base + aligned_offset;
}

void *arena_calloc(struct arena *arena, size_t size)
{
    void *ptr = arena_alloc(arena, size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}

void arena_free(struct arena *arena, size_t size)
{
    size_t freed_in_current;
    if (!arena || size == 0)
        return;
    size = (size > arena->total_offset) ? arena->total_offset : size;
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
}

size_t arena_get_pos(struct arena *arena)
{
    return arena ? arena->total_offset : 0;
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
    while (region)
    {
        print_region(region);
        region = region->next;
    }
}

#endif //  __ARENA_IMPL__

#ifdef __cplusplus
}
#endif


#endif
