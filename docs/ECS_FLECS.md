# Entity Component System Architecture in Pevi

## Overview

Pevi uses Flecs v4, a high-performance Entity Component System (ECS) framework, to manage all objects in the 3D spatial code editor. This architecture follows the latest Flecs best practices for optimal performance, including atomic component design, cached query optimization, and sophisticated relationship patterns.

## Core Flecs Principles

### Data-Oriented Architecture
Flecs uses archetype-based storage where entities with identical component sets are grouped together for optimal cache performance. This provides structure-of-arrays access that enables automatic compiler vectorization and minimizes cache misses.

### Everything is an Entity
In Flecs, systems, queries, and components are all entities, providing unified handling throughout the application. This enables powerful meta-programming capabilities and consistent API usage.

## Atomic Component Design

Following Flecs best practices, Pevi uses **small, atomic components** rather than large composite ones. This design provides:

- **Cache Performance**: Systems only load data they actually need
- **Vectorization**: Enables automatic compiler optimizations
- **Minimal Refactoring**: Atomic components have fewer reasons to change
- **Reusability**: Components work across different projects
- **Query Efficiency**: Cached queries have minimal overhead for multiple components

### Core Spatial Components

```c
// Spatial transformation components (atomic design)
typedef struct {
    float x, y, z;
} Position;

typedef struct {
    float x, y, z, w;  // Quaternion
} Rotation;

typedef struct {
    float x, y, z;
} Scale;

typedef struct {
    float x, y, z;
} Velocity;

// Phantom-specific components
typedef struct {
    float width, height;
} PhantomSize;

typedef struct {
    uint32_t id;
} PhantomId;

typedef struct {
    char* content;
    size_t length;
    size_t capacity;
} TextContent;

typedef struct {
    char* filepath;
    bool is_dirty;
    time_t last_modified;
} FileInfo;

// Camera components
typedef struct {
    float fov;
    float near_plane;
    float far_plane;
} CameraSettings;

typedef struct {
    ecs_entity_t target;
} CameraTarget;

// Editor state components
typedef struct {
    char* theme;
    float font_size;
    float line_height;
} EditorConfig;
```

### Tags for State Management

Tags are zero-sized components optimized by Flecs for marking entity states:

```c
// Visibility states (mutually exclusive)
ECS_TAG_DECLARE(Visible);
ECS_TAG_DECLARE(Hidden);

// Focus states (mutually exclusive)
ECS_TAG_DECLARE(Focused);
ECS_TAG_DECLARE(Unfocused);

// Selection and modification states
ECS_TAG_DECLARE(Selected);
ECS_TAG_DECLARE(Modified);
ECS_TAG_DECLARE(Locked);

// Editor modes (mutually exclusive)
ECS_TAG_DECLARE(NavigationMode);
ECS_TAG_DECLARE(EditMode);
ECS_TAG_DECLARE(CommandMode);

// Phantom type classifications
ECS_TAG_DECLARE(IsPhantom);
ECS_TAG_DECLARE(IsCamera);
ECS_TAG_DECLARE(IsEditor);
```

## Relationship System

Pevi leverages Flecs' powerful relationship system for modeling entity connections with only 5-10% performance overhead compared to regular components.

### Built-in Relationships

```c
// Flecs built-in relationships
// ChildOf - hierarchical parent-child (automatic cleanup)
// IsA - prefab inheritance
// DependsOn - custom dependency modeling

// Custom relationships for Pevi
ECS_DECLARE(References);    // Entity references another entity
ECS_DECLARE(Contains);      // Container-like structures
ECS_DECLARE(Imports);       // Code import dependencies
ECS_DECLARE(Targets);       // Camera targeting
```

### Hierarchy Management with ChildOf

Flecs provides automatic cleanup and path-based lookups with O(1) performance:

```c
// Create project hierarchy
ecs_entity_t project = ecs_new_entity(world, "ProjectRoot");
ecs_entity_t ui_group = ecs_new_entity(world, "UI");
ecs_add_pair(world, ui_group, EcsChildOf, project);

// Automatic cleanup - deleting project deletes all children
ecs_delete(world, project);  // ui_group is also deleted

// Path-based lookups with hashmap optimization
ecs_entity_t found = ecs_lookup(world, "ProjectRoot.UI");
```

