#include <flecs.h>
#include <stdio.h>

// Define Position and Velocity components
typedef struct {
  float x, y, z;
} Position;

typedef struct {
  float dx, dy, dz;
} Velocity;

// System that moves entities with Position and Velocity
void MoveSystem(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 1);
    const Velocity *v = ecs_field(it, Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].dx * it->delta_time;
        p[i].y += v[i].dy * it->delta_time;
        p[i].z += v[i].dz * it->delta_time;
        printf("Entity %llu moved to: {%f, %f, %f}\n", 
               (unsigned long long)it->entities[i], p[i].x, p[i].y, p[i].z);
    }
}

int main(void) {
    ecs_world_t *world = ecs_init();

    // Register components
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    // Register system
    ECS_SYSTEM(world, MoveSystem, EcsOnUpdate, Position, [in] Velocity);

    // Create an entity with Position and Velocity
    ecs_entity_t e1 = ecs_new(world);
    ecs_set(world, e1, Position, {0, 0, 0});
    ecs_set(world, e1, Velocity, {1.0f, 0.5f, 0.25f});
    
    ecs_set_name(world, e1, "MyMovingEntity");

    // Create another entity with Position and Velocity
    ecs_entity_t e2 = ecs_new(world);
    ecs_set(world, e2, Position, {10, 10, 10});
    ecs_set(world, e2, Velocity, {-0.5f, -1.0f, -0.1f});
    ecs_set_name(world, e2, "AnotherMovingEntity");

    // Create an entity with only Position (should not be processed by MoveSystem)
    ecs_entity_t e3 = ecs_new(world);
    ecs_set(world, e3, Position, {100, 100, 100});
    ecs_set_name(world, e3, "StaticEntity");


    // Run the simulation for a few frames
    printf("Running simulation...\n");
    for (int i = 0; i < 5; ++i) {
        printf("\n--- Frame %d ---\n", i + 1);
        // ecs_progress returns false if ecs_quit() has been called
        if (!ecs_progress(world, 0)) { // 0 for delta_time means use internal calculation
            break;
        }
        // Optional: Print position of e1 to see changes outside the system
        const Position *p1 = ecs_get(world, e1, Position);
        if (p1) {
            printf("Position of MyMovingEntity after system run: {%f, %f, %f}\n", p1->x, p1->y, p1->z);
        }
        const Position *p2 = ecs_get(world, e2, Position);
        if (p2) {
            printf("Position of AnotherMovingEntity after system run: {%f, %f, %f}\n", p2->x, p2->y, p2->z);
        }
    }

    // Cleanup
    ecs_fini(world);
    return 0;
}
