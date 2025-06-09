#include "spatial.h"

// Define tags and relationships
ECS_DECLARE(Visible);
ECS_DECLARE(Hidden);
ECS_DECLARE(NeedsReload);

ECS_DECLARE(References);
ECS_DECLARE(Contains);
ECS_DECLARE(Imports);
ECS_DECLARE(Includes);
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
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Rotation);
    ECS_COMPONENT_DEFINE(world, Scale);
    ECS_COMPONENT_DEFINE(world, Velocity);
    ECS_COMPONENT_DEFINE(world, EcsTransform);
    
    // Register phantom-specific components
    ECS_COMPONENT_DEFINE(world, TextContent);
    ECS_COMPONENT_DEFINE(world, FileReference);
    ECS_COMPONENT_DEFINE(world, Selected);
    ECS_COMPONENT_DEFINE(world, BoundingSphere);
    
    // Register camera and editor components
    ECS_COMPONENT_DEFINE(world, CameraController);
    ECS_COMPONENT_DEFINE(world, EditorState);
    
    // Register tags
    ECS_TAG_DEFINE(world, Visible);
    ECS_TAG_DEFINE(world, Hidden);
    ECS_TAG_DEFINE(world, NeedsReload);
    
    // Register custom relationships
    ECS_TAG_DEFINE(world, References);
    ECS_TAG_DEFINE(world, Contains);
    ECS_TAG_DEFINE(world, Imports);
    ECS_TAG_DEFINE(world, Includes);
    ECS_TAG_DEFINE(world, Targets);
    
    // Register pipeline phases
    ECS_TAG_DEFINE(world, InputPhase);
    ECS_TAG_DEFINE(world, TransformPhase);
    ECS_TAG_DEFINE(world, CullingPhase);
    ECS_TAG_DEFINE(world, RenderPhase);
    
    // Set up cleanup hooks for complex components
    // Note: Cleanup hooks are set separately since components are already registered
    ecs_set_hooks(world, TextContent, {
        .dtor = TextContent_dtor
    });
    
    ecs_set_hooks(world, FileReference, {
        .dtor = FileReference_dtor
    });
}