## Prefab System for Entity Templates

Prefabs provide template-based entity creation with component inheritance through IsA relationships:

```c
// Create phantom prefab
ecs_entity_t phantom_prefab = ecs_new_entity(world, "PhantomPrefab");
ecs_set(world, phantom_prefab, Position, {0, 0, 0});
ecs_set(world, phantom_prefab, Rotation, {0, 0, 0, 1});
ecs_set(world, phantom_prefab, Scale, {1, 1, 1});
ecs_set(world, phantom_prefab, PhantomSize, {400, 300});
ecs_add(world, phantom_prefab, Visible);
ecs_add(world, phantom_prefab, Unfocused);
ecs_add(world, phantom_prefab, IsPhantom);

// Fast entity creation from prefab
ecs_entity_t phantom = ecs_new(world, 0);
ecs_add_pair(world, phantom, EcsIsA, phantom_prefab);  // Inherits all prefab components

// Override specific values
ecs_set(world, phantom, Position, {100, 50, 0});
ecs_remove(world, phantom, Unfocused);
ecs_add(world, phantom, Focused);
```

## System Architecture with Pipeline Phases

Pevi uses Flecs' eight-phase pipeline for automatic dependency resolution and deterministic execution:

### Phase Organization

```c
// OnLoad: Input and external data loading
ECS_SYSTEM(world, LoadInputSystem, EcsOnLoad, InputState);

// PostLoad: Process input into high-level actions
ECS_SYSTEM(world, ProcessInputSystem, EcsPostLoad, [in] InputState, [inout] EditorConfig);

// PreUpdate: Frame preparation and cleanup
ECS_SYSTEM(world, PrepareFrameSystem, EcsPreUpdate, [out] FrameData);

// OnUpdate: Main logic (default phase)
ECS_SYSTEM(world, UpdatePhantomsSystem, EcsOnUpdate,
    [in] Position, [in] TextContent, [inout] PhantomSize, Visible, IsPhantom);

ECS_SYSTEM(world, CameraControlSystem, EcsOnUpdate,
    [inout] Position, [inout] Rotation, [in] CameraSettings, IsCamera);

// OnValidate: State validation after updates
ECS_SYSTEM(world, ValidatePhantomPositionsSystem, EcsOnValidate,
    [inout] Position, [in] PhantomSize, Visible, IsPhantom);

// PostUpdate: Apply corrections
ECS_SYSTEM(world, ApplyCorrectionsSystem, EcsPostUpdate, [inout] Position);

// PreStore: Prepare rendering data
ECS_SYSTEM(world, CalculateTransformsSystem, EcsPreStore,
    [in] Position, [in] Rotation, [in] Scale, [out] TransformMatrix);

// OnStore: Rendering
ECS_SYSTEM(world, RenderPhantomsSystem, EcsOnStore,
    [in] Position, [in] TextContent, [in] TransformMatrix, Visible, IsPhantom);
```

### Single Responsibility Systems

Each system has one clear purpose for maximum reusability and maintainability:

```c
// Example: Focused phantom management system
void FocusManagementSystem(ecs_iter_t *it) {
    // Clear previous focus using cached query
    ecs_query_t *focused_query = ecs_query_new(it->world, "[inout] PhantomId, Focused");
    ecs_iter_t focused_it = ecs_query_iter(it->world, focused_query);

    while (ecs_query_next(&focused_it)) {
        for (int i = 0; i < focused_it.count; i++) {
            ecs_entity_t entity = focused_it.entities[i];
            ecs_remove(it->world, entity, Focused);
            ecs_add(it->world, entity, Unfocused);
        }
    }

    // Apply new focus to selected phantom
    ecs_query_t *selected_query = ecs_query_new(it->world, "[inout] PhantomId, Selected");
    ecs_iter_t selected_it = ecs_query_iter(it->world, selected_query);

    while (ecs_query_next(&selected_it)) {
        for (int i = 0; i < selected_it.count; i++) {
            ecs_entity_t entity = selected_it.entities[i];
            ecs_remove(it->world, entity, Unfocused);
            ecs_add(it->world, entity, Focused);
        }
    }
}
```

