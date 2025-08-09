#ifndef __MEMORYZONE_H__
#define __MEMORYZONE_H__

#include <stddef.h>

#define DEFAULT_REGION_SIZE 4096
#define DEFAULT_ALINGMENT 8

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


#ifdef __cplusplus
}
#endif


#endif
