# Entity Component System Architecture in Pevi

## Overview

Pevi uses Flecs, a high-performance Entity Component System (ECS) framework, to manage all objects in the 3D editor space. This document describes how Pevi leverages Flecs features and follows Flecs design best practices for optimal performance and maintainability.

## Core Concepts

### Entities
Entities in Flecs are unique identifiers that represent objects in the editor. They can be named for easy lookup and debugging, and support hierarchical organization through the built-in `ChildOf` relationship.

```pseudo
// Create entities
entity = world.create_entity()
named_entity = world.create_entity("MainEditor")

// Named entities can be looked up (O(1) performance)
found = world.lookup("MainEditor")

// Check if entity is still alive (important for stored handles)
if entity.is_alive() {
    // Safe to use
}
```

### Components - Atomic Design
Following Flecs best practices, Pevi uses **small, atomic components** rather than large composite ones. This improves cache performance, reduces refactoring, and increases reusability.

```pseudo
// GOOD: Atomic components
component Position {
    x: float
    y: float
    z: float
}

component Rotation {
    pitch: float
    yaw: float
    roll: float
}

component Scale {
    x: float
    y: float
    z: float
}

component TextContent {
    content: string
    length: size_t
    capacity: size_t
}

component FileInfo {
    filepath: string
    is_dirty: boolean
    last_modified: time_t
}

component PhantomSize {
    width: float
    height: float
}

component PhantomState {
    is_visible: boolean
    is_focused: boolean
    phantom_id: int
}

// AVOID: Large composite components
// component LargePhantom {
//     x, y, z, pitch, yaw, roll, scale_x, scale_y, scale_z: float
//     content, filepath: string
//     is_dirty, is_visible, is_focused: boolean
//     width, height: float
//     phantom_id: int
// }  // Don't do this - violates atomic design!
```

**Benefits of Atomic Components:**
- **Cache Performance**: Systems only load data they actually need
- **Minimal Refactoring**: Fewer reasons to change atomic components
- **Reusability**: Atomic components work across different projects
- **Query Efficiency**: Cached queries have minimal overhead for multiple components

### Tags
Tags are zero-sized components used for marking entities with specific states. Flecs optimizes tag storage and queries.

```pseudo
// Tag definitions
tag Selected
tag Hidden
tag Modified
tag Focused
tag Locked

// Using tags
entity.add(Selected)
entity.has(Selected)  // returns true
entity.remove(Selected)

// Tags are efficient for queries
query = world.query([Position, Selected])  // All selected entities with position
```

### Relationships
Flecs relationships are a powerful feature for modeling entity connections. The two most common are `ChildOf` (hierarchies) and `IsA` (prefabs/inheritance).

```pseudo
// Built-in relationships
relationship ChildOf      // Hierarchical parent-child
relationship IsA          // Prefab inheritance

// Custom relationships
relationship DependsOn    // Entity depends on another entity
relationship References   // Entity references another entity
relationship Contains     // For container-like structures

// Using relationships
entity.add_pair(ChildOf, parent_entity)
entity.add_pair(References, file_entity)
entity.has_pair(ChildOf, parent_entity)  // returns true

// Multiple instances of same relationship
phantom.add_pair(DependsOn, lib1)
phantom.add_pair(DependsOn, lib2)  // Can depend on multiple things
```

**When to Use Relationships:**
- You have a component with an entity handle and need reverse lookups
- You need multiple instances of the same component on an entity
- You're designing container structures (like inventories)
- You want to group entities by layers/cells with efficient lookup
- You have many similar components that duplicate code

#### Hierarchies with ChildOf

Flecs provides built-in hierarchy support with automatic cleanup and path-based lookups:

```pseudo
// Creating hierarchies
parent = world.create_entity("ProjectRoot")
child = world.create_entity("UIGroup")
child.add_pair(ChildOf, parent)

// Automatic cleanup - deleting parent deletes all children
world.delete_entity(parent)  // child is also deleted!

// Path-based lookups (O(1) performance with hashmap)
found = world.lookup("ProjectRoot.UIGroup")

// Hierarchical queries
query = world.query([Position, ChildOf(parent)])  // All children with Position
```

