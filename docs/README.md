# Pevi - 3D Spatial Code Editor

Pevi is an innovative code editor that operates in 3D space. Unlike traditional text editors confined to 2D windows, Pevi allows code to be visualized and manipulated in a three-dimensional environment where code blocks can be spatially arranged to represent relationships.

## Core Concept

Pevi reimagines code editing by representing code as "phantoms" - objects that float in 3D space. These phantoms can be:

- Positioned and arranged spatially
- Organized to visualize relationships between code components
- Moved and grouped to create meaningful layouts
- Edited with familiar text editing capabilities

This spatial approach to code editing provides a unique perspective that can enhance understanding of complex codebases and create more intuitive mental models of code structure.

## Features

Pevi offers a range of features designed to enhance the coding experience in a 3D environment:

### Spatial Organization & Interaction
- **Visualize Relationships**: Arrange code spatially to understand connections between components.
- **Intuitive Manipulation**: Position, move, and group code "phantoms" (floating 3D objects) with ease.
- **Drag-and-Drop**: Reposition phantoms using simple drag-and-drop actions.
- **3D Workspace**: Navigate your codebase within a fully three-dimensional environment.

### Advanced Editor Core
- **Phantom-Based Editing**: Code is represented as "phantoms," each potentially holding different sections of files or new unsaved buffers.
- **Multiple Phantom Management**: Easily cycle through active phantoms (`N`/`P` keys).
- **Efficient Text Engine**: Robust UTF-8 support with efficient text buffer management for responsive editing.
- **Simplified Camera Controls**: Navigate the 3D space with straightforward camera movements.
- **Clear Text Rendering**: Code is displayed with clear, legible fonts.

### User Interface & Interaction Modes
Pevi provides a streamlined interface with distinct modes for different tasks:

- **Navigation Mode**:
  - `WASD` / `Arrow keys`: Move camera
  - `Mouse`: Look around
  - `G`: Grab/move the currently focused phantom
  - `N` / `P`: Cycle focus between phantoms
  - `I`: Enter Edit Mode for the current phantom
  - `:`: Enter Command Mode

- **Edit Mode**:
  - Standard text input for inserting code.
  - `Backspace` / `Delete`: Remove characters.
  - `Enter`: Insert a new line.
  - `ESC`: Return to Navigation Mode.
  - `Ctrl+S`: Save the content of the current phantom (if associated with a file).
  - `Ctrl+Q`: Quit the application.

- **Command Mode**:
  - Access essential editor functions via a simple command line interface.
  - `:e <file>`: Open a file in a new phantom.
  - `:new`: Create a new, empty phantom.
  - `:d`: Delete the current phantom.
  - `:q`: Quit the application.

### Visualization Capabilities
- **Syntax Highlighting**: Enhance code readability with language-specific syntax coloring.
- **Code Structure Visualization**: (Future Goal) Visually represent the structure within and between code phantoms.
- **Relationship Visualization**: (Future Goal) Illustrate dependencies and connections between different phantoms.

### Project & System
- **Modular Design**: Built with a clean, modular architecture for maintainability and extensibility. For more details, see [Pevi Editor Architecture](ARCHITECTURE.md).
- **Workspace Management**: (Future Goal) Manage collections of files and phantoms as projects.
- **File Browser Integration**: (Future Goal) Navigate and open files from an integrated file browser.


