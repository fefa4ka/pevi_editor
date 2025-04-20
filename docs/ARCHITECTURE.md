# Pevi Editor Architecture

## Overview

Pevi is a 3D code editor built with a simplified architecture that emphasizes modularity and usability. The architecture is designed to support spatial code organization while maintaining clean separation of concerns.

```
┌─────────────────────────────────────────────────────────────────┐
│                      Application Layer                          │
├─────────────────────────────────────────────────────────────────┤
│ • Application Lifecycle   • System Coordination                 │
│ • Configuration Management                                      │
└───────────────────────────────┬─────────────────────────────────┘
                                │
┌───────────────────────────────▼─────────────────────────────────┐
│                         Core Layer                              │
├─────────────────────────────────────────────────────────────────┤
│ • Component Framework     • Event System                        │
│ • Memory Management       • Error Handling                      │
│ • Spatial Management      • Basic Threading                     │
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

### Component Architecture

Pevi uses a component-based architecture for managing editor entities and systems.

**Key Benefits:**
* **Modular Design:** Components can be added or removed as needed
* **Separation of Concerns:** Each component handles a specific aspect of functionality
* **Maintainable Code:** Easier to understand and modify individual components
* **Testability:** Components can be tested in isolation

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

// Spatial components
typedef struct {
    bool is_selected;         // Whether phantom is currently selected
    bool is_visible;          // Whether phantom is currently visible
    Vector3 target_position;  // Position phantom should move toward
} SpatialComponent;
```

### Spatial Management

Pevi provides a simplified spatial management system for organizing code in 3D space.

**Spatial Capabilities:**
* **Positioning:** Phantoms can be placed at specific 3D coordinates
* **Selection:** Users can select and move phantoms
* **Grouping:** Related phantoms can be positioned near each other
* **Visibility Control:** Phantoms can be shown or hidden based on context

**Implementation Details:**
* Simple interpolation for smooth movement between positions
* Basic collision detection to prevent complete overlap
* Spatial queries to find phantoms in specific regions

### Event System

Pevi uses a simple event system for handling user interactions and system events.

**Event System Features:**
* **Event Dispatching:** Central event queue for processing user actions
* **Event Handlers:** Components register for events they need to process
* **Cross-platform:** Consistent behavior across operating systems
* **Simple API:** Easy to understand and use


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

* **3D Rendering:** Manages the spatial organization of visual elements
* **Font Rendering:** Clear text rendering for code display
* **Camera Management:** Simple camera controls for navigation
* **Basic Shaders:** Essential shaders for text rendering
* **Billboarding:** Camera-relative orientation of phantoms

### Input System

Manages user input with simplified handling:

* **Mode-Based Input:** Basic input handling for navigation and editing
* **Key Handling:** Essential keyboard shortcuts
* **Mouse Interaction:** 3D selection and movement of phantoms
* **Context Switching:** Simple mode switching between navigation and editing

## Data Flow

The data flows through the system in streamlined patterns:

```
┌─────────────┐     ┌─────────────┐     ┌───────────────┐
│  User Input │────▶│ Input System│────▶│ Event System  │
└─────────────┘     └─────────────┘     └──────┬────────┘
                                               │
┌─────────────┐     ┌─────────────┐     ┌──────▼────────┐
│   Display   │◀────│  Renderer   │◀────│ Domain Logic  │
└─────────────┘     └─────────────┘     └───────────────┘
```

Key data flows include:

1. **Input Processing:** Input → Event System → Domain Systems
2. **File Operations:** File I/O → Buffer System → Phantoms
3. **Rendering Pipeline:** Components → Rendering Systems → Display
4. **Command Execution:** Command → Event → System Updates
5. **Spatial Management:** Position Updates → Transform Components

## Technology Stack

| Layer          | Technology                | Purpose                                |
|----------------|---------------------------|----------------------------------------|
| Core           | C11                       | Programming language                   |
| Graphics       | Raylib                    | Rendering, windowing, input            |
| Math           | Raymath                   | Vector/matrix operations               |
| Build System   | CMake                     | Cross-platform build configuration     |
| Testing        | Unity                     | Unit testing framework                 |