### Prefabs
Flecs prefabs are entity templates that provide default components and values. They're faster than manual initialization and help classify entity types.

```pseudo
// Create prefab
phantom_prefab = world.create_entity("PhantomPrefab")
phantom_prefab.set(Position, {0, 0, 0})
phantom_prefab.set(PhantomSize, {400, 300})
phantom_prefab.set(PhantomState, {true, false, 0})

// Create entities from prefab (fast!)
phantom1 = world.create_entity()
phantom1.add_pair(IsA, phantom_prefab)  // Inherits all prefab components

// Override specific values
phantom1.set(Position, {100, 50, 0})  // Override position, keep other defaults
```

### Systems
Systems contain logic that operates on entities with specific component combinations. Flecs systems follow single responsibility principle and use proper scheduling.

```pseudo
system UpdatePhantomPositions {
    // Query with proper access annotations
    query: [in] Position, [inout] PhantomState
    phase: OnUpdate  // Explicit phase assignment
    
    update(delta_time) {
        for each entity matching query {
            position = entity.get(Position)      // Read-only access
            state = entity.get_mut(PhantomState) // Mutable access
            
            if state.is_visible {
                // Process phantom position
                update_phantom_display(position, state)
            }
        }
    }
}
```

**System Design Principles:**
- **Single Responsibility**: Each system does one thing well
- **Small Systems**: Easier to isolate, optimize, and reuse
- **Proper Phases**: Use Flecs phase system for ordering
- **Access Annotations**: Mark components as `in`, `inout`, or `out`

## Pevi Entity Architecture

### Prefabs for Entity Types

Following Flecs best practices, Pevi uses prefabs to define entity templates:

```pseudo
// Phantom prefab
phantom_prefab = world.create_entity("PhantomPrefab")
phantom_prefab.set(Position, {0, 0, 0})
phantom_prefab.set(Rotation, {0, 0, 0})
phantom_prefab.set(PhantomSize, {400, 300})
phantom_prefab.set(PhantomState, {true, false, 0})
phantom_prefab.set(TextContent, {"", 0, 1024})

// Camera prefab
camera_prefab = world.create_entity("CameraPrefab")
camera_prefab.set(Position, {0, 0, 5})
camera_prefab.set(Rotation, {0, 0, 0})
camera_prefab.set(CameraSettings, {45.0, 0.1, 1000.0})

// Editor prefab (singleton)
editor_prefab = world.create_entity("EditorPrefab")
editor_prefab.set(EditorMode, {true, false, false})
editor_prefab.set(EditorConfig, {"default", 12.0})
```

### Core Entity Types

#### Editor Entity (Singleton)
```pseudo
entity Editor {
    components: [
        EditorMode { 
            is_navigation: boolean
            is_edit: boolean
            is_command: boolean
        },
        EditorConfig {
            theme: string
            font_size: float
        }
    ]
}
```

#### Camera Entity
```pseudo
entity Camera {
    components: [
        Position { x, y, z },
        Rotation { pitch, yaw, roll },
        CameraSettings {
            fov: float
            near_plane: float
            far_plane: float
        }
    ]
}
```

#### Phantom Entities (Atomic Components)
```pseudo
entity Phantom {
    components: [
        Position { x, y, z },
        Rotation { pitch, yaw, roll },
        PhantomSize { width, height },
        PhantomState { is_visible, is_focused, phantom_id },
        TextContent { content, length, capacity },
        FileInfo { filepath, is_dirty, last_modified }  // Optional
    ],
    tags: [
        Selected,    // Currently selected by user
        Modified,    // Has unsaved changes
        Locked,      // Cannot be edited
        Hidden       // Temporarily hidden from view
    ]
}
```

### Relationships in Pevi

Pevi uses pairs to model various relationships between entities:

