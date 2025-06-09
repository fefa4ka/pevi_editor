#include <flecs.h>
#include <raylib.h>
#include <stdio.h>

#include "components/spatial.h"
#include "systems/core_systems.h"
#include "systems/observers.h"
#include "systems/prefabs.h"
#include "systems/file_loader.h"

int main(void) {
    // Initialize Raylib
    const int screenWidth = 1200;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Pevi 3D Spatial Code Editor - Flecs ECS Complete Example");
    SetTargetFPS(60);
    
    // Initialize Flecs ECS world
    ecs_world_t *world = ecs_init();
    
    printf("Initializing Pevi ECS Complete Example...\n");
    
    // Register all components and tags
    RegisterSpatialComponents(world);
    
    
    // Register core systems with pipeline phases
    RegisterCoreSystems(world);
    
    // Register observers for reactive behavior
    RegisterObservers(world);
    
    // Create prefabs for code editor elements
    CreatePrefabs(world);
    
    // Load example project files as phantoms
    LoadProjectAsPhantoms(world, "./src");
    
    // Create additional code hierarchy examples
    CreateCodeHierarchy(world);
    SetupCodeDependencies(world);
    
    // Create camera entity with orbital controller
    ecs_entity_t camera_entity = ecs_new(world);
    ecs_set_name(world, camera_entity, "MainCamera");
    ecs_set(world, camera_entity, CameraController, {
        .target = {0.0f, 0.0f, 0.0f},
        .distance = 20.0f,
        .pitch = 30.0f,
        .yaw = 45.0f,
        .move_speed = 10.0f,
        .rotation_speed = 0.5f,
        .mode = 0  // Orbital mode
    });
    
    // Create editor state singleton
    ecs_entity_t editor = ecs_new(world);
    ecs_set_name(world, editor, "Editor");
    ecs_set(world, editor, EditorState, {
        .current_mode = 0,  // Start in navigation mode
        .previous_mode = 0,
        .mode_transition = false,
        .focused_entity = 0
    });
    
    // Create some example function instances using prefabs
    CreateFunctionInstance(world, "init_editor()", (Vector3){5.0f, 5.0f, 0.0f});
    CreateFunctionInstance(world, "update_camera()", (Vector3){-5.0f, 5.0f, 0.0f});
    CreateFunctionInstance(world, "render_phantoms()", (Vector3){0.0f, -5.0f, 0.0f});
    
    // Create file instances
    CreateFileInstance(world, "editor.c");
    CreateFileInstance(world, "renderer.c");
    
    printf("ECS world initialized with %d entities\n", ecs_count_id(world, EcsAny));
    
    // Performance tracking variables
    double last_fps_time = GetTime();
    int frame_count = 0;
    float avg_fps = 0.0f;
    
    // Main game loop
    while (!WindowShouldClose()) {
        double current_time = GetTime();
        float delta_time = GetFrameTime();
        
        // Update FPS tracking
        frame_count++;
        if (current_time - last_fps_time >= 1.0) {
            avg_fps = frame_count / (current_time - last_fps_time);
            frame_count = 0;
            last_fps_time = current_time;
        }
        
        // Update ECS world - this runs all systems in pipeline order
        ecs_progress(world, delta_time);
        
        // Rendering
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Set up 3D camera from camera controller
        const CameraController *cam_ctrl = ecs_get(world, camera_entity, CameraController);
        if (cam_ctrl) {
            CameraController cam_ctrl_copy = *cam_ctrl;
            Camera3D camera = CreateCamera(&cam_ctrl_copy);
            
            BeginMode3D(camera);
            
            // Draw reference grid
            DrawGrid(20, 1.0f);
            
            // Draw coordinate axes for reference
            DrawLine3D((Vector3){0, 0, 0}, (Vector3){5, 0, 0}, RED);    // X axis
            DrawLine3D((Vector3){0, 0, 0}, (Vector3){0, 5, 0}, GREEN);  // Y axis
            DrawLine3D((Vector3){0, 0, 0}, (Vector3){0, 0, 5}, BLUE);   // Z axis
            
            // ECS text rendering happens in TextRenderSystem
            // But we can also draw additional 3D elements here
            
            // Draw selection indicators for focused entities
            const EditorState *editor_state_inner = ecs_get(world, editor, EditorState);
            if (editor_state_inner && editor_state_inner->focused_entity != 0) {
                const Position *pos = ecs_get(world, editor_state_inner->focused_entity, Position);
                if (pos) {
                    Vector3 entity_pos = {pos->x, pos->y, pos->z};
                    DrawSphere(entity_pos, 0.8f, ColorAlpha(YELLOW, 0.3f));
                    DrawSphereWires(entity_pos, 0.8f, 8, 8, YELLOW);
                }
            }
            
            EndMode3D();
        }
        
        // Draw 2D UI overlay
        const EditorState *editor_state = ecs_get(world, editor, EditorState);
        if (editor_state) {
            const char* mode_names[] = {"Navigation", "Edit", "Command"};
            const Color mode_colors[] = {SKYBLUE, GREEN, ORANGE};
            
            DrawText(TextFormat("Mode: %s", mode_names[editor_state->current_mode]), 
                    10, 10, 24, mode_colors[editor_state->current_mode]);
            
            if (editor_state->focused_entity != 0) {
                DrawText(TextFormat("Selected: Entity %llu", editor_state->focused_entity),
                        10, 40, 20, YELLOW);
                
                // Show selected entity information
                const char *entity_name = ecs_get_name(world, editor_state->focused_entity);
                if (entity_name) {
                    DrawText(TextFormat("Name: %s", entity_name), 10, 65, 16, WHITE);
                }
                
                const TextContent *text = ecs_get(world, editor_state->focused_entity, TextContent);
                if (text) {
                    DrawText(TextFormat("Text: \"%.30s%s\"", text->text, 
                            strlen(text->text) > 30 ? "..." : ""), 10, 85, 16, LIGHTGRAY);
                }
            }
            
            // Camera information
            if (cam_ctrl) {
                DrawText(TextFormat("Camera: Distance %.1f, Pitch %.1f°, Yaw %.1f°", 
                        cam_ctrl->distance, cam_ctrl->pitch, cam_ctrl->yaw),
                        10, GetScreenHeight() - 60, 16, LIGHTGRAY);
            }
            
            // Performance information
            DrawText(TextFormat("FPS: %.1f | Entities: %d", avg_fps, ecs_count_id(world, EcsAny)),
                    10, GetScreenHeight() - 40, 16, LIME);
            
            // ECS world statistics
            int visible_count = ecs_count_id(world, Visible);
            int selected_count = ecs_count_id(world, ecs_id(Selected));
            DrawText(TextFormat("Visible: %d | Selected: %d", visible_count, selected_count),
                    10, GetScreenHeight() - 20, 16, LIGHTGRAY);
        }
        
        // Controls help
        DrawText("Controls:", GetScreenWidth() - 300, 10, 20, WHITE);
        DrawText("Left Mouse + Drag: Rotate Camera", GetScreenWidth() - 300, 35, 14, LIGHTGRAY);
        DrawText("Right Mouse + Drag: Pan Camera", GetScreenWidth() - 300, 55, 14, LIGHTGRAY);
        DrawText("Mouse Wheel: Zoom", GetScreenWidth() - 300, 75, 14, LIGHTGRAY);
        DrawText("Left Click: Select Phantom", GetScreenWidth() - 300, 95, 14, LIGHTGRAY);
        DrawText("Tab: Switch Mode", GetScreenWidth() - 300, 115, 14, LIGHTGRAY);
        DrawText("ESC: Exit", GetScreenWidth() - 300, 135, 14, LIGHTGRAY);
        
        // Mode transition feedback
        if (editor_state && editor_state->mode_transition) {
            const Color mode_colors[] = {SKYBLUE, GREEN, ORANGE};
            DrawText("MODE SWITCHED!", GetScreenWidth() / 2 - 100, GetScreenHeight() / 2, 30, mode_colors[editor_state->current_mode]);
            // Reset transition flag after showing feedback
            EditorState *mutable_state = ecs_get_mut(world, editor, EditorState);
            if (mutable_state) {
                mutable_state->mode_transition = false;
            }
        }
        
        EndDrawing();
    }
    
    // Cleanup
    printf("Shutting down ECS world...\n");
    
    // Print final statistics
    printf("Final entity count: %d\n", ecs_count_id(world, EcsAny));
    printf("Visible entities: %d\n", ecs_count_id(world, Visible));
    printf("Text entities: %d\n", ecs_count_id(world, ecs_id(TextContent)));
    
    ecs_fini(world);
    CloseWindow();
    
    printf("Pevi ECS Complete Example shutdown complete.\n");
    return 0;
}
