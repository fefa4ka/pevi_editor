#include <flecs.h>
#include <raylib.h>
#include <stdio.h>
#include <math.h>

// Simple component definitions
typedef struct {
    float x, y, z;
} Position;

typedef struct {
    char text[256];
    Color color;
} TextContent;

typedef struct {
    Vector3 target;
    float distance;
    float pitch, yaw;
} CameraController;

// Declare components
ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(TextContent);
ECS_COMPONENT_DECLARE(CameraController);

// Simple 3D text drawing
void DrawText3DSimple(const char *text, Vector3 position, float fontSize, Color color) {
    // Use the Y position to determine stable screen placement
    int screen_y = 100 + (int)((4.0f - position.y) / 2.0f * 30.0f);
    DrawText(text, 20, screen_y, (int)fontSize, color);
}

// Get camera position from controller
Vector3 GetCameraPosition(const CameraController *ctrl) {
    float pitch_rad = ctrl->pitch * DEG2RAD;
    float yaw_rad = ctrl->yaw * DEG2RAD;
    
    return (Vector3){
        ctrl->target.x + ctrl->distance * cosf(pitch_rad) * cosf(yaw_rad),
        ctrl->target.y + ctrl->distance * sinf(pitch_rad),
        ctrl->target.z + ctrl->distance * cosf(pitch_rad) * sinf(yaw_rad)
    };
}

// Input system
void InputSystem(ecs_iter_t *it) {
    CameraController *cameras = ecs_field(it, CameraController, 0);
    
    for (int i = 0; i < it->count; i++) {
        Vector2 mouse_delta = GetMouseDelta();
        bool left_mouse = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        float wheel = GetMouseWheelMove();
        
        if (left_mouse) {
            cameras[i].yaw += mouse_delta.x * 0.5f;
            cameras[i].pitch -= mouse_delta.y * 0.5f;
            // Clamp pitch manually
            if (cameras[i].pitch < -89.0f) cameras[i].pitch = -89.0f;
            if (cameras[i].pitch > 89.0f) cameras[i].pitch = 89.0f;
        }
        
        cameras[i].distance -= wheel * 2.0f;
        // Clamp distance manually  
        if (cameras[i].distance < 1.0f) cameras[i].distance = 1.0f;
        if (cameras[i].distance > 100.0f) cameras[i].distance = 100.0f;
    }
}

// Text render system
void TextRenderSystem(ecs_iter_t *it) {
    Position *positions = ecs_field(it, Position, 0);
    TextContent *texts = ecs_field(it, TextContent, 1);
    
    for (int i = 0; i < it->count; i++) {
        Vector3 pos = {positions[i].x, positions[i].y, positions[i].z};
        DrawText3DSimple(texts[i].text, pos, 20.0f, texts[i].color);
    }
}

int main(void) {
    // Initialize Raylib
    InitWindow(1200, 800, "Pevi ECS Simple Demo");
    SetTargetFPS(60);
    
    // Initialize Flecs
    ecs_world_t *world = ecs_init();
    
    // Register components
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, TextContent);
    ECS_COMPONENT_DEFINE(world, CameraController);
    
    // Register systems
    ECS_SYSTEM(world, InputSystem, EcsOnUpdate, CameraController);
    ECS_SYSTEM(world, TextRenderSystem, EcsOnStore, Position, TextContent);
    
    // Create camera entity
    ecs_entity_t camera_entity = ecs_new(world);
    ecs_set(world, camera_entity, CameraController, {
        .target = {0.0f, 0.0f, 0.0f},
        .distance = 20.0f,
        .pitch = 30.0f,
        .yaw = 45.0f
    });
    
    // Create some text phantoms
    const char* demo_texts[] = {
        "#include <stdio.h>",
        "int main() {",
        "    printf(\"Hello World\");",
        "    return 0;",
        "}"
    };
    
    for (int i = 0; i < 5; i++) {
        ecs_entity_t phantom = ecs_new(world);
        ecs_set(world, phantom, Position, {0.0f, (float)(4 - i) * 2.0f, 0.0f});
        ecs_set(world, phantom, TextContent, {
            .color = (i == 0) ? PURPLE : (i == 1 || i == 4) ? GREEN : WHITE
        });
        strncpy(ecs_get_mut(world, phantom, TextContent)->text, 
                demo_texts[i], sizeof(((TextContent*)0)->text) - 1);
    }
    
    // Main loop
    while (!WindowShouldClose()) {
        // Update ECS
        ecs_progress(world, GetFrameTime());
        
        // Render
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Get camera
        const CameraController *cam_ctrl = ecs_get(world, camera_entity, CameraController);
        if (cam_ctrl) {
            Camera3D camera = {
                .position = GetCameraPosition(cam_ctrl),
                .target = cam_ctrl->target,
                .up = {0.0f, 1.0f, 0.0f},
                .fovy = 45.0f,
                .projection = CAMERA_PERSPECTIVE
            };
            
            BeginMode3D(camera);
            DrawGrid(10, 1.0f);
            EndMode3D();
        }
        
        // UI
        DrawText("Pevi ECS Demo - Left Mouse: Rotate, Wheel: Zoom", 10, 10, 20, YELLOW);
        DrawFPS(GetScreenWidth() - 100, 10);
        
        EndDrawing();
    }
    
    // Cleanup
    ecs_fini(world);
    CloseWindow();
    
    return 0;
}