```pseudo
// Parent-child relationships for grouping phantoms
phantom1.add_pair(ChildOf, group_entity)
phantom2.add_pair(ChildOf, group_entity)

// Phantom references a file
phantom.add_pair(ReferencesFile, file_entity)

// Camera targets a phantom
camera.add_pair(Targeting, phantom)

// Phantom depends on another phantom (e.g., for imports)
phantom.add_pair(DependsOn, imported_phantom)

// Multiple phantoms can reference the same file
phantom1.add_pair(ReferencesFile, main_file)
phantom2.add_pair(ReferencesFile, main_file)  // Different view of same file

// Query examples with relationships
query = world.query([Phantom, ChildOf(group_entity)])  // All phantoms in a group
query = world.query([ReferencesFile(file_entity)])     // All phantoms referencing a file

// Leveraging built-in hierarchy features
group = world.create_entity("CodeGroup")
phantom1 = world.create_entity_with_pair(ChildOf, group)
phantom2 = world.create_entity_with_pair(ChildOf, group)

// Deleting the group automatically cleans up all phantoms in it
world.delete_entity(group)  // phantom1 and phantom2 are also deleted
```

Benefits of using pairs for relationships:
- **Flexible Hierarchies**: Phantoms can be organized in groups and subgroups
- **Automatic Cleanup**: Deleting a parent group automatically removes all child phantoms
- **Multiple Views**: Multiple phantoms can reference the same file, showing different parts
- **Dependency Tracking**: Track which phantoms depend on others
- **Efficient Queries**: Query for all entities with specific relationships
- **Path-based Access**: Navigate hierarchies using path names (e.g., "UI.Components.Button")

## System Architecture with Flecs Phases

### Flecs Pipeline Phases

Pevi uses Flecs' built-in phase system for proper system ordering:

```pseudo
// OnLoad: Load input data
system LoadInput {
    query: [InputState]
    phase: OnLoad
    
    update() {
        // Load keyboard/mouse input
        load_input_events()
    }
}

// PostLoad: Process input into high-level actions
system ProcessInput {
    query: [in] InputState, [inout] EditorMode
    phase: PostLoad
    
    update() {
        // Convert raw input to editor actions
        process_navigation_input()
        process_edit_input()
    }
}

// PreUpdate: Prepare frame, cleanup
system PrepareFrame {
    query: [PhantomState]
    phase: PreUpdate
    
    update() {
        // Clear previous frame state
        clear_selection_highlights()
    }
}

// OnUpdate: Main game logic (default phase)
system UpdatePhantoms {
    query: [inout] PhantomState, [in] TextContent, [in] FileInfo
    phase: OnUpdate  // Default phase
    
    update() {
        for each phantom {
            // Update phantom logic
            update_phantom_state(phantom)
        }
    }
}

system ControlCamera {
    query: [inout] Position, [inout] Rotation, [in] CameraSettings
    phase: OnUpdate
    
    update(delta_time) {
        // Camera movement logic
        apply_camera_controls(delta_time)
    }
}

// OnValidate: Validate state after updates
system ValidatePhantomPositions {
    query: [inout] Position, [in] PhantomSize
    phase: OnValidate
    
    update() {
        // Prevent phantom overlap, validate bounds
        resolve_phantom_collisions()
    }
}

// PostUpdate: Apply corrections
system ApplyPhantomCorrections {
    query: [inout] Position
    phase: PostUpdate
    
    update() {
        // Apply position corrections from validation
        apply_position_corrections()
    }
}

// PreStore: Prepare for rendering
system PrepareRendering {
    query: [in] Position, [in] Rotation, [out] TransformMatrix
    phase: PreStore
    
    update() {
        // Calculate transform matrices
        calculate_world_matrices()
    }
}

// OnStore: Render frame
system RenderPhantoms {
    query: [in] Position, [in] PhantomState, [in] TextContent, [in] TransformMatrix
    phase: OnStore
    
    render() {
        // Render all visible phantoms
        render_phantom_text()
    }
}
```

### Event System Integration

Events are handled through queries and systems that respond to state changes.