## Query Optimization Patterns

### Cached vs Uncached Queries

Flecs provides two query types with different performance characteristics:

```c
// Cached queries: expensive to create, cheap to iterate
// Use for systems and frequently-used queries
ecs_query_t *visible_phantoms = ecs_query_new(world,
    "[in] Position, [in] PhantomId, Visible, IsPhantom");

// Uncached queries: fast to create, more expensive to iterate
// Use for ad-hoc queries
ecs_filter_t *temp_filter = ecs_filter_new(world,
    "[in] Position, Selected");
```

### Access Annotations for Performance

Always annotate component access for optimal performance:

```c
// Read-only access (enables optimizations)
ECS_SYSTEM(world, ReadOnlySystem, EcsOnUpdate, [in] Position, [in] Velocity);

// Read-write access
ECS_SYSTEM(world, UpdateSystem, EcsOnUpdate, [inout] Position, [in] Velocity);

// Write-only access (for initialization)
ECS_SYSTEM(world, InitSystem, EcsOnUpdate, [out] Position, [in] PhantomPrefab);

// Mixed access patterns
ECS_SYSTEM(world, ComplexSystem, EcsOnUpdate,
    [in] Position, [inout] Velocity, [out] TransformMatrix);
```

## Event-Driven Architecture with Observers

Observers provide reactive programming capabilities for responding to ECS events:

```c
// Observer for phantom selection changes
ECS_OBSERVER(world, OnPhantomSelected, EcsOnAdd, Selected, IsPhantom);

void OnPhantomSelected(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i++) {
        ecs_entity_t phantom = it->entities[i];

        // Trigger editor mode change
        ecs_entity_t editor = ecs_lookup(it->world, "Editor");
        ecs_remove(it->world, editor, NavigationMode);
        ecs_add(it->world, editor, EditMode);

        // Update camera target
        ecs_entity_t camera = ecs_lookup(it->world, "MainCamera");
        ecs_set(it->world, camera, CameraTarget, {phantom});
    }
}

// Observer for file modifications
ECS_OBSERVER(world, OnFileModified, EcsOnSet, TextContent, IsPhantom);

void OnFileModified(ecs_iter_t *it) {
    TextContent *content = ecs_field(it, TextContent, 1);

    for (int i = 0; i < it->count; i++) {
        ecs_entity_t phantom = it->entities[i];

        // Mark as modified
        ecs_add(it->world, phantom, Modified);

        // Update file info if exists
        if (ecs_has(it->world, phantom, FileInfo)) {
            FileInfo *file_info = ecs_get_mut(it->world, phantom, FileInfo);
            file_info->is_dirty = true;
        }
    }
}
```

## Spatial Relationship Modeling

Pevi uses relationships for sophisticated spatial organization:

```c
// Create spatial hierarchy
ecs_entity_t workspace = ecs_new_entity(world, "Workspace");
ecs_entity_t code_layer = ecs_new_entity(world, "CodeLayer");
ecs_entity_t ui_layer = ecs_new_entity(world, "UILayer");

ecs_add_pair(world, code_layer, EcsChildOf, workspace);
ecs_add_pair(world, ui_layer, EcsChildOf, workspace);

// Group related phantoms
ecs_entity_t main_file = create_phantom_from_file(world, "main.c");
ecs_entity_t header_file = create_phantom_from_file(world, "main.h");

ecs_add_pair(world, main_file, EcsChildOf, code_layer);
ecs_add_pair(world, header_file, EcsChildOf, code_layer);
ecs_add_pair(world, header_file, References, main_file);

// Query all phantoms in a layer
ecs_query_t *code_phantoms = ecs_query_new(world,
    "[in] Position, [in] TextContent, (ChildOf, code_layer), IsPhantom");
```

## Singleton Management

