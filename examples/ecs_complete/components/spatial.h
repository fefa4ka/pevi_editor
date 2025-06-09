#ifndef SPATIAL_COMPONENTS_H
#define SPATIAL_COMPONENTS_H

#include <flecs.h>
#include <raylib.h>
#include <time.h>
#include <stdint.h>

// Atomic spatial components for maximum cache efficiency
typedef struct {
    float x, y, z;
} Position;

typedef struct {
    float x, y, z, w;  // Quaternion for smooth rotations
} Rotation;

typedef struct {
    float x, y, z;
} Scale;

typedef struct {
    float x, y, z;
} Velocity;

// Computed transform matrix (updated by TransformSystem)
typedef struct {
    Matrix world_matrix;
    Matrix local_matrix;
    bool needs_update;
} Transform;

// 3D text phantom entity components
typedef struct {
    char text[256];
    float font_size;
    Color color;
    bool billboard_mode;  // Always face camera
} TextContent;

typedef struct {
    char filepath[512];
    int line_number;
    time_t last_modified;
} FileReference;

// Selection and interaction
typedef struct {
    bool is_selected;
    uint32_t selection_id;
    float selection_time;
} Selected;

typedef struct {
    float radius;
    Vector3 center_offset;
} BoundingSphere;

// Camera controller component
typedef struct {
    Vector3 target;
    float distance;
    float pitch, yaw;
    float move_speed;
    float rotation_speed;
    int mode;  // Orbital, free, first-person
} CameraController;

// Editor state management
typedef struct {
    int current_mode;  // Navigation, edit, command
    int previous_mode;
    bool mode_transition;
    ecs_entity_t focused_entity;
} EditorState;

// Tags for state management
extern ECS_DECLARE(Visible);
extern ECS_DECLARE(Hidden);
extern ECS_DECLARE(NeedsReload);

// Custom relationships
extern ECS_DECLARE(References);
extern ECS_DECLARE(Contains);
extern ECS_DECLARE(Imports);
extern ECS_DECLARE(Targets);

// Pipeline phases
extern ECS_DECLARE(InputPhase);
extern ECS_DECLARE(TransformPhase);
extern ECS_DECLARE(CullingPhase);
extern ECS_DECLARE(RenderPhase);

// Component registration function
void RegisterSpatialComponents(ecs_world_t *world);

#endif // SPATIAL_COMPONENTS_H
