#include <flecs.h>
#include <stdio.h>

// Define a component for position
typedef struct {
    float x;
    float y;
    float z;
} ExamplePosition;

// Define a component for velocity
typedef struct {
    float dx;
    float dy;
    float dz;
} ExampleVelocity;

int main(int argc, char *argv[]) {
    // 1. Initialize a Flecs world
    ecs_world_t *world = ecs_init();
    printf("Flecs world initialized.\n");

    // 2. Define simple components
    // Register components with Flecs
    // The ECS_COMPONENT_DEFINE macro uses the world and the component type.
    ECS_COMPONENT_DEFINE(world, ExamplePosition);
    ECS_COMPONENT_DEFINE(world, ExampleVelocity);
    printf("ExamplePosition and ExampleVelocity components registered.\n");

    // 3. Create a few entities
    ecs_entity_t entity1 = ecs_new_id(world);
    ecs_entity_t entity2 = ecs_new_id(world);
    ecs_entity_t entity3 = ecs_new_id(world);

    printf("Created entities: %llu, %llu, %llu\n", 
           (unsigned long long)entity1, 
           (unsigned long long)entity2, 
           (unsigned long long)entity3);

    // 4. Add components to these entities with initial values
    // Add ExamplePosition to entity1
    ecs_set(world, entity1, ExamplePosition, {1.0f, 2.0f, 3.0f});
    // Add ExampleVelocity to entity1
    ecs_set(world, entity1, ExampleVelocity, {0.1f, 0.2f, 0.3f});

    // Add ExamplePosition to entity2
    ecs_set(world, entity2, ExamplePosition, {4.0f, 5.0f, 6.0f});
    // Entity2 does not have ExampleVelocity

    // Add ExamplePosition to entity3
    ecs_set(world, entity3, ExamplePosition, {7.0f, 8.0f, 9.0f});
    // Add ExampleVelocity to entity3
    ecs_set(world, entity3, ExampleVelocity, {0.4f, 0.5f, 0.6f});
    
    printf("Components added to entities.\n");

    // 5. Iterate over entities with ExamplePosition and print their coordinates
    printf("\nIterating over entities with ExamplePosition component:\n");
    ecs_iter_t it = ecs_query_iter(world, ecs_query(world, { .terms = {{ ecs_id(ExamplePosition) }} }));

    while (ecs_query_next(&it)) {
        ExamplePosition *p = ecs_field(&it, ExamplePosition, 1); // Get the ExamplePosition component array, index is 1-based

        for (int i = 0; i < it.count; i++) {
            printf("Entity %llu - ExamplePosition: {x: %.2f, y: %.2f, z: %.2f}\n",
                   (unsigned long long)it.entities[i], 
                   p[i].x, p[i].y, p[i].z);
            
            // Optionally, check if the entity also has ExampleVelocity
            if (ecs_has(world, it.entities[i], ExampleVelocity)) {
                const ExampleVelocity *v = ecs_get(world, it.entities[i], ExampleVelocity);
                printf("  Entity %llu - ExampleVelocity: {dx: %.2f, dy: %.2f, dz: %.2f}\n",
                       (unsigned long long)it.entities[i],
                       v->dx, v->dy, v->dz);
            } else {
                printf("  Entity %llu - Does not have ExampleVelocity component.\n", (unsigned long long)it.entities[i]);
            }
        }
    }

    // Cleanup the world
    printf("\nCleaning up Flecs world.\n");
    ecs_fini(world);

    return 0;
}
