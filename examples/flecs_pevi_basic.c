#include <stdio.h>
#include <flecs.h>

// Define components
typedef struct {
    double x, y, z;
} Position;

typedef struct {
    double x, y, z;
} Velocity;

// Movement system
void move_system(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = ecs_field(it, Velocity, 1);
    
    for (int i = 0; i < it->count; i++) {
        p[i].x += v[i].x * it->delta_time;
        p[i].y += v[i].y * it->delta_time;
        p[i].z += v[i].z * it->delta_time;
        
        printf("Entity %lu moved to (%.2f, %.2f, %.2f)\n", 
               (unsigned long)it->entities[i], p[i].x, p[i].y, p[i].z);
    }
}

int main() {
    // Create the world
    ecs_world_t *world = ecs_init();
    
    // Register components
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    // Create a system that moves entities
    ECS_SYSTEM(world, move_system, EcsOnUpdate, Position, Velocity);
    
    // Create an entity with position and velocity
    ecs_entity_t e = ecs_new(world);
    ecs_set_name(world, e, "MyEntity");
    ecs_set(world, e, Position, {0, 0, 0});
    ecs_set(world, e, Velocity, {1, 2, 3});
    
    printf("Pevi Basic ECS Example\n");
    printf("Running simulation for 5 steps...\n");
    
    // Run the simulation for a few steps
    for (int i = 0; i < 5; i++) {
        printf("Step %d:\n", i + 1);
        ecs_progress(world, 0.016f); // 16ms per frame (60 FPS)
    }
    
    // Cleanup
    ecs_fini(world);
    
    printf("Simulation complete.\n");
    return 0;
}
