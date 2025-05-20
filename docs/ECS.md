# Proposal: Integrating Flecs for Entity Component System (ECS) in Pevi

## 1. Introduction and Goal

This document proposes the integration of **Flecs**, a high-performance Entity Component System (ECS), into the Pevi project. The primary goal is to establish a robust, data-oriented foundation for managing "phantoms" (3D code objects), their diverse properties, and associated behaviors. Adopting Flecs will enhance modularity, performance, and future extensibility of Pevi's core architecture.

Pevi's concept of spatially arranged code "phantoms" naturally aligns with an ECS paradigm, where phantoms are entities, their characteristics (position, text content, visual style, interaction state) are components, and their behaviors (rendering, interaction, saving) are implemented by systems.

## 2. Why Flecs for Pevi?

Flecs offers several advantages that are well-suited for Pevi's requirements:

*   **Data-Oriented Design:** Promotes efficient memory layout and access patterns, crucial for performance as the number of phantoms and their complexity grows.
*   **Modular and Decoupled Systems:** Logic is encapsulated in systems that operate on specific combinations of components. This leads to cleaner, more maintainable, and testable code.
*   **Dynamic Composition:** Phantoms can easily acquire or lose characteristics and behaviors by adding or removing components at runtime (e.g., a phantom becoming "editable" or "error-highlighted").
*   **High Performance:** Flecs is designed for speed, efficiently managing large numbers of entities and components, and providing fast querying.
*   **Powerful Querying:** Offers a rich query language to find entities based on their components, essential for systems to identify which phantoms to process.
*   **Extensibility:** Simplifies the addition of new types of phantoms, properties, or global systems in the future.
*   **Alignment with Architecture:** Directly supports the "Component Architecture" principle outlined in `ARCHITECTURE.md`.

## 3. Architectural Integration

Flecs will be integrated into Pevi's existing layered architecture as follows:

### 3.1. Core Layer Integration

*   **Flecs World Management:**
    *   A global Flecs world (`ecs_world_t*`) will be initialized and managed within the Core layer.
    *   **New Files:**
        *   `include/pevi/core/ecs_manager.h`: Public header defining the interface for accessing the Flecs world (e.g., `pevi_ecs_get_world()`) and basic ECS utility functions.
        *   `src/core/ecs_manager.c`: Implementation for initializing, managing, and cleaning up the Flecs world. Also, for registering core, engine-level components if any.

### 3.2. Editor (Domain) Layer Integration

*   **Pevi-Specific Components:**
    *   Components defining the properties of phantoms and other editor-specific entities will reside here.
    *   **New Files:**
        *   `include/pevi/editor/phantom_components.h`: Headers for Pevi-specific components (e.g., `PeviPosition`, `PeviRotation`, `PeviTextBufferRef`, `PeviFilePath`, `PeviIsFocused`, `PeviIsGrabbed`).
        *   `src/editor/phantom_components.c`: Implementation for registering these components with Flecs.
*   **Pevi-Specific Systems:**
    *   Systems that implement the logic for phantoms and editor functionalities.
    *   **New Files:**
        *   `include/pevi/editor/phantom_systems.h`: Headers for Pevi-specific systems.
        *   `src/editor/phantom_systems.c`: Implementation of systems (e.g., `PhantomRenderSystem`, `PhantomInteractionSystem`, `PhantomFileIOSystem`, `CommandExecutionSystem`).

This structure maintains a clear separation of concerns, with the Core layer providing the generic ECS capability and the Editor layer defining the application-specific data (components) and logic (systems).

## 4. Core Concepts and Usage Examples

### 4.1. Flecs World Initialization

The Flecs world will be initialized early in the application's lifecycle, likely in `src/main.c` or an application initialization module, using the `ecs_manager`:

```c
// In main.c or app_init.c
#include "pevi/core/ecs_manager.h"
#include "pevi/editor/phantom_components.h"
#include "pevi/editor/phantom_systems.h"

// ...
int main(int argc, char *argv[]) {
    pevi_ecs_init_world(); // Initializes the global Flecs world via ecs_manager
    
    pevi_register_phantom_components(); // Registers all editor-specific components
    pevi_register_phantom_systems();  // Registers all editor-specific systems

    ecs_world_t* world = pevi_ecs_get_world();
    // ... main loop ...

    pevi_ecs_fini_world(); // Cleans up the Flecs world
    return 0;
}
```

### 4.2. Component Definition

Components are C structs representing pure data.

