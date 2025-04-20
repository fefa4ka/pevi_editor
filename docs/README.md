# Pevi - 3D Physics-Enabled Code Editor

Pevi is an innovative code editor that operates in 3D space with realistic physics. Unlike traditional text editors confined to 2D windows, Pevi allows code to be visualized and manipulated in a three-dimensional environment where code blocks behave like physical objects.

## Core Concept

Pevi reimagines code editing by representing code as "phantoms" - physical objects that float in 3D space. These phantoms can be:

- Grabbed, moved, and arranged spatially
- Organized to visualize relationships between code components
- Manipulated with realistic physics (throw, stack, collide)
- Edited with familiar text editing capabilities

This spatial approach to code editing provides a unique perspective that can enhance understanding of complex codebases and create more intuitive mental models of code structure.

## Key Features

- **Physics-Enabled Environment**: Code phantoms with realistic physics behavior
- **3D Workspace**: Navigate through your code in a fully 3D environment
- **Spatial Organization**: Arrange code spatially to visualize relationships
- **Chunk-Based Editing**: Different phantoms represent different sections of files
- **Multi-Modal Interaction**: Seamlessly switch between navigation, editing, and command modes
- **Modern Text Engine**: UTF-8 support with efficient buffer management
- **Command System**: Powerful command interface with history and arguments
- **Entity-Component Architecture**: Flexible and extensible design

## Current Implementation Status

### Working Features

- **Physics Integration**:
  - JoltPhysics-powered phantom interaction
  - Realistic collision detection between phantoms
  - Physics-driven movement and positioning
  - Drag-and-drop phantom repositioning

- **Editor Core**:
  - 3D workspace with floating code phantoms
  - Multiple phantom management (cycle with N/P keys)
  - Camera controls with multiple modes (free, edit, command)
  - SDF font rendering with custom shaders
  - Linked ring text buffers for efficient editing

- **User Interface**:
  - Command system with history and arguments (:e <file>, :q)
  - Context-aware input handling with multiple modes
  - UTF-8 text editing with proper cursor management

- **System Architecture**:
  - ECS (Entity-Component-System) architecture
  - Event system for inter-component communication
  - Memory tracking and leak detection system
  - Error handling system with contextual logging

### Interaction Controls

- **Navigation Mode** (Free):
  - WASD/Arrow keys: Move camera
  - Mouse: Look around
  - Shift: Enable mouse look
  - G: Grab/Release phantom (physics-enabled)
  - F: Apply force to phantom in view direction
  - R: Reset physics state of current phantom
  - N/P: Cycle between phantoms
  - I: Enter edit mode on current phantom
  - :: Enter command mode

- **Edit Mode**:
  - Type to insert text
  - Backspace/Delete: Remove characters
  - Enter: Insert new line
  - ESC: Return to navigation
  - Ctrl+S: Save current phantom
  - Ctrl+Q: Quit

- **Command Mode**:
  - :e <file> - Open file in new phantom
  - :q - Quit application
  - :new - Create new phantom
  - :d - Delete current phantom

### Planned Features

- **Visualization**:
  - Syntax highlighting shaders
  - Code structure visualization
  - Relationship visualization between phantoms

- **Integration**:
  - Project workspace management
  - File browser integration
  - Version control visualization

- **Extensibility**:
  - Plugin system architecture
  - Multi-window support
  - Collaborative editing features


