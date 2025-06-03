#include <flecs.h>
#include <stdio.h>
#include <string.h>

// Core Pevi Components
typedef struct {
    float x, y, z;
} Position;

typedef struct {
    float pitch, yaw, roll;
} Rotation;

typedef struct {
    float x, y, z;
    float target_x, target_y, target_z;
    float fov;
} Camera;

typedef struct {
    char* content;
    size_t length;
    size_t capacity;
    bool is_dirty;
    char* filepath;  // NULL if unsaved buffer
} TextBuffer;

typedef struct {
    float width, height;
    bool is_focused;
    bool is_visible;
    int phantom_id;
} Phantom;

typedef struct {
    bool is_navigation_mode;
    bool is_edit_mode;
    bool is_command_mode;
} EditorMode;

// Tags for different entity types
ECS_TAG_DECLARE(PhantomTag);
ECS_TAG_DECLARE(CameraTag);
ECS_TAG_DECLARE(EditorTag);

// Forward declare components
ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Rotation);
ECS_COMPONENT_DECLARE(Camera);
ECS_COMPONENT_DECLARE(TextBuffer);
ECS_COMPONENT_DECLARE(Phantom);
ECS_COMPONENT_DECLARE(EditorMode);

// System to update phantom positions (smooth interpolation)
void UpdatePhantomPositions(ecs_iter_t *it) {
    Position *pos = ecs_field(it, Position, 1);
    Phantom *phantom = ecs_field(it, Phantom, 2);
    
    for (int i = 0; i < it->count; i++) {
        if (phantom[i].is_visible) {
            // Simple interpolation towards target position could be added here
            printf("Phantom %d at position (%.1f, %.1f, %.1f)\n", 
                   phantom[i].phantom_id, pos[i].x, pos[i].y, pos[i].z);
        }
    }
}

// System to manage text buffer state
void UpdateTextBuffers(ecs_iter_t *it) {
    TextBuffer *buffer = ecs_field(it, TextBuffer, 1);
    
    for (int i = 0; i < it->count; i++) {
        if (buffer[i].is_dirty) {
            printf("Text buffer needs saving: %s\n", 
                   buffer[i].filepath ? buffer[i].filepath : "unsaved");
        }
    }
}

// System to handle camera updates
void UpdateCamera(ecs_iter_t *it) {
    Camera *cam = ecs_field(it, Camera, 1);
    Position *pos = ecs_field(it, Position, 2);
    
    for (int i = 0; i < it->count; i++) {
        // Simple camera follow logic
        printf("Camera at (%.1f, %.1f, %.1f) looking at (%.1f, %.1f, %.1f)\n",
               pos[i].x, pos[i].y, pos[i].z,
               cam[i].target_x, cam[i].target_y, cam[i].target_z);
    }
}

// Helper function to create a phantom entity
ecs_entity_t create_phantom(ecs_world_t *world, float x, float y, float z, const char* content) {
    ecs_entity_t phantom = ecs_new(world);
    
    // Add components
    ecs_add(world, phantom, PhantomTag);
    ecs_set(world, phantom, Position, {x, y, z});
    ecs_set(world, phantom, Rotation, {0, 0, 0});
    
    // Create text buffer
    TextBuffer buffer = {0};
    if (content) {
        size_t len = strlen(content);
        buffer.content = malloc(len + 1);
        strcpy(buffer.content, content);
        buffer.length = len;
        buffer.capacity = len + 1;
    }
    buffer.is_dirty = false;
    buffer.filepath = NULL;
    
    ecs_set_ptr(world, phantom, TextBuffer, &buffer);
    
    // Set phantom properties
    static int phantom_counter = 1;
    ecs_set(world, phantom, Phantom, {200.0f, 150.0f, true, true, phantom_counter++});
    
    return phantom;
}

// Helper function to create camera entity
ecs_entity_t create_camera(ecs_world_t *world) {
    ecs_entity_t camera = ecs_new(world);
    
    ecs_add(world, camera, CameraTag);
    ecs_set(world, camera, Position, {0, 0, 5});
    ecs_set(world, camera, Camera, {0, 0, 0, 0, 0, 0, 45.0f});
    
    return camera;
}

// Helper function to create editor state entity
ecs_entity_t create_editor(ecs_world_t *world) {
    ecs_entity_t editor = ecs_new(world);
    
    ecs_add(world, editor, EditorTag);
    ecs_set(world, editor, EditorMode, {true, false, false});
    
    return editor;
}

