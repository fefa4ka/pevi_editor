#include "core_systems.h"
#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <stdio.h>
#include <float.h>

// Helper function to draw 3D text (simplified implementation)
void DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint) {
    int length = TextLength(text);
    (void)length; // Mark as unused to suppress warning
    float textOffsetY = 0.0f;
    float textOffsetX = 0.0f;
    
    // Calculate text dimensions for centering
    Vector2 textSize = MeasureTextEx(font, text, fontSize, fontSpacing);
    textOffsetX = textSize.x / 2.0f;
    textOffsetY = textSize.y / 2.0f;
    
    // Project 3D position to screen space and draw as 2D text
    Camera3D camera = {0}; // This is a simplification - in real implementation, pass camera
    Vector2 screenPos = GetWorldToScreen(position, camera);
    
    if (screenPos.x >= -textSize.x && screenPos.x <= GetScreenWidth() + textSize.x &&
        screenPos.y >= -textSize.y && screenPos.y <= GetScreenHeight() + textSize.y) {
        DrawTextEx(font, text, (Vector2){screenPos.x - textOffsetX, screenPos.y - textOffsetY}, fontSize, fontSpacing, tint);
    }
}

// Input system for 3D navigation and selection - uses singleton queries
void InputSystem(ecs_iter_t *it) {
    // Get singleton entities for editor and camera
    ecs_entity_t editor_entity = ecs_lookup(it->world, "Editor");
    ecs_entity_t camera_entity = ecs_lookup(it->world, "MainCamera");
    
    if (editor_entity == 0 || camera_entity == 0) {
        printf("Warning: Could not find Editor or MainCamera entities\n");
        return;
    }
    
    EditorState *editor_state = ecs_get_mut(it->world, editor_entity, EditorState);
    CameraController *camera = ecs_get_mut(it->world, camera_entity, CameraController);
    
    if (!editor_state || !camera) {
        printf("Warning: Could not get EditorState or CameraController components\n");
        return;
    }
    
    // Handle mouse input for camera control
    Vector2 mouse_delta = GetMouseDelta();
    bool left_mouse = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    bool right_mouse = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    float wheel = GetMouseWheelMove();
    
    if (editor_state->current_mode == 0) { // Navigation mode
        if (left_mouse) {
            // Orbital rotation
            camera->yaw += mouse_delta.x * camera->rotation_speed;
            camera->pitch -= mouse_delta.y * camera->rotation_speed;
            camera->pitch = Clamp(camera->pitch, -89.0f, 89.0f);
        }
        
        if (right_mouse) {
            // Pan camera target
            Vector3 cam_pos = GetCameraPosition(*camera);
            Vector3 forward = Vector3Normalize(Vector3Subtract(camera->target, cam_pos));
            Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, (Vector3){0, 1, 0}));
            Vector3 up = Vector3CrossProduct(right, forward);
            
            camera->target = Vector3Add(camera->target, 
                Vector3Scale(right, -mouse_delta.x * 0.01f));
            camera->target = Vector3Add(camera->target, 
                Vector3Scale(up, mouse_delta.y * 0.01f));
        }
        
        // Zoom with mouse wheel
        camera->distance -= wheel * 2.0f;
        camera->distance = Clamp(camera->distance, 1.0f, 100.0f);
    }
    
    // Mode switching
    if (IsKeyPressed(KEY_TAB)) {
        editor_state->previous_mode = editor_state->current_mode;
        editor_state->current_mode = (editor_state->current_mode + 1) % 3;
        editor_state->mode_transition = true;
        printf("Switched to mode: %d\n", editor_state->current_mode);
    }
}