```pseudo
// Event types
event PhantomSelected { entity: EntityID }
event BufferModified { entity: EntityID, change: TextChange }
event FileOpened { filepath: string, content: string }

// Event handling system
system HandlePhantomSelection {
    event_query: [PhantomSelected]
    
    on_event(event: PhantomSelected) {
        // Clear previous focus
        for each phantom with Phantom component {
            phantom.is_focused = false
        }
        
        // Set new focus
        selected = world.get_entity(event.entity)
        phantom = selected.get(Phantom)
        phantom.is_focused = true
        
        // Update editor mode
        editor_mode = get_singleton(EditorMode)
        editor_mode.is_edit = true
    }
}
```

## Query Patterns with Flecs

### Query Types

Flecs has two query types - use the right one for your use case:

```pseudo
// Cached queries: expensive to create, cheap to iterate
// Use for systems and frequently-used queries
cached_query = world.create_cached_query([Position, PhantomState])

// Uncached queries: fast to create, more expensive to iterate  
// Use for ad-hoc queries
uncached_query = world.create_query([Position, PhantomState])
```

### Access Annotations

Always annotate component access for optimal performance:

```pseudo
// Read-only access (enables optimizations)
query = world.query([in] Position, [in] PhantomState)

// Read-write access (default if not specified)
query = world.query([inout] Position, [in] PhantomState)

// Write-only access (for initialization)
query = world.query([out] Position, [in] PhantomPrefab)

// Mixed access patterns
query = world.query([in] Position, [inout] PhantomState, [out] TransformMatrix)
```

### Basic Queries
```pseudo
// All entities with Position (read-only)
query = world.query([in] Position)

// All entities with Position AND PhantomState
query = world.query([in] Position, [in] PhantomState)

// All entities with Position but NOT Hidden tag
query = world.query([in] Position, !Hidden)

// All selected phantoms
query = world.query([in] Position, Selected)
```

### Relationship Queries
```pseudo
// All phantoms that are children of a specific group
query = world.query([in] PhantomState, ChildOf(group_entity))

// All top-level phantoms (no parent)
query = world.query([in] PhantomState, !ChildOf(*))

// All entities that reference a specific file
query = world.query([References(file_entity)])

// All phantoms that depend on a specific phantom
query = world.query([in] PhantomState, DependsOn(source_phantom))
```

### Singleton Access
```pseudo
// Access singleton components (no query needed)
editor_mode = world.get_singleton(EditorMode)
camera_settings = world.get_singleton(CameraSettings)

// Modify singleton
world.set_singleton(EditorMode, {false, true, false})
```

## Modules for Pevi

Following Flecs best practices, Pevi organizes functionality into reusable modules:

### Component Modules
```pseudo
// components.phantom - Define phantom-related components
module ComponentsPhantom {
    export Position, Rotation, PhantomSize, PhantomState
    export TextContent, FileInfo
    export Selected, Hidden, Modified tags
}

// components.camera - Define camera components  
module ComponentsCamera {
    export CameraSettings, CameraTarget
}

// components.editor - Define editor components
module ComponentsEditor {
    export EditorMode, EditorConfig
}
```

### System Modules
```pseudo
// systems.phantom - Implement phantom behavior
module SystemsPhantom {
    import ComponentsPhantom
    
    export UpdatePhantoms, ValidatePhantomPositions
    export RenderPhantoms, HandlePhantomSelection
}

// systems.camera - Implement camera behavior
module SystemsCamera {
    import ComponentsCamera, ComponentsEditor
    
    export ControlCamera, UpdateCameraTarget
}

// systems.input - Handle input processing
module SystemsInput {
    import ComponentsEditor
    
    export LoadInput, ProcessInput
}
```

### Module Benefits
- **Reusability**: Well-designed modules work across projects
- **Feature Swapping**: Replace `systems.phantom` with different implementation
- **No Performance Penalty**: Unused systems remain dormant
- **Dependency Management**: Modules handle their own dependencies

## Benefits for Pevi