int main(void) {
    ecs_world_t *world = ecs_init();
    
    // Register components
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Rotation);
    ECS_COMPONENT_DEFINE(world, Camera);
    ECS_COMPONENT_DEFINE(world, TextBuffer);
    ECS_COMPONENT_DEFINE(world, Phantom);
    ECS_COMPONENT_DEFINE(world, EditorMode);
    
    // Register tags
    ECS_TAG_DEFINE(world, PhantomTag);
    ECS_TAG_DEFINE(world, CameraTag);
    ECS_TAG_DEFINE(world, EditorTag);
    
    // Register systems
    ECS_SYSTEM(world, UpdatePhantomPositions, EcsOnUpdate, Position, Phantom, PhantomTag);
    ECS_SYSTEM(world, UpdateTextBuffers, EcsOnUpdate, TextBuffer, PhantomTag);
    ECS_SYSTEM(world, UpdateCamera, EcsOnUpdate, Camera, Position, CameraTag);
    
    printf("=== Pevi Editor ECS Demo ===\n\n");
    
    // Create editor state
    ecs_entity_t editor = create_editor(world);
    printf("Created editor entity\n");
    
    // Create camera
    ecs_entity_t camera = create_camera(world);
    printf("Created camera entity\n");
    
    // Create some phantom entities with different code content
    ecs_entity_t phantom1 = create_phantom(world, -2, 0, 0, 
        "#include <stdio.h>\nint main() {\n    printf(\"Hello World\");\n    return 0;\n}");
    printf("Created phantom 1 with C code\n");
    
    ecs_entity_t phantom2 = create_phantom(world, 2, 0, 0,
        "def fibonacci(n):\n    if n <= 1:\n        return n\n    return fibonacci(n-1) + fibonacci(n-2)");
    printf("Created phantom 2 with Python code\n");
    
    ecs_entity_t phantom3 = create_phantom(world, 0, 2, 0,
        "// Pevi Editor\n// 3D Spatial Code Organization\nclass Phantom {\n    constructor(content) {\n        this.content = content;\n    }\n}");
    printf("Created phantom 3 with JavaScript code\n");
    
    printf("\n=== Running simulation steps ===\n");
    
    // Run a few simulation steps
    for (int i = 0; i < 3; i++) {
        printf("\n--- Step %d ---\n", i + 1);
        ecs_progress(world, 0);
    }
    
    // Query all phantoms to show their state
    printf("\n=== Final Phantom States ===\n");
    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { ecs_id(Position) },
            { ecs_id(Phantom) },
            { ecs_id(TextBuffer) },
            { PhantomTag }
        }
    });
    
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        Position *pos = ecs_field(&it, Position, 1);
        Phantom *phantom = ecs_field(&it, Phantom, 2);
        TextBuffer *buffer = ecs_field(&it, TextBuffer, 3);
        
        for (int i = 0; i < it.count; i++) {
            printf("\nPhantom %d:\n", phantom[i].phantom_id);
            printf("  Position: (%.1f, %.1f, %.1f)\n", pos[i].x, pos[i].y, pos[i].z);
            printf("  Size: %.1fx%.1f\n", phantom[i].width, phantom[i].height);
            printf("  Focused: %s, Visible: %s\n", 
                   phantom[i].is_focused ? "Yes" : "No",
                   phantom[i].is_visible ? "Yes" : "No");
            printf("  Content length: %zu chars\n", buffer[i].length);
            if (buffer[i].content && buffer[i].length > 0) {
                printf("  First line: %.50s%s\n", 
                       buffer[i].content, 
                       buffer[i].length > 50 ? "..." : "");
            }
        }
    }
    
    // Cleanup
    ecs_query_fini(q);
    
    // Free text buffer memory (in a real implementation, this would be handled by component destructors)
    ecs_iter_t cleanup_it = ecs_query_iter(world, ecs_query(world, {
        .terms = {{ ecs_id(TextBuffer) }}
    }));
    while (ecs_query_next(&cleanup_it)) {
        TextBuffer *buffers = ecs_field(&cleanup_it, TextBuffer, 1);
        for (int i = 0; i < cleanup_it.count; i++) {
            if (buffers[i].content) {
                free(buffers[i].content);
            }
        }
    }
    
    printf("\n=== Demo Complete ===\n");
    
    ecs_fini(world);
    return 0;
}
