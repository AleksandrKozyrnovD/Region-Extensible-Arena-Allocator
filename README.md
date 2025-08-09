# Fast review

[Arena allocator](https://en.m.wikipedia.org/wiki/Region-based_memory_management), or region based allocators in general, is a type of memory management in which each allocated object is assigned to a region. A region, also called a partition, subpool, zone, arena, area, or memory context, is a collection of allocated objects that can be efficiently reallocated or deallocated all at once. 

So it means that it needs low overhead and logic to allocate and deallocate objects. Because all memory resides in big regions, one free for each region is needed to clear all allocated objects. It massively simplifies complex lifetime object management. For example one way list. Tail nodes' lifetime is dependent on head node lifetime (you cant free tail node before freeing head node as it can cause memory leak or UB). There also tree-like lifetime dependencies with n layers of object lifetimes. If all these objects will be inside one contigious region, one free is needed and no complex logic is required. Also it can propogate across all codepaths easily.


My implementation is based on dynamically allocated memory regions on demand with configurable alignment with stack-like properties. When freed, offset position is moved relatively/absolutely to what you gave. Once memory region is allocated, it remains allocated until arena is destroyed. It means that if sufficiently large number of objects allocated and then released, all memory regions will stay for future usage. 

## Usage
1. Copy arena.h arena.c in your project
2. Include arena.h in your project
3. Compile arena.c with your application

Example main.c (of any other .c file)
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

# Examples
This section casts light on proposed use-cases and implementation details.

### Example 1
This [example](examples/example1.c) shows very basic allocation of array and a string with single free.

### Example 2
This [example](examples/example2.c) provides information about its stack-like properties. Once freed/set back position (as it is equivelent operations) will just move offset back.

### Example 3
This [example](examples/example3.c) shows a way to make a contextual allocations. You can wrap specific arena with a functions.

### Example 4
This [example](examples/example4.c) shows example of clearing arena (which means setting offset to 0).

### Example 5
This [example](examples/example5.c) show structures and arrays allocation
