# Pevi Editor Architecture

## Overview
Architecture designed for a 3D code editor, focusing on modularity, performance, and extensibility:

```
┌─────────────────────────────┐
│       Application Layer     │
├─────────────────────────────┤
│ - Application lifecycle     │
│ - System coordination       │
│ - Plugin management         │
└───────────────┬─────────────┘
                │
┌───────────────▼─────────────┐
│         Core Layer          │
├─────────────────────────────┤
│ 1. ECS Framework (Flecs)    │
│ 2. libuv Event Loop         │
│ 3. Memory Management        │
│ 4. Error Handling           │
│ 5. Configuration System     │
└───────────────┬─────────────┘
                │
    ┌───────────┴───────────┐
    │                       │
┌───▼───────────┐     ┌─────▼───────────┐
│ Domain Layer  │     │ Services Layer  │
├───────────────┤     ├─────────────────┤
│ 1. Text Engine│     │ 1. Renderer     │
│ 2. Phantom    │     │ 2. Input System │
│ 3. Buffer     │     │ 3. File I/O     │
│ 4. Command    │     │ 4. UI System    │
│ 5. Camera     │     │ 5. Logger       │
└───────────────┘     └─────────────────┘
```

## Key Components

### Physics Engine - JoltPhysics

Pevi integrates the **JoltPhysics** engine (https://github.com/jrouwe/JoltPhysics) via its C API (JoltC) to provide realistic physics simulation for the 3D editor environment. This enables:

* **Physical Interaction**: Phantoms can be physically manipulated with realistic physics
* **Collision Detection**: Prevents phantoms from intersecting each other
* **Spatial Awareness**: Physics-based organization of code elements
* **Dynamic Behavior**: Natural movement and positioning of UI elements

**Integration:**
* Physics bodies are created for each phantom
* Collision shapes match the visual representation
* Physics simulation runs in a separate thread via the JobSystem
* Two collision layers are used: one for static elements and one for dynamic elements

### Entity Component System (ECS) - Flecs

Instead of a custom implementation, Pevi will leverage the **Flecs** library (https://github.com/SanderMertens/flecs). Flecs is a fast and flexible open-source Entity Component System framework for C/C++.

**Rationale for using Flecs:**
*   **Maturity and Stability:** Flecs is a well-established library with a strong community and ongoing development.
*   **Performance:** Highly optimized for cache efficiency and low overhead.
*   **Rich Feature Set:** Offers features like relationships, hierarchies, systems scheduling, reflection, and more out-of-the-box.
*   **Reduced Development Time:** Avoids the need to implement, test, and maintain a complex custom ECS framework.

**Integration:**
*   Core game logic, entities (like Phantoms, Cursors), and their associated data (components) will be managed by the Flecs world (`ecs_world_t`).
*   Systems (logic operating on components) will be defined using the Flecs API.

#### Phantom Component System
```c
// Components for ECS
typedef struct {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
} TransformComponent;

typedef struct {
    BufferId buffer_id;  // Reference to buffer
    size_t line_from;
    size_t line_to;
    FontSettings font;
    Color color;
} PhantomComponent;

typedef struct {
    size_t line_no;
    size_t pos;
    struct lr_cell* owner;
    struct lr_cell* needle;
} CursorComponent;

typedef struct {
    JPH_Body* body;           // JoltPhysics body
    JPH_Shape* shape;         // Collision shape
    JPH_BodyID body_id;       // Body identifier
    JPH_MotionType motion_type; // Static, dynamic, or kinematic
    float mass;               // Mass of the physics body
    float restitution;        // Bounciness factor
    float friction;           // Surface friction
} PhysicsComponent;
```
*Note: These components will be registered with and managed by the Flecs ECS world.*
                           

### Event System with libuv

Instead of a custom event bus implementation, Pevi will leverage the **libuv** library (https://github.com/libuv/libuv) for event handling and asynchronous I/O operations.

**Rationale for using libuv:**
* **Cross-platform:** Works consistently across different operating systems
* **Performance:** Highly optimized event loop with minimal overhead
* **Mature Ecosystem:** Well-tested in production environments (Node.js, etc.)
* **Rich Feature Set:** Provides timers, file I/O, networking, and more
* **Reduced Development Time:** Avoids implementing a custom event system


## Data Flow
```
1. Input → State Machine → libuv Event Loop → Domain Systems (via Flecs)
2. File I/O → libuv Async Operations → Buffer System → Phantom Components (in Flecs)
3. Flecs Registry → Rendering Pipeline → Display
4. Command → libuv Event → System Updates (via Flecs)
5. Config Changes → libuv Event → System Reconfiguration
6. Plugin Messages → libuv Event Queue → Plugin Handler → System Updates
7. Timer Events → libuv Timer Callbacks → Scheduled Operations
8. Network I/O → libuv TCP/UDP Handlers → Data Processing Pipeline
```

## Benefits

- **Clear Separation of Concerns**: Domain logic separated from technical services
- **High Performance**: Leverages the optimized Flecs ECS core and libuv event loop
- **Asynchronous I/O**: Non-blocking file operations via libuv
- **Cross-Platform Event Handling**: Consistent behavior across operating systems
- **State-Based Input**: Clear transitions between editor modes
- **Configurable Rendering**: Layer-based approach for extensibility
- **Memory Optimization**: Context-based allocation for better resource management
- **Plugin Architecture**: Designed for extension from the start
- **Leverages Mature Libraries**: Benefits from Flecs and libuv features, stability, and community support

## Physics Integration

The physics system is integrated with the ECS architecture:

```
┌─────────────────────────────┐
│      Physics System         │
├─────────────────────────────┤
│ 1. JoltPhysics Integration  │
│ 2. Collision Detection      │
│ 3. Physics Step System      │
│ 4. Body Management          │
│ 5. Force Application        │
└───────────────┬─────────────┘
                │
                ▼
┌─────────────────────────────┐
│      ECS (Flecs)            │
├─────────────────────────────┤
│ - PhysicsComponent          │
│ - TransformComponent        │
│ - PhantomComponent          │
└───────────────┬─────────────┘
                │
                ▼
┌─────────────────────────────┐
│      Rendering System       │
└─────────────────────────────┘
```

### Physics Workflow

1. **Initialization**:
   - Initialize JoltPhysics with `JPH_Init()`
   - Set up trace and assertion handlers
   - Create JobSystem for multithreaded physics
   - Configure collision system with layers

2. **Body Creation**:
   - Create shapes for phantoms (typically box shapes)
   - Set up body creation settings (mass, motion type)
   - Add bodies to physics system

3. **Simulation**:
   - Step physics world in fixed time steps
   - Synchronize ECS transform components with physics bodies
   - Handle collision events via callbacks

4. **Interaction**:
   - Apply forces/impulses for user interaction
   - Update motion states based on editor commands
   - Handle constraints for special behaviors

## Tech Stack

| Layer          | Technology                |
|----------------|---------------------------|
| Core           | C11, Flecs, libuv         |
| Physics        | JoltPhysics (JoltC)       |
| Math           | Raymath                   |
| Rendering      | Raylib                    |
| Windowing      | Raylib                    |
| Build System   | CMake                     |
| Testing        | Unity                     |