Flecs implements singletons by adding components to themselves:

```c
// Editor singleton setup
ecs_entity_t editor_config = ecs_new_entity(world, "EditorConfig");
ecs_add(world, editor_config, EditorConfig);
ecs_set(world, editor_config, EditorConfig, {"dark_theme", 12.0f, 1.2f});
ecs_add(world, editor_config, NavigationMode);

// Access singleton components
EditorConfig *config = ecs_get_mut(world, editor_config, EditorConfig);
bool is_navigation = ecs_has(world, editor_config, NavigationMode);

// Mode switching
ecs_remove(world, editor_config, NavigationMode);
ecs_add(world, editor_config, EditMode);
```

## Module Organization

Organize functionality into reusable modules for better maintainability:

```c
// components_spatial.h
typedef struct {
    ECS_DECLARE(Position);
    ECS_DECLARE(Rotation);
    ECS_DECLARE(Scale);
    ECS_DECLARE(Velocity);
} ComponentsSpatialModule;

void ComponentsSpatialImport(ecs_world_t *world) {
    ECS_MODULE(world, ComponentsSpatial);
    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Rotation);
    ECS_COMPONENT_DEFINE(world, Scale);
    ECS_COMPONENT_DEFINE(world, Velocity);
}

// systems_phantom.h
typedef struct {
    ECS_DECLARE(UpdatePhantomsSystem);
    ECS_DECLARE(ValidatePhantomPositionsSystem);
    ECS_DECLARE(RenderPhantomsSystem);
} SystemsPhantomModule;

void SystemsPhantomImport(ecs_world_t *world) {
    ECS_MODULE(world, SystemsPhantom);
    ECS_IMPORT(world, ComponentsSpatial);
    ECS_IMPORT(world, ComponentsPhantom);

    ECS_SYSTEM_DEFINE(world, UpdatePhantomsSystem, EcsOnUpdate,
        [in] Position, [in] TextContent, [inout] PhantomSize, Visible, IsPhantom);

    ECS_SYSTEM_DEFINE(world, ValidatePhantomPositionsSystem, EcsOnValidate,
        [inout] Position, [in] PhantomSize, Visible, IsPhantom);

    ECS_SYSTEM_DEFINE(world, RenderPhantomsSystem, EcsOnStore,
        [in] Position, [in] TextContent, [in] TransformMatrix, Visible, IsPhantom);
}
```

## Performance Optimization Strategies

### Memory Layout Optimization
- **Design for vectorization**: Use atomic components with consistent data types
- **Minimize archetype fragmentation**: Use consistent component patterns
- **Leverage change detection**: Use Flecs' built-in change tracking for incremental updates

### Query Performance
- **Pre-create cached queries**: Avoid repeated query creation in systems
- **Use proper access annotations**: Enable compiler optimizations
- **Batch similar operations**: Group related entities for optimal cache utilization

### Relationship Optimization
- **Use relationships judiciously**: Only when you need multiple instances or reverse lookups
- **Prefer components for single instances**: Relationships have 5-10% overhead
- **Cache relationship queries**: Relationship queries benefit significantly from caching

## Critical Anti-Patterns to Avoid

### Component Design Anti-Patterns
```c
// AVOID: God components that bundle unrelated data
typedef struct {
    float x, y, z;           // Position data
    float r, g, b, a;        // Color data
    char* content;           // Text data
    bool is_visible;         // State data
    float font_size;         // UI data
} BadPhantomComponent;     // DON'T DO THIS!

// PREFER: Atomic components
typedef struct { float x, y, z; } Position;
typedef struct { float r, g, b, a; } Color;
typedef struct { char* content; size_t length; } TextContent;
// + separate tags for state
```

### Query Anti-Patterns
```c
// AVOID: Creating queries repeatedly in systems
void BadSystem(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i++) {
        // DON'T: Create query every iteration
        ecs_query_t *query = ecs_query_new(it->world, "[in] Position");
        // ... use query
        ecs_query_fini(query);
    }
}

// PREFER: Pre-created cached queries
static ecs_query_t *cached_query = NULL;

void GoodSystem(ecs_iter_t *it) {
    if (!cached_query) {
        cached_query = ecs_query_new(it->world, "[in] Position");
    }
    // ... use cached_query
}
```

