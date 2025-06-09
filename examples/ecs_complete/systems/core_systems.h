#ifndef CORE_SYSTEMS_H
#define CORE_SYSTEMS_H

#include <flecs.h>
#include "../components/spatial.h"

// System declarations
void InputSystem(ecs_iter_t *it);
void TransformSystem(ecs_iter_t *it);
void CullingSystem(ecs_iter_t *it);
void TextRenderSystem(ecs_iter_t *it);
void HotReloadSystem(ecs_iter_t *it);
void PickingSystem(ecs_iter_t *it);

// Helper functions
Vector3 GetCameraPosition(CameraController camera_ctrl);
Camera3D CreateCamera(CameraController *camera_ctrl);

// System registration
void RegisterCoreSystems(ecs_world_t *world);

#endif // CORE_SYSTEMS_H
