# Entity Component System Architecture in Pevi

## Overview

Pevi uses an Entity Component System (ECS) architecture to manage all objects in the 3D editor space. This document describes the conceptual model and patterns used, independent of any specific ECS implementation.

## Core Concepts

### Entities
Entities are unique identifiers that represent objects in the editor. They have no data or behavior on their own - they're simply IDs that components can be attached to.

```pseudo
entity = world.create_entity()
entity2 = world.create_entity("named_entity")
```

### Components
Components are data containers that can be attached to entities. They contain only data, no logic.

```pseudo
// Component definitions
component Position {
    x: float
    y: float
    z: float
}

component TextBuffer {
    content: string
    filepath: string
    is_dirty: boolean
}

component Phantom {
    width: float
    height: float
    is_visible: boolean
    is_focused: boolean
}
```

### Systems
Systems contain the logic that operates on entities with specific component combinations.

```pseudo
system UpdatePhantomPositions {
    // Query for entities with both Position and Phantom components
    query: [Position, Phantom]
    
    update(delta_time) {
        for each entity matching query {
            position = entity.get(Position)
            phantom = entity.get(Phantom)
            
            // Update logic here
            if phantom.is_visible {
                // Process phantom position
            }
        }
    }
}
```

## Pevi Entity Architecture

### Core Entity Types

#### Editor Entity
The main editor singleton entity that manages global editor state.

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
Represents the 3D camera for navigating the editor space.

```pseudo
entity Camera {
    components: [
        Position { x, y, z },
        Rotation { pitch, yaw, roll },
        Camera {
            fov: float
            near_plane: float
            far_plane: float
            target: vec3
        }
    ]
}
```

#### Phantom Entities
Phantoms are the visual representations of code in 3D space.

```pseudo
entity Phantom {
    components: [
        Position { x, y, z },
        Rotation { pitch, yaw, roll },
        Phantom {
            width: float
            height: float
            is_visible: boolean
            is_focused: boolean
            phantom_id: int
        },
        TextBuffer {
            content: string
            length: size
            capacity: size
            is_dirty: boolean
            filepath: string  // null if unsaved
        }
    ]
}
```

### Relationships

Pevi uses entity relationships to model connections between entities.

```pseudo
// Parent-child relationships for grouping
phantom1.add_relationship(ChildOf, group_entity)
phantom2.add_relationship(ChildOf, group_entity)

// Phantom references a file
phantom.add_relationship(ReferencesFile, file_entity)

// Camera targets a phantom
camera.add_relationship(Targeting, phantom)
```

## System Architecture

### Core Systems

#### Input Processing System
```pseudo
system ProcessInput {
    query: [EditorMode]
    
    update() {
        mode = get_singleton(EditorMode)
        
        if mode.is_navigation {
            // Process navigation inputs
            dispatch_event(NavigationInput, input_data)
        } else if mode.is_edit {
            // Process edit inputs
            dispatch_event(EditInput, input_data)
        }
    }
}
```

#### Phantom Management System
```pseudo
system ManagePhantoms {
    query: [Phantom, Position, TextBuffer]
    
    update() {
        for each entity matching query {
            phantom = entity.get(Phantom)
            buffer = entity.get(TextBuffer)
            
            if buffer.is_dirty {
                // Mark for save
                dispatch_event(BufferDirty, entity)
            }
            
            if phantom.is_focused {
                // Update focus visuals
                update_phantom_appearance(entity)
            }
        }
    }
}
```

#### Camera Control System
```pseudo
system ControlCamera {
    query: [Camera, Position, Rotation]
    singleton_query: [EditorMode]
    
    update(delta_time) {
        mode = get_singleton(EditorMode)
        if not mode.is_navigation {
            return
        }
        
        for each camera matching query {
            position = camera.get(Position)
            rotation = camera.get(Rotation)
            
            // Apply movement based on input
            apply_camera_movement(position, rotation, delta_time)
            
            // Update view matrix
            update_view_matrix(camera)
        }
    }
}
```

#### Text Rendering System
```pseudo
system RenderText {
    query: [Phantom, TextBuffer, Position]
    singleton_query: [Camera]
    
    render() {
        camera = get_singleton_entity(Camera)
        camera_pos = camera.get(Position)
        
        for each phantom matching query {
            pos = phantom.get(Position)
            buffer = phantom.get(TextBuffer)
            phantom_data = phantom.get(Phantom)
            
            if phantom_data.is_visible {
                // Calculate screen position
                screen_pos = world_to_screen(pos, camera)
                
                // Render text content
                render_text_buffer(buffer, screen_pos, phantom_data)
            }
        }
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

## Query Patterns

### Basic Queries
```pseudo
// All entities with Position
query = world.query([Position])

// All entities with Position AND Velocity
query = world.query([Position, Velocity])

// All entities with Position but NOT Hidden
query = world.query([Position, !Hidden])
```

### Hierarchical Queries
```pseudo
// All phantoms that are children of a specific group
query = world.query([Phantom, ChildOf(group_entity)])

// All top-level phantoms (no parent)
query = world.query([Phantom, !ChildOf(*)])
```

### Singleton Queries
```pseudo
// Access singleton components
camera_settings = world.get_singleton(CameraSettings)
editor_mode = world.get_singleton(EditorMode)
```

## Benefits for Pevi

1. **Modularity**: Each aspect of the editor (phantoms, camera, text editing) is isolated in its own components and systems.

2. **Flexibility**: New features can be added by creating new components and systems without modifying existing code.

3. **Performance**: ECS allows efficient iteration over entities with specific component combinations.

4. **Spatial Organization**: The ECS naturally supports Pevi's spatial metaphor - positions, relationships, and hierarchies are first-class concepts.

5. **State Management**: All editor state is explicitly stored in components, making it easy to save/load, undo/redo, or synchronize.

6. **Extensibility**: The component-based architecture makes it straightforward to add new phantom types, visualization modes, or editor features.

## Example: Creating a New Phantom

```pseudo
function create_phantom(world, x, y, z, content) {
    // Create entity
    phantom = world.create_entity()
    
    // Add components
    phantom.add(Position { x: x, y: y, z: z })
    phantom.add(Rotation { pitch: 0, yaw: 0, roll: 0 })
    phantom.add(Phantom {
        width: 400,
        height: 300,
        is_visible: true,
        is_focused: false,
        phantom_id: generate_id()
    })
    phantom.add(TextBuffer {
        content: content,
        length: content.length,
        capacity: content.length * 2,
        is_dirty: false,
        filepath: null
    })
    
    // Dispatch creation event
    world.dispatch_event(PhantomCreated { entity: phantom })
    
    return phantom
}
```

This architecture provides a clean separation of concerns while maintaining the flexibility needed for Pevi's innovative 3D code editing approach.
