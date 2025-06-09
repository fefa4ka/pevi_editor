#ifndef OBSERVERS_H
#define OBSERVERS_H

#include <flecs.h>
#include "../components/spatial.h"

// Observer callback declarations
void OnSelectionChanged(ecs_iter_t *it);
void OnFileModified(ecs_iter_t *it);

// Observer registration
void RegisterObservers(ecs_world_t *world);

#endif // OBSERVERS_H