1. **Atomic Components**: Better cache performance and reduced refactoring
2. **Prefabs**: Fast entity initialization with default values  
3. **Proper Phases**: Automatic system ordering without manual dependencies
4. **Query Optimization**: Cached queries and access annotations for performance
5. **Modular Design**: Reusable modules enable feature swapping
6. **Relationships**: Powerful hierarchies and entity connections
7. **Flecs Features**: Built-in naming, path lookup, and automatic cleanup

## Example: Creating Phantoms with Flecs Best Practices

```pseudo
// Setup prefabs first (done once at startup)
function setup_phantom_prefabs(world) {
    phantom_prefab = world.create_entity("PhantomPrefab")
    phantom_prefab.set(Position, {0, 0, 0})
    phantom_prefab.set(Rotation, {0, 0, 0})
    phantom_prefab.set(PhantomSize, {400, 300})
    phantom_prefab.set(PhantomState, {true, false, generate_id()})
    phantom_prefab.set(TextContent, {"", 0, 1024})
    
    return phantom_prefab
}

// Create phantom using prefab (fast!)
function create_phantom(world, prefab, x, y, z, content, parent = null, file = null) {
    // Create from prefab
    phantom = world.create_entity()
    phantom.add_pair(IsA, prefab)  // Inherit all prefab components
    
    // Override specific values (atomic components)
    phantom.set(Position, {x, y, z})
    if content != "" {
        phantom.set(TextContent, {content, content.length, content.length * 2})
        phantom.add(Modified)  // Tag for modified content
    }
    
    // Add file info if provided
    if file != null {
        phantom.set(FileInfo, {file.path, false, file.last_modified})
        phantom.add_pair(References, file)
    }
    
    // Add to hierarchy
    if parent != null {
        phantom.add_pair(ChildOf, parent)
    }
    
    return phantom
}

// Efficient selection using queries
function select_phantom(world, phantom) {
    // Use cached query to clear all selections (efficient)
    selected_query = world.get_cached_query([Selected])
    for each entity in selected_query {
        entity.remove(Selected)
    }
    
    // Select new phantom
    phantom.add(Selected)
    
    // Update state atomically
    state = phantom.get_mut(PhantomState)
    state.is_focused = true
}

// Create hierarchical organization
function organize_project_phantoms(world, phantom_prefab) {
    // Create project hierarchy using ChildOf
    project = world.create_entity("MyProject")
    project.set(Position, {0, 0, 0})
    
    ui_group = world.create_entity("UI")
    ui_group.add_pair(ChildOf, project)
    ui_group.set(Position, {100, 0, 0})
    
    backend_group = world.create_entity("Backend")  
    backend_group.add_pair(ChildOf, project)
    backend_group.set(Position, {-100, 0, 0})
    
    // Create phantoms in groups (using prefab)
    button_phantom = create_phantom(world, phantom_prefab, 
                                  120, 0, 0, "button code", ui_group)
    api_phantom = create_phantom(world, phantom_prefab,
                               -120, 0, 0, "api code", backend_group)
    
    // Path-based lookup (O(1) performance)
    found_ui = world.lookup("MyProject.UI")
    
    // Automatic cleanup - deleting project removes everything
    // world.delete_entity(project)  // Cleans up all children!
    
    return project
}

// Query patterns for phantom management
function phantom_queries(world) {
    // Cached queries for frequent use
    visible_phantoms = world.create_cached_query([in] Position, [in] PhantomState, !Hidden)
    selected_phantoms = world.create_cached_query([inout] PhantomState, Selected)
    dirty_files = world.create_cached_query([in] FileInfo, Modified)
    
    // Relationship queries
    ui_phantoms = world.create_cached_query([in] PhantomState, ChildOf(ui_group)])
    
    return {visible_phantoms, selected_phantoms, dirty_files, ui_phantoms}
}
```

This Flecs-based architecture provides optimal performance through atomic components, efficient prefab-based creation, proper query design, and leverages Flecs' powerful relationship and hierarchy features for Pevi's 3D code editing approach.
