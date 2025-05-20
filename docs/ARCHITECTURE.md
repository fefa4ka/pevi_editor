# Pevi Editor Architecture

## Overview

Pevi is a 3D code editor. Its architecture is designed for modularity and usability, supporting spatial code organization with a clear separation of concerns.

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
│                           │       │ • Scripting Engine / Ext API  │
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
* **Cross-platform:** Aims for consistent event processing behavior across supported operating systems.
* **Simple API:** Easy to understand and use


## Domain Layer

The Domain Layer contains the core business logic specific to the code editor functionality.

### Text Engine

The Text Engine manages the representation and manipulation of text content:

* **Buffer System:** Efficient storage and manipulation of text using linked ring buffers
* **UTF-8 Support:** Full Unicode text handling with proper character boundaries
* **Basic Undo/Redo:** Simple operation history
* **Text Measurement:** Character and line metrics for rendering

### Phantom System

Phantoms are the 3D representations of code in the editor space:

* **Content Management:** Maps buffers to visual representations
* **Layout Engine:** Determines text layout within phantoms
* **Lifecycle Management:** Creation, updating, and destruction of phantoms
* **Selection Model:** Text selection within phantoms
* **Visibility Control:** Show/hide and focus management

### Command System

The Command System processes essential user commands:

* **Command Parser:** Interprets basic command syntax (:e, :q, etc.)
* **Command Execution:** Routes commands to appropriate subsystems
* **Simple Validation:** Basic argument validation

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

Handles user input through a simplified, mode-based system:

* **Mode-Based Input:** Basic input handling for navigation and editing
* **Key Handling:** Essential keyboard shortcuts
* **Mouse Interaction:** 3D selection and movement of phantoms
* **Context Switching:** Simple mode switching between navigation and editing

### Scripting Engine / Extension API

(Future Goal) This system will provide the infrastructure for extending Pevi's functionality:

* **Scripting Language Integration:** Embed a scripting engine (e.g., Lua, Wren, or similar lightweight language) to allow users to write scripts that interact with the editor.
* **Extension API:** Define a clear API that extensions can use to add new features, commands, UI elements, or modify existing behavior.
* **Lifecycle Management:** Handle the loading, unloading, and execution of scripts and extensions.
* **Sandboxing:** (Consideration) Provide a secure environment for running untrusted code.

## Data Flow

Data flow within Pevi follows streamlined patterns to ensure efficient processing and clear separation of concerns:

```
┌─────────────┐       ┌──────────────────┐       ┌──────────────────┐
│ User Input  │──────▶│   Input System   │──────▶│   Event Bus      │
└─────────────┘       │ (Raw to Abstract │       │ (Input Events,   │
                      │  Input Events)   │       │  Command Events) │
                      └──────────────────┘       └────────┬─────────┘
                                                          │
                                                          │ (Events)
                                                          ▼
                        ┌───────────────────────────────────────────────────┐
                        │ Event Handlers / Logic Systems (Domain Layer)     │
                        │ ------------------------------------------------- │
                        │ * Process events (from Event Bus)                 │
                        │ * Interact with Services (e.g., File I/O)         │
                        │ * MUTATE STATE (primarily in Components)          │
                        └──────────────────┬─────────────────┬──────────────┘
                                           │                 │
(Services used by Logic)                   │ (State Access)  │ (State Access)
┌───────────┐                              │                 │
│ File I/O  │◀─────────────────────────────┘                 │
└───────────┘                                                │
                                                             │
                                                             ▼
                                          ┌──────────────────────────────────┐
                                          │ Entities & Components (The State)│
                                          │ -------------------------------- │
                                          │ - Phantoms (as Entities)         │
                                          │ - Camera State                   │
                                          └─────────────────┬────────────────┘
                                                            │
                                                            │ (Read State for Rendering)
                                                            ▼
┌─────────────┐       ┌───────────────────────────────────────────────────┐
│  Display    │◀──────│ Rendering Systems (Service Layer, using Raylib)   │
└─────────────┘       │ ------------------------------------------------- │
                      │ * Read Components                                 │
                      └───────────────────────────────────────────────────┘
```



## Technology Stack

| Layer          | Technology                | Purpose                                |
|----------------|---------------------------|----------------------------------------|
| Core           | C11                       | Programming language                   |
| Graphics       | Raylib                    | Rendering, windowing, input            |
| Math           | Raymath                   | Vector/matrix operations               |
| Build System   | CMake                     | Cross-platform build configuration     |
| Testing        |                           | Unit testing framework                 |

