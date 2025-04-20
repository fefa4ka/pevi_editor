# Pevi Editor Architecture

## Overview

Pevi is a 3D code editor built with a layered architecture that emphasizes modularity, performance, and extensibility. The architecture is designed to support the unique requirements of a physics-enabled 3D editing environment while maintaining clean separation of concerns.

```
┌─────────────────────────────────────────────────────────────────┐
│                      Application Layer                          │
├─────────────────────────────────────────────────────────────────┤
│ • Application Lifecycle   • System Coordination                 │
│ • Plugin Management       • Configuration Management            │
└───────────────────────────────┬─────────────────────────────────┘
                                │
┌───────────────────────────────▼─────────────────────────────────┐
│                         Core Layer                              │
├─────────────────────────────────────────────────────────────────┤
│ • ECS Framework (Flecs)   • Event System (libuv)                │
│ • Memory Management       • Error Handling                      │
│ • Physics Engine (JoltC)  • Threading & Concurrency             │
└───────────────────────────────┬─────────────────────────────────┘
                                │
            ┌───────────────────┴───────────────────┐
            │                                       │
┌───────────▼───────────────┐       ┌───────────────▼───────────────┐
│      Domain Layer         │       │      Services Layer           │
├───────────────────────────┤       ├───────────────────────────────┤
│ • Text Engine             │       │ • Renderer (Raylib)           │
│ • Phantom Management      │       │ • Input System                │
│ • Buffer System           │       │ • File I/O                    │
│ • Command Processor       │       │ • UI System                   │
│ • Camera System           │       │ • Logging Service             │
└───────────────────────────┘       └───────────────────────────────┘
```

## Core Systems

### Entity Component System (ECS) - Flecs

