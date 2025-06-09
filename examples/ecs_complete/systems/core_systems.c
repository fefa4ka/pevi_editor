#include "core_systems.h"
#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <stdio.h>
#include <float.h>

// Input system for 3D navigation and selection
void InputSystem(ecs_iter_t *it) {
    EditorState *editor_state = ecs_field(it, EditorState, 0);
    CameraController *camera = ecs_field(it, CameraController, 1);
    
    for (int i = 0; i < it->count; i++) {
        // Handle mouse input for camera control
        Vector2 mouse_delta = GetMouseDelta();
        bool left_mouse = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        bool right_mouse = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
        float wheel = GetMouseWheelMove();
        
        if (editor_state[i].current_mode == 0) { // Navigation mode
            if (left_mouse) {
                // Orbital rotation
                camera[i].yaw += mouse_delta.x * camera[i].rotation_speed;
                camera[i].pitch -= mouse_delta.y * camera[i].rotation_speed;
                camera[i].pitch = Clamp(camera[i].pitch, -89.0f, 89.0f);
            }
            
            if (right_mouse) {
                // Pan camera target
                Vector3 cam_pos = GetCameraPosition(camera[i]);
                Vector3 forward = Vector3Normalize(Vector3Subtract(camera[i].target, cam_pos));
                Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, (Vector3){0, 1, 0}));
                Vector3 up = Vector3CrossProduct(right, forward);
                
                camera[i].target = Vector3Add(camera[i].target, 
                    Vector3Scale(right, -mouse_delta.x * 0.01f));
                camera[i].target = Vector3Add(camera[i].target, 
                    Vector3Scale(up, mouse_delta.y * 0.01f));
            }
            
            // Zoom with mouse wheel
            camera[i].distance -= wheel * 2.0f;
            camera[i].distance = Clamp(camera[i].distance, 1.0f, 100.0f);
        }
        
        // Mode switching
        if (IsKeyPressed(KEY_TAB)) {
            editor_state[i].previous_mode = editor_state[i].current_mode;
            editor_state[i].current_mode = (editor_state[i].current_mode + 1) % 3;
            editor_state[i].mode_transition = true;
            printf("Switched to mode: %d\n", editor_state[i].current_mode);
        }
    }
}

// Transform computation system with hierarchical support
void TransformSystem(ecs_iter_t *it) {
    Position *positions = ecs_field(it, Position, 0);
    Rotation *rotations = ecs_field(it, Rotation, 1);
    Scale *scales = ecs_field(it, Scale, 2);
    Transform *transforms = ecs_field(it, Transform, 3);
    
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
    Transform *transforms = ecs_field(it, Transform, 0);
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
    Transform *transforms = ecs_field(it, Transform, 0);
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

// 3D picking system for phantom selection
void PickingSystem(ecs_iter_t *it) {
    EditorState *editor_state = ecs_field(it, EditorState, 0);
    CameraController *camera_ctrl = ecs_field(it, CameraController, 1);
    
    for (int i = 0; i < it->count; i++) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && 
            editor_state[i].current_mode == 0) { // Navigation mode
            
            // Generate picking ray from mouse position
            Vector2 mouse_pos = GetMousePosition();
            Camera3D camera = CreateCamera(&camera_ctrl[i]);
            Ray picking_ray = GetMouseRay(mouse_pos, camera);
            
            // Find closest intersection with phantom entities
            float closest_distance = FLT_MAX;
            ecs_entity_t closest_entity = 0;
            
            // Query all selectable phantoms
            ecs_query_t *selectable_query = ecs_query(it->world, {
                .terms = {
                    { ecs_id(Transform) },
                    { ecs_id(BoundingSphere) },
                    { ecs_id(TextContent) }
                }
            });
            
            ecs_iter_t select_it = ecs_query_iter(it->world, selectable_query);
            while (ecs_query_next(&select_it)) {
                Transform *transforms = ecs_field(&select_it, Transform, 0);
                BoundingSphere *bounds = ecs_field(&select_it, BoundingSphere, 1);
                
                for (int j = 0; j < select_it.count; j++) {
                    Vector3 world_pos = Vector3Transform(
                        bounds[j].center_offset, transforms[j].world_matrix);
                    
                    // Ray-sphere intersection test
                    RayCollision collision = GetRayCollisionSphere(
                        picking_ray, world_pos, bounds[j].radius);
                    
                    if (collision.hit && collision.distance < closest_distance) {
                        closest_distance = collision.distance;
                        closest_entity = select_it.entities[j];
                    }
                }
            }
            ecs_query_fini(selectable_query);
            
            // Update selection
            if (closest_entity != 0) {
                // Clear previous selection
                if (editor_state[i].focused_entity != 0) {
                    ecs_remove(it->world, editor_state[i].focused_entity, Selected);
                }
                
                // Set new selection
                ecs_set(it->world, closest_entity, Selected, {
                    .is_selected = true,
                    .selection_id = (uint32_t)closest_entity,
                    .selection_time = GetTime()
                });
                
                editor_state[i].focused_entity = closest_entity;
                printf("Selected phantom entity %llu\n", closest_entity);
            }
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
    // Register systems with pipeline phases
    ECS_SYSTEM(world, InputSystem, InputPhase, EditorState, CameraController);
    ECS_SYSTEM(world, TransformSystem, TransformPhase, Position, Rotation, Scale, Transform);
    ECS_SYSTEM(world, CullingSystem, CullingPhase, Transform, BoundingSphere);
    ECS_SYSTEM(world, TextRenderSystem, RenderPhase, Transform, TextContent, Visible);
    ECS_SYSTEM(world, HotReloadSystem, EcsOnUpdate, FileReference);
    ECS_SYSTEM(world, PickingSystem, InputPhase, EditorState, CameraController);
    
    // Set up dependencies to ensure proper execution order
    ecs_add_pair(world, ecs_id(TransformSystem), EcsDependsOn, ecs_id(InputSystem));
    ecs_add_pair(world, ecs_id(CullingSystem), EcsDependsOn, ecs_id(TransformSystem));
    ecs_add_pair(world, ecs_id(TextRenderSystem), EcsDependsOn, ecs_id(CullingSystem));
}
