# Pevi - 3D Code Editor

Pevi is an innovative code editor that operates in 3D space. Unlike traditional text editors that are confined to 2D windows, Pevi allows code to be visualized and manipulated in a three-dimensional environment.

## Key Features

- **3D Workspace**: Navigate through your code in a fully 3D environment
- **Floating Panes**: Code is displayed on panes that float in 3D space
- **Chunk-Based Editing**: Different panes can represent different chunks or sections of files
- **Spatial Organization**: Organize your code spatially to visualize relationships between components
- **Immersive Editing**: Experience a new way of interacting with your codebase

Pevi aims to provide a unique perspective on code editing by leveraging spatial relationships to enhance understanding of complex codebases.

## Current Implementation Status

### Working Features
- 3D workspace with floating code phantoms (panes)
- Physics-based phantom interaction with JoltPhysics
- Realistic collision detection between phantoms
- Physics-driven movement and positioning
- Drag-and-drop phantom repositioning (G key)
- Multiple phantom management (N/P keys to cycle)
- Camera controls with multiple modes (free, edit, command)
- SDF font rendering with custom shaders
- Linked ring text buffers for efficient editing
- Command system with history and arguments (:e <file>, :q)
- Context-aware input handling with multiple modes
- UTF-8 text editing with proper cursor management
- Memory tracking and leak detection system
- Error handling system with contextual logging
- ECS (Entity-Component-System) architecture
- Event bus system for inter-component communication


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

### Features
- Phantom drag-and-drop positioning
- Camera-relative billboarding
- Multi-line text measurement
- Command history and argument parsing
- Memory tracking with allocation types
- Event-driven input system
- Contextual error handling
- Linked ring buffer operations
- UTF-8 character handling

- Syntax highlighting shaders
- Project workspace management
- File browser integration
- Code structure visualization
- Plugin system architecture
- Multi-window support
- Collaborative editing features


