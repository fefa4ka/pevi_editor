# Pevi - 3D Spatial Code Editor

Pevi is an innovative code editor that operates in 3D space. Unlike traditional text editors confined to 2D windows, Pevi allows code to be visualized and manipulated in a three-dimensional environment where code blocks can be spatially arranged to represent relationships.

## Core Concept

Pevi reimagines code editing by representing code as "phantoms" - objects that float in 3D space. These phantoms can be:

- Positioned and arranged spatially
- Organized to visualize relationships between code components
- Moved and grouped to create meaningful layouts
- Edited with familiar text editing capabilities

This spatial approach to code editing provides a unique perspective that can enhance understanding of complex codebases and create more intuitive mental models of code structure.

## Key Features

- **Spatial Organization**: Arrange code in 3D to visualize relationships
- **3D Workspace**: Navigate through your code in a three-dimensional environment
- **Chunk-Based Editing**: Different phantoms represent different sections of files
- **Simplified Interaction**: Intuitive controls for navigation and editing
- **Modern Text Engine**: UTF-8 support with efficient buffer management
- **Essential Commands**: Streamlined command interface for core operations
- **Modular Architecture**: Clean separation of concerns for maintainability

## Current Implementation Status

### Working Features

- **Spatial Interaction**:
  - Intuitive phantom positioning
  - Simple movement and arrangement
  - Drag-and-drop phantom repositioning

- **Editor Core**:
  - 3D workspace with floating code phantoms
  - Multiple phantom management (cycle with N/P keys)
  - Simplified camera controls
  - Text rendering with clear fonts
  - Efficient text buffers for editing

- **User Interface**:
  - Essential commands (:e <file>, :q)
  - Streamlined input handling
  - UTF-8 text editing with proper cursor management

- **System Architecture**:
  - Clean component architecture
  - Event system for communication
  - Memory management
  - Error handling with logging

### Interaction Controls

- **Navigation Mode**:
  - WASD/Arrow keys: Move camera
  - Mouse: Look around
  - G: Grab/move phantom
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
  - Syntax highlighting
  - Code structure visualization
  - Relationship visualization between phantoms

- **Integration**:
  - Project workspace management
  - File browser integration