Pevi uses the **Flecs** library (https://github.com/SanderMertens/flecs) as its core architectural pattern, providing a data-oriented approach to managing game entities and systems.

**Key Benefits:**
* **Data-Oriented Design:** Components are organized for optimal cache efficiency
* **Declarative Logic:** Systems operate on component queries rather than object hierarchies
* **Flexible Composition:** Entities can be composed of multiple components at runtime
* **Reactive Systems:** Event-based triggers for component changes
* **Built-in Reflection:** Type information available at runtime

**Core Components:**
```c
// Transform components
typedef struct {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
} TransformComponent;

// Phantom (code display) components
typedef struct {
    BufferId buffer_id;  // Reference to text buffer
    size_t line_from;    // Starting line to display
    size_t line_to;      // Ending line to display
    FontSettings font;   // Font configuration
    Color color;         // Text color
} PhantomComponent;

// Cursor components
typedef struct {
    size_t line_no;           // Current line number
    size_t pos;               // Position in line
    struct lr_cell* owner;    // Buffer cell reference
    struct lr_cell* needle;   // Current position reference
} CursorComponent;

// Physics components
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

### Physics Engine - JoltPhysics

Pevi integrates the **JoltPhysics** engine via its C API (JoltC) to provide realistic physics simulation for the 3D editor environment.

**Physics Capabilities:**
* **Physical Interaction:** Phantoms can be grabbed, thrown, and manipulated
* **Collision Detection:** Prevents phantoms from intersecting each other
* **Spatial Organization:** Physics-based arrangement of code elements
* **Dynamic Behavior:** Natural movement and positioning of UI elements

**Integration Details:**
* Physics bodies are created for each phantom with appropriate collision shapes
* Physics simulation runs in a dedicated thread via the JoltPhysics JobSystem
* Two collision layers separate static and dynamic elements
* Transform components are synchronized with physics bodies each frame

### Event System - libuv

Pevi uses the **libuv** library (https://github.com/libuv/libuv) for event handling and asynchronous I/O operations.

**Event System Features:**
* **Non-blocking I/O:** Asynchronous file operations prevent UI freezing
* **Event Loop:** Centralized event processing with callbacks
* **Cross-platform:** Consistent behavior across operating systems
* **Timer Management:** Precise scheduling of recurring operations
* **Signal Handling:** Clean response to system signals


## Domain Layer

The Domain Layer contains the core business logic specific to the code editor functionality.

### Text Engine

The Text Engine manages the representation and manipulation of text content:

* **Buffer System:** Efficient storage and manipulation of text using linked ring buffers
* **UTF-8 Support:** Full Unicode text handling with proper character boundaries
* **Undo/Redo:** Multi-level operation history with transaction support
* **Syntax Analysis:** Lexical analysis for syntax highlighting
* **Text Measurement:** Character and line metrics for rendering

### Phantom System

Phantoms are the 3D representations of code in the editor space:

* **Content Management:** Maps buffers to visual representations
* **Layout Engine:** Determines text layout within phantoms
* **Lifecycle Management:** Creation, updating, and destruction of phantoms
* **Selection Model:** Text selection within and across phantoms
* **Visibility Control:** Show/hide and focus management

### Command System

The Command System processes user commands and editor operations:

* **Command Parser:** Interprets command syntax (:e, :q, etc.)
* **Command History:** Tracks and allows recall of previous commands
* **Argument Processing:** Handles command arguments and validation
* **Command Execution:** Routes commands to appropriate subsystems
* **Macro Support:** Recording and playback of command sequences

## Services Layer

The Services Layer provides technical capabilities that support the domain logic.

### Rendering System

Built on Raylib, the rendering system visualizes the 3D editor environment:

* **3D Scene Graph:** Manages the spatial organization of visual elements
* **SDF Font Rendering:** High-quality text rendering with custom shaders
* **Camera Management:** Multiple camera modes with smooth transitions
* **Shader Pipeline:** Custom shaders for text, effects, and UI
* **Billboarding:** Camera-relative orientation of phantoms

### Input System

Manages all user input with context-sensitive handling:

* **Mode-Based Input:** Different input handling based on editor mode
* **Key Mapping:** Configurable keyboard shortcuts
* **Mouse Interaction:** 3D picking and interaction with phantoms
* **Input Context Stack:** Push/pop contexts for modal operations
* **Gesture Recognition:** Support for trackpad gestures

## Data Flow

The data flows through the system in well-defined patterns:

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  User Input │────▶│ Input System│────▶│ State Machine│────▶│ libuv Event │
└─────────────┘     └─────────────┘     └─────────────┘     └──────┬──────┘
                                                                   │
┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌──────▼──────┐
│   Display   │◀────│  Renderer   │◀────│ ECS Systems │◀────│ Domain Logic│
└─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘
```

Key data flows include:

1. **Input Processing:** Input → State Machine → Event Loop → Domain Systems
2. **File Operations:** File I/O → Async Operations → Buffer System → Phantoms
3. **Rendering Pipeline:** ECS Components → Rendering Systems → Display
4. **Command Execution:** Command → Event → System Updates
5. **Physics Simulation:** Physics Step → Body Updates → Transform Components

## Physics Integration

The physics system is tightly integrated with the ECS architecture:

```
┌─────────────────────────────┐
│      Physics System         │
├─────────────────────────────┤
│ • JoltPhysics Integration   │
│ • Collision Detection       │
│ • Physics Step System       │
│ • Body Management           │
│ • Force Application         │
└───────────────┬─────────────┘
                │
                ▼
┌─────────────────────────────┐
│      ECS (Flecs)            │
├─────────────────────────────┤
│ • PhysicsComponent          │
│ • TransformComponent        │
│ • PhantomComponent          │
└───────────────┬─────────────┘
                │
                ▼
┌─────────────────────────────┐
│      Rendering System       │
└─────────────────────────────┘
```

### Physics Workflow

1. **Initialization:**
   - Initialize JoltPhysics with appropriate configuration
   - Set up trace and assertion handlers
   - Create JobSystem for multithreaded physics
   - Configure collision system with layers

2. **Body Creation:**
   - Create shapes for phantoms (typically box shapes)
   - Set up body creation settings (mass, motion type)
   - Add bodies to physics system

3. **Simulation:**
   - Step physics world in fixed time steps
   - Synchronize ECS transform components with physics bodies
   - Handle collision events via callbacks

4. **Interaction:**
   - Apply forces/impulses for user interaction
   - Update motion states based on editor commands
   - Handle constraints for special behaviors

## Technology Stack

| Layer          | Technology                | Purpose                                |
|----------------|---------------------------|----------------------------------------|
| Core           | C11                       | Programming language                   |
|                | Flecs                     | Entity Component System                |
|                | libuv                     | Event loop and async I/O               |
| Physics        | JoltPhysics (JoltC)       | Physics simulation                     |
| Graphics       | Raylib                    | Rendering, windowing, input            |
| Math           | Raymath                   | Vector/matrix operations               |
| Build System   | CMake                     | Cross-platform build configuration     |
| Testing        | Unity                     | Unit testing framework                 |
| Version Control| Git                       | Source code management                 |

