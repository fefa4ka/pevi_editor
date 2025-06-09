#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include <flecs.h>
#include "../components/spatial.h"

// File loading functions
void LoadFileAsPhantoms(ecs_world_t *world, const char* filepath, Vector3 start_position);
void LoadProjectAsPhantoms(ecs_world_t *world, const char* project_path);

// Helper functions
ecs_entity_t CreatePhantomFromLine(ecs_world_t *world, const char* line_text, int line_number, 
                                   const char* filepath, Vector3 position, ecs_entity_t parent);

#endif // FILE_LOADER_H