```c
// include/pevi/editor/phantom_components.h
typedef struct { float x, y, z; } PeviPosition;
typedef struct { float qx, qy, qz, qw; } PeviRotation;
typedef struct { YourTextBuffer* buffer_ptr; } PeviTextBufferRef; // Points to existing text buffer
typedef struct { char path[512]; bool is_dirty; } PeviFileState;
// Tag components (empty structs) for states:
typedef struct { } PeviIsFocused;
typedef struct { } PeviIsGrabbed;
```
Registration in `src/editor/phantom_components.c`:
```c
// src/editor/phantom_components.c
#include "pevi/core/ecs_manager.h"
#include "phantom_components.h" // Self-header

void pevi_register_phantom_components() {
    ecs_world_t* world = pevi_ecs_get_world();
    ECS_COMPONENT_DEFINE(world, PeviPosition);
    ECS_COMPONENT_DEFINE(world, PeviRotation);
    ECS_COMPONENT_DEFINE(world, PeviTextBufferRef);
    ECS_COMPONENT_DEFINE(world, PeviFileState);
    ECS_TAG_DEFINE(world, PeviIsFocused); // For tags
    ECS_TAG_DEFINE(world, PeviIsGrabbed);
    // ... and so on for all components
}
```

### 4.3. Phantom (Entity) Creation

When a new phantom is needed (e.g., opening a file, `:new` command):

```c
// Example: somewhere in command processing logic
ecs_world_t* world = pevi_ecs_get_world();
ecs_entity_t new_phantom = ecs_new_id(world); // Create a new entity

// Set initial components
ecs_set(world, new_phantom, PeviPosition, {0.0f, 0.0f, -5.0f}); // Default position
ecs_set(world, new_phantom, PeviRotation, {0.0f, 0.0f, 0.0f, 1.0f}); // Default rotation (identity quaternion)

// Create and assign a text buffer
YourTextBuffer* text_buf = create_my_text_buffer();
ecs_set(world, new_phantom, PeviTextBufferRef, {text_buf});

if (filepath_to_open) {
    ecs_set(world, new_phantom, PeviFileState, { .path = strcpy_s(path_buf, sizeof(path_buf), filepath_to_open), .is_dirty = false });
    // Load file content into text_buf
} else {
    ecs_set(world, new_phantom, PeviFileState, { .path = "", .is_dirty = false });
}
```

### 4.4. System Definition

Systems contain the logic that operates on entities with specific components.

```c
// src/editor/phantom_systems.c

// System to render phantoms that have a position and text buffer
void RenderPhantoms(ecs_iter_t *it) {
    PeviPosition *p = ecs_field(it, PeviPosition, 1);       // Field 1: PeviPosition
    PeviTextBufferRef *t = ecs_field(it, PeviTextBufferRef, 2); // Field 2: PeviTextBufferRef
    // PeviRotation *r = ecs_field(it, PeviRotation, 3);    // Optional: PeviRotation (if term is optional)

    for (int i = 0; i < it->count; i ++) {
        // Get entity ID: it->entities[i]
        // Access components: p[i], t[i].buffer_ptr
        // Use Raylib to draw text from t[i].buffer_ptr at position p[i]
        // Example: DrawText3D(font, t[i].buffer_ptr->content, (Vector3){p[i].x, p[i].y, p[i].z}, ...);
    }
}

// System to handle grabbing/moving focused phantoms
void GrabFocusedPhantom(ecs_iter_t *it) {
    PeviPosition *p = ecs_field(it, PeviPosition, 1); // Field 1: PeviPosition
    // This system queries for entities that are BOTH PeviIsFocused AND PeviIsGrabbed
    
    for (int i = 0; i < it->count; i++) {
        // Update p[i] based on mouse movement
        // Example: p[i].x += GetMouseDelta().x * drag_speed;
    }
}

void pevi_register_phantom_systems() {
    ecs_world_t* world = pevi_ecs_get_world();

    ECS_SYSTEM_DEFINE(world, RenderPhantoms, EcsOnUpdate, PeviPosition, PeviTextBufferRef, ?PeviRotation);
    // The ?PeviRotation makes the Rotation component optional for this system.
    // EcsOnUpdate means it runs every frame as part of ecs_progress().

    ECS_SYSTEM_DEFINE(world, GrabFocusedPhantom, EcsOnUpdate, PeviPosition, PeviIsFocused, PeviIsGrabbed);
    // This system will only match entities that have Position, AND are Focused, AND are Grabbed.
}
```

### 4.5. Main Loop Integration

The main application loop in `src/main.c` will drive Flecs:

```c
// src/main.c (simplified loop)
ecs_world_t* world = pevi_ecs_get_world();
while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    // --- Input Processing (could be its own system or pre-ECS step) ---
    // ... handle raw input, potentially setting components like MouseInputState ...

    // --- ECS Update ---
    ecs_progress(world, dt); // This executes all registered Flecs systems

    // --- Rendering (largely handled by Flecs Render Systems) ---
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode3D(camera_entity_or_struct.camera); // Camera might also be an entity or managed by a system
        // RenderPhantoms system (and others) will be called by ecs_progress()
    EndMode3D();
    // ... UI rendering (e.g. ImGui, could also be a Flecs system or post-ECS step) ...
    EndDrawing();
}
```

