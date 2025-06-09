#include "spatial.h"

// Define tags and relationships
ECS_DECLARE(Visible);
ECS_DECLARE(Hidden);
ECS_DECLARE(NeedsReload);

ECS_DECLARE(References);
ECS_DECLARE(Contains);
ECS_DECLARE(Imports);
ECS_DECLARE(Targets);

ECS_DECLARE(InputPhase);
ECS_DECLARE(TransformPhase);
ECS_DECLARE(CullingPhase);
ECS_DECLARE(RenderPhase);

// Component cleanup hooks
void TextContent_dtor(void *ptr, int32_t count, const ecs_type_info_t *ti) {
    TextContent *texts = (TextContent*)ptr;
    for (int i = 0; i < count; i++) {
        // Clean up any dynamically allocated text data if needed
        printf("Cleaning up text component: %s\n", texts[i].text);
    }
}

void FileReference_dtor(void *ptr, int32_t count, const ecs_type_info_t *ti) {
    FileReference *refs = (FileReference*)ptr;
    for (int i = 0; i < count; i++) {
        printf("Cleaning up file reference: %s\n", refs[i].filepath);
        // Close any open file handles, clean up watchers, etc.
    }
}

void RegisterSpatialComponents(ecs_world_t *world) {
    // Register atomic spatial components
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rotation);
    ECS_COMPONENT(world, Scale);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Transform);
    
    // Register phantom-specific components
    ECS_COMPONENT(world, TextContent);
    ECS_COMPONENT(world, FileReference);
    ECS_COMPONENT(world, Selected);
    ECS_COMPONENT(world, BoundingSphere);
    
    // Register camera and editor components
    ECS_COMPONENT(world, CameraController);
    ECS_COMPONENT(world, EditorState);
    
    // Register tags
    ECS_TAG(world, Visible);
    ECS_TAG(world, Hidden);
    ECS_TAG(world, NeedsReload);
    
    // Register custom relationships
    ECS_TAG(world, References);
    ECS_TAG(world, Contains);
    ECS_TAG(world, Imports);
    ECS_TAG(world, Targets);
    
    // Register pipeline phases
    ECS_TAG(world, InputPhase);
    ECS_TAG(world, TransformPhase);
    ECS_TAG(world, CullingPhase);
    ECS_TAG(world, RenderPhase);
    
    // Set up cleanup hooks for complex components
    ecs_component(world, {
        .entity = ecs_id(TextContent),
        .type.size = sizeof(TextContent),
        .type.alignment = ECS_ALIGNOF(TextContent),
        .type.hooks.dtor = TextContent_dtor
    });
    
    ecs_component(world, {
        .entity = ecs_id(FileReference),
        .type.size = sizeof(FileReference),
        .type.alignment = ECS_ALIGNOF(FileReference),
        .type.hooks.dtor = FileReference_dtor
    });
}
