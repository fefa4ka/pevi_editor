#ifndef PREFABS_H
#define PREFABS_H

#include <flecs.h>
#include "../components/spatial.h"

// Prefab creation functions
void CreatePrefabs(ecs_world_t *world);
ecs_entity_t CreateFunctionInstance(ecs_world_t *world, const char* name, Vector3 position);
ecs_entity_t CreateFileInstance(ecs_world_t *world, const char* filename);

// Spatial hierarchy functions
void CreateCodeHierarchy(ecs_world_t *world);
void SetupCodeDependencies(ecs_world_t *world);

#endif // PREFABS_H