### 4.6. Querying and Modifying Components

Logic outside of systems (e.g., command handlers) can still interact with Flecs:

```c
// Example: Focusing a phantom on click
void process_phantom_click(ecs_entity_t clicked_phantom_id) {
    ecs_world_t* world = pevi_ecs_get_world();

    // Remove PeviIsFocused from any currently focused phantom
    ecs_filter_t *filter = ecs_filter(world, { .terms = {{ ecs_id(PeviIsFocused) }} });
    ecs_iter_t it = ecs_filter_iter(world, filter);
    while(ecs_filter_next(&it)) {
        for(int i = 0; i < it.count; i++) {
            ecs_remove(world, it.entities[i], PeviIsFocused);
        }
    }
    ecs_filter_fini(filter);

    // Add PeviIsFocused to the newly clicked phantom
    ecs_add(world, clicked_phantom_id, PeviIsFocused);
}
```

## 5. Impact on Existing Architecture and Documentation

Integrating Flecs will refine and solidify concepts outlined in `ARCHITECTURE.md` and `PROJECT_STRUCTURE.md`:

*   **`ARCHITECTURE.md`:**
    *   **Component Architecture:** Will explicitly state it's implemented using Flecs. Benefits will be updated to reflect Flecs's strengths (data-orientation, powerful queries).
    *   **Spatial Management:** Will be realized through Flecs components (e.g., `PeviPosition`, `PeviRotation`) attached to phantom entities. Systems will handle spatial logic.
    *   **Phantom System:** Phantoms become Flecs entities. Their lifecycle, properties, and behaviors are managed via components and systems.
    *   **Data Flow:** The diagram and description will be updated to show `ecs_progress()` as the central mechanism for state mutation via systems, and systems querying Flecs for data.
    *   **Technology Stack:** Flecs will be added.
*   **`PROJECT_STRUCTURE.md`:**
    *   The new files (`ecs_manager.h/c`, `phantom_components.h/c`, `phantom_systems.h/c`) will be documented in their respective `include/pevi/core/`, `src/core/`, `include/pevi/editor/`, and `src/editor/` sections.
    *   The "Relationship to `ARCHITECTURE.md`" section will be updated to reflect how these new ECS-specific files map to the architectural layers.

The existing **Text Engine** and **Buffer System** will remain crucial. Flecs components (like `PeviTextBufferRef`) will hold pointers or references to text buffers managed by these systems, effectively linking the ECS world to the text data.

## 6. Benefits Summary

*   **Clearer State Management:** Centralized and predictable state changes through components and systems.
*   **Improved Modularity:** Decoupled logic makes features easier to add, remove, or modify.
*   **Enhanced Performance:** Data-oriented design is cache-friendly.
*   **Greater Extensibility:** Adding new phantom types or behaviors becomes simpler.
*   **Testability:** Systems and components can be tested more easily in isolation.

## 7. Proposed Migration Strategy / Next Steps

1.  **Setup Core ECS Management:**
    *   Create `ecs_manager.h` and `ecs_manager.c` in the `core` layer.
    *   Implement `pevi_ecs_init_world()`, `pevi_ecs_get_world()`, `pevi_ecs_fini_world()`.
    *   Integrate initialization/cleanup calls into `src/main.c`.
2.  **Define Basic Phantom Components:**
    *   Create `phantom_components.h` and `phantom_components.c` in the `editor` layer.
    *   Define and register essential components: `PeviPosition`, `PeviTextBufferRef`.
3.  **Refactor Phantom Creation:**
    *   Modify the logic for creating a new phantom (e.g., from a command) to create a Flecs entity and assign the basic components.
4.  **Implement a Basic Render System:**
    *   Create `phantom_systems.h` and `phantom_systems.c`.
    *   Implement a simple `RenderPhantoms` system that queries for entities with `PeviPosition` and `PeviTextBufferRef` and uses Raylib to draw them.
    *   Register this system and ensure `ecs_progress()` is called in the main loop.
5.  **Iterative Migration:**
    *   Gradually migrate existing phantom properties (e.g., file path, focus state, grabbed state) to Flecs components.
    *   Refactor corresponding logic (e.g., input handling, file operations) into new Flecs systems or adapt existing functions to interact with Flecs entities.
    *   Address camera management (potentially making the camera an entity or its state managed by a system).
6.  **Documentation Update:**
    *   Update `ARCHITECTURE.md` and `PROJECT_STRUCTURE.md` as outlined in section 5.

This phased approach allows for incremental adoption of Flecs, minimizing disruption and allowing the team to gain familiarity with the ECS paradigm.