// Transform computation system with hierarchical support
void TransformSystem(ecs_iter_t *it) {
    Position *positions = ecs_field(it, Position, 0);
    Rotation *rotations = ecs_field(it, Rotation, 1);
    Scale *scales = ecs_field(it, Scale, 2);
    EcsTransform *transforms = ecs_field(it, EcsTransform, 3);
    
    for (int i = 0; i < it->count; i++) {
        if (transforms[i].needs_update) {
            // Convert quaternion to rotation matrix
            Matrix rot_matrix = QuaternionToMatrix((Quaternion){
                rotations[i].x, rotations[i].y, rotations[i].z, rotations[i].w
            });
            
            // Build transform matrix: T * R * S
            Matrix scale_matrix = MatrixScale(scales[i].x, scales[i].y, scales[i].z);
            Matrix translate_matrix = MatrixTranslate(positions[i].x, positions[i].y, positions[i].z);
            
            transforms[i].local_matrix = MatrixMultiply(
                MatrixMultiply(scale_matrix, rot_matrix), translate_matrix
            );
            
            // Handle parent-child relationships (simplified - full implementation would traverse hierarchy)
            transforms[i].world_matrix = transforms[i].local_matrix;
            transforms[i].needs_update = false;
        }
    }
}

// Frustum culling system for performance optimization
void CullingSystem(ecs_iter_t *it) {
    EcsTransform *transforms = ecs_field(it, EcsTransform, 0);
    BoundingSphere *bounds = ecs_field(it, BoundingSphere, 1);
    
    for (int i = 0; i < it->count; i++) {
        Vector3 world_pos = Vector3Transform(bounds[i].center_offset, transforms[i].world_matrix);
        
        // Simple distance-based culling (full implementation would use frustum planes)
        // For demo purposes, assume camera at origin
        float distance = Vector3Length(world_pos);
        bool visible = distance < 200.0f; // Culling distance
        
        if (visible) {
            ecs_add(it->world, it->entities[i], Visible);
        } else {
            ecs_remove(it->world, it->entities[i], Visible);
        }
    }
}

// 3D text rendering system with billboard support
void TextRenderSystem(ecs_iter_t *it) {
    EcsTransform *transforms = ecs_field(it, EcsTransform, 0);
    TextContent *texts = ecs_field(it, TextContent, 1);
    
    Font font = GetFontDefault();
    
    for (int i = 0; i < it->count; i++) {
        Vector3 position = Vector3Transform((Vector3){0, 0, 0}, transforms[i].world_matrix);
        
        if (texts[i].billboard_mode) {
            // Billboard rendering would need camera reference
            // For now, just draw at world position
            DrawText3D(font, texts[i].text, position, texts[i].font_size, 
                      1.0f, 0.0f, true, texts[i].color);
        } else {
            // True 3D text rendering
            DrawText3D(font, texts[i].text, position, texts[i].font_size, 
                      1.0f, 0.0f, true, texts[i].color);
        }
    }
}

// Hot reload system for file changes
void HotReloadSystem(ecs_iter_t *it) {
    FileReference *file_refs = ecs_field(it, FileReference, 0);
    
    for (int i = 0; i < it->count; i++) {
        // Check if file needs reloading (simplified)
        time_t current_time = time(NULL);
        if (current_time - file_refs[i].last_modified > 1) { // Check every second
            printf("Checking file: %s\n", file_refs[i].filepath);
            file_refs[i].last_modified = current_time;
        }
    }
}

