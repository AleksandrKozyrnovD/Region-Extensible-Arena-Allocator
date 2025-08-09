#include "../arena.h"
#include <stdio.h>
#include <string.h>

typedef struct
{
    int id;
    char name[20];
} Person;

int main(void)
{
    struct arena* arena = arena_create();
    
    // Allocate array
    Person* team = arena_alloc(arena, 3 * sizeof(Person));
    
    // Allocate individual string
    char* project = arena_alloc(arena, 25);
    strcpy(project, "Arena Demo");
    
    // Initialize structs
    team[0] = (Person){1, "Alice"};
    team[1] = (Person){2, "Bob"};
    team[2] = (Person){3, "Charlie"};
    
    printf("Project: %s\n", project);
    for (int i = 0; i < 3; i++) {
        printf("Member %d: %s\n", team[i].id, team[i].name);
    }
    
    arena_destroy(arena);
    return 0;
}
