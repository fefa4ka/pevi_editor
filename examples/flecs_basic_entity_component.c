#include <flecs.h>
#include <stdio.h>

typedef struct {
  float x, y, z;
} Position;
typedef struct {
  float dx, dy, dz;
} Velocity;

int main(void) {
  ecs_world_t *world = ecs_init();

  ECS_COMPONENT(
      world,
      Position); // Register component
                 // [oai_citation:3‡Flecs](https://www.flecs.dev/flecs/md_docs_2Quickstart.html)
  ECS_COMPONENT(world, Velocity);

  // Create an empty entity, then set Position
  ecs_entity_t e1 = ecs_new(
      world); // Correct: one argument
              // [oai_citation:4‡Flecs](https://www.flecs.dev/flecs/group__creating__entities.html?utm_source=chatgpt.com)
  ecs_set(
      world, e1, Position,
      {1, 2,
       3}); // Adds Position component
            // [oai_citation:5‡Flecs](https://www.flecs.dev/flecs/md_docs_2Quickstart.html)

  // Create another entity, then set Velocity
  ecs_entity_t e2 = ecs_new(world);
  ecs_set(world, e2, Velocity, {0.1f, 0.2f, 0.3f});

  // Create a third entity, then set both
  ecs_entity_t e3 = ecs_new(world);
  ecs_set(world, e3, Position, {5, 6, 7});
  ecs_set(world, e3, Velocity, {0.5f, 0.5f, 0.5f});

  // … continue with queries or systems …

  ecs_fini(world);
  return 0;
}