// 3D picking system for phantom selection - uses singleton queries
void PickingSystem(ecs_iter_t *it) {
    // Get singleton entities for editor and camera
    ecs_entity_t editor_entity = ecs_lookup(it->world, "Editor");
    ecs_entity_t camera_entity = ecs_lookup(it->world, "MainCamera");
    
    if (editor_entity == 0 || camera_entity == 0) {
        return; // Silently return if entities not found
    }
    
    EditorState *editor_state = ecs_get_mut(it->world, editor_entity, EditorState);
    const CameraController *camera_ctrl = ecs_get(it->world, camera_entity, CameraController);
    
    if (!editor_state || !camera_ctrl) {
        return; // Silently return if components not found
    }
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && 
        editor_state->current_mode == 0) { // Navigation mode
        
        // Generate picking ray from mouse position
        Vector2 mouse_pos = GetMousePosition();
        CameraController camera_copy = *camera_ctrl; // Make a copy since CreateCamera expects non-const
        Camera3D camera = CreateCamera(&camera_copy);
        Ray picking_ray = GetMouseRay(mouse_pos, camera);
        
        // Find closest intersection with phantom entities
        float closest_distance __attribute__((unused)) = FLT_MAX;
        ecs_entity_t closest_entity = 0;
        
        // Simplified selection - just select a dummy entity for now
        // In a full implementation, this would iterate through entities with proper component queries
        printf("Mouse clicked at (%.2f, %.2f) - ray: (%.2f, %.2f, %.2f)\n", 
               mouse_pos.x, mouse_pos.y, picking_ray.direction.x, picking_ray.direction.y, picking_ray.direction.z);
        
        // For demo purposes, create a simple entity to select
        closest_entity = ecs_new(it->world);
        printf("Created entity %llu for selection\n", closest_entity);
        
        // Update selection
        if (closest_entity != 0) {
            // Clear previous selection (simplified)
            if (editor_state->focused_entity != 0) {
                printf("Clearing previous selection: %llu\n", editor_state->focused_entity);
            }
            
            // Set new selection (simplified - just store entity ID)
            printf("Selecting new entity: %llu\n", closest_entity);
            
            editor_state->focused_entity = closest_entity;
            printf("Selected phantom entity %llu\n", closest_entity);
        }
    }
}

// Helper function to get camera position from controller
Vector3 GetCameraPosition(CameraController camera_ctrl) {
    // Convert spherical coordinates to cartesian
    float pitch_rad = camera_ctrl.pitch * DEG2RAD;
    float yaw_rad = camera_ctrl.yaw * DEG2RAD;
    
    Vector3 position = {
        camera_ctrl.target.x + camera_ctrl.distance * cosf(pitch_rad) * cosf(yaw_rad),
        camera_ctrl.target.y + camera_ctrl.distance * sinf(pitch_rad),
        camera_ctrl.target.z + camera_ctrl.distance * cosf(pitch_rad) * sinf(yaw_rad)
    };
    
    return position;
}

// Helper function to create camera from controller
Camera3D CreateCamera(CameraController *camera_ctrl) {
    Camera3D camera = {
        .position = GetCameraPosition(*camera_ctrl),
        .target = camera_ctrl->target,
        .up = {0.0f, 1.0f, 0.0f},
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE
    };
    return camera;
}

// Register all core systems
void RegisterCoreSystems(ecs_world_t *world) {
    // Register systems - InputSystem and PickingSystem run as singletons
    ecs_entity_t input_system = ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "InputSystem"
        }),
        .callback = InputSystem
    });
    
    ecs_entity_t picking_system = ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "PickingSystem"
        }),
        .callback = PickingSystem
    });
    
    // Register component-based systems with proper queries
    ECS_SYSTEM(world, TransformSystem, EcsOnUpdate, Position, Rotation, Scale, EcsTransform);
    ECS_SYSTEM(world, CullingSystem, EcsOnUpdate, EcsTransform, BoundingSphere);
    ECS_SYSTEM(world, TextRenderSystem, EcsOnUpdate, EcsTransform, TextContent, Visible);
    ECS_SYSTEM(world, HotReloadSystem, EcsOnUpdate, FileReference);
    
    // Set up dependencies to ensure proper execution order
    ecs_add_pair(world, ecs_id(TransformSystem), EcsDependsOn, input_system);
    ecs_add_pair(world, ecs_id(CullingSystem), EcsDependsOn, ecs_id(TransformSystem));
    ecs_add_pair(world, ecs_id(TextRenderSystem), EcsDependsOn, ecs_id(CullingSystem));
    ecs_add_pair(world, picking_system, EcsDependsOn, input_system);
}