### Entity Handle Anti-Patterns
```c
// AVOID: Using entity handles without validation
void BadEntityUsage(ecs_world_t *world, ecs_entity_t entity) {
    Position *pos = ecs_get_mut(world, entity, Position);  // Might be null!
    pos->x = 100;  // Crash if entity was deleted
}

// PREFER: Always validate entity handles
void GoodEntityUsage(ecs_world_t *world, ecs_entity_t entity) {
    if (ecs_is_alive(world, entity)) {
        Position *pos = ecs_get_mut(world, entity, Position);
        if (pos) {
            pos->x = 100;
        }
    }
}
```

## Integration with Raylib Renderer

Separate rendering from game logic using appropriate pipeline phases:

```c
// Rendering system in OnStore phase
void RenderPhantomsSystem(ecs_iter_t *it) {
    Position *positions = ecs_field(it, Position, 1);
    TextContent *contents = ecs_field(it, TextContent, 2);
    TransformMatrix *transforms = ecs_field(it, TransformMatrix, 3);

    for (int i = 0; i < it->count; i++) {
        // Use Raylib for actual rendering
        Matrix transform = transforms[i].matrix;

        BeginMode3D(camera);
        rlPushMatrix();
        rlMultMatrixf(&transform.m0);

        // Render phantom text as billboard
        DrawText3D(contents[i].content,
                  (Vector3){0, 0, 0},
                  12.0f,
                  WHITE);

        rlPopMatrix();
        EndMode3D();
    }
}
```

## Practical Example: Complete Phantom Management

```c
// Complete phantom creation with best practices
ecs_entity_t create_phantom_from_file(ecs_world_t *world, const char *filepath) {
    // Create entity from prefab
    ecs_entity_t phantom_prefab = ecs_lookup(world, "PhantomPrefab");
    ecs_entity_t phantom = ecs_new(world, 0);
    ecs_add_pair(world, phantom, EcsIsA, phantom_prefab);

    // Load file content
    char *content = load_file_content(filepath);
    if (content) {
        ecs_set(world, phantom, TextContent, {content, strlen(content), strlen(content) * 2});
        ecs_set(world, phantom, FileInfo, {strdup(filepath), false, get_file_mtime(filepath)});
    }

    // Add to spatial hierarchy
    ecs_entity_t code_layer = ecs_lookup(world, "Workspace.CodeLayer");
    if (code_layer) {
        ecs_add_pair(world, phantom, EcsChildOf, code_layer);
    }

    // Generate unique position
    Position pos = generate_phantom_position(world, code_layer);
    ecs_set(world, phantom, Position, pos);

    return phantom;
}

// Efficient phantom selection with cached queries
void select_phantom(ecs_world_t *world, ecs_entity_t phantom) {
    static ecs_query_t *selected_query = NULL;

    if (!selected_query) {
        selected_query = ecs_query_new(world, "[inout] Selected, IsPhantom");
    }

    // Clear all selections
    ecs_iter_t it = ecs_query_iter(world, selected_query);
    while (ecs_query_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            ecs_remove(world, it.entities[i], Selected);
        }
    }

    // Select new phantom
    if (ecs_has(world, phantom, IsPhantom)) {
        ecs_add(world, phantom, Selected);
        ecs_remove(world, phantom, Unfocused);
        ecs_add(world, phantom, Focused);
    }
}
```

## Conclusion

This Flecs-based ECS architecture provides Pevi with optimal performance through atomic component design, cached query optimization, sophisticated relationship modeling, and reactive observer patterns. The framework's archetype-based storage, automatic vectorization capabilities, and powerful relationship system create a robust foundation for complex 3D spatial applications while maintaining clean, maintainable code architecture.

By following these established patterns and avoiding common anti-patterns, Pevi achieves excellent performance characteristics suitable for real-time 3D code editing with responsive user interactions and efficient spatial organization of code phantoms.
