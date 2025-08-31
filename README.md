# Fast review

[Arena allocator](https://en.m.wikipedia.org/wiki/Region-based_memory_management), or region based allocators in general, is a type of memory management in which each allocated object is assigned to a region. A region, also called a partition, subpool, zone, arena, area, or memory context, is a collection of allocated objects that can be efficiently reallocated or deallocated all at once. 

This type of allocator needs low overhead and logic to allocate and deallocate objects. Because all memory resides in big regions, one free for each region is needed to clear all allocated memory. It massively simplifies complex lifetime object management. For example one way list. Tail nodes' lifetime is dependent on head node lifetime (you cant free tail node before freeing head node as it can cause memory leak or UB). There also tree-like lifetime dependencies with n layers of object lifetimes. If all these objects will be inside one contigious region, one free is needed and no complex logic is required. Also it can propogate across all codepaths easily.


My implementation is based on dynamically allocated memory regions on demand with configurable alignment. When freed, offset position is moved relatively/absolutely. Once memory region is allocated, it remains allocated until arena is destroyed. This make it unsuitable for complete control of memory usage. Ideally it works for programms or situations that expects constant or finite memory. For example, frame buffers or simple programs. 

All available examples is [here](examples/).

## Usage
1. Copy `arena.h` and `arena.c` in your project
2. Include `arena.h` in your project
3. Compile `arena.c` with your application

Example main.c (or any other .c file)
```c
#include "arena.h"

int main() {
    struct arena* mem = arena_create();
    // ... use arena ...
    arena_destroy(mem);
}
```

## Key Advantages
- **O(1) allocation** after region creation
- **Minimal Fragmentation** for region-contained objects (but can occur at end of each region if allocation is bigger that left)
- **Stack-style rollback** for batch deallocations
- **Region reuse** across clear boundaries
