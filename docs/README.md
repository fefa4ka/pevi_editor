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
  - `WASD` / `Arrow keys`: Move camera.
  - `Mouse Look`: Control camera orientation.
  - `Mouse Click` (on a phantom): Select/focus on the phantom.
  - `Mouse Drag` (on a phantom): Click, hold, and drag to reposition the phantom.
  - `G` (while focused on a phantom): Grab the phantom. Move mouse to reposition, click to release.
  - `N` / `P`: Cycle focus between phantoms.
  - `I`: Enter Edit Mode for the currently focused phantom.
  - `:`: Enter Command Mode.

- **Edit Mode** (when a phantom is focused and 'I' is pressed):
  - Standard text input for inserting code.
  - `Backspace` / `Delete`: Remove characters.
  - `Enter`: Insert a new line.
  - `Arrow Keys`: Navigate text.
  - `Mouse Click`: Position cursor.
  - `Mouse Drag`: Select text.
  - `Shift + Arrow Keys`: Select text.
  - `Ctrl+C` / `Cmd+C`: Copy selected text.
  - `Ctrl+X` / `Cmd+X`: Cut selected text.
  - `Ctrl+V` / `Cmd+V`: Paste text from clipboard.
  - `Ctrl+Z` / `Cmd+Z`: Undo last action.
  - `Ctrl+Y` / `Cmd+Y` (or `Ctrl+Shift+Z` / `Cmd+Shift+Z`): Redo last undone action.
  - `Mouse Wheel`: Scroll through text within the phantom.
  - `ESC`: Return to Navigation Mode.
  - `Ctrl+S`: Save the content of the current phantom (if associated with a file).
  - `Ctrl+Q`: Quit the application.

- **Command Mode** (accessed by pressing `:` from Navigation Mode):
  - Access essential editor functions via a simple command line interface.
  - `:e <filepath>`: Open a file into a new phantom. If the file exists, its content is loaded.
  - `:w` or `:write`: Save the current phantom to its associated file.
  - `:w <filepath>` or `:saveas <filepath>`: Save the current phantom to the specified filepath and associate it with this file.
  - `:new`: Create a new, empty phantom (not associated with any file initially).
  - `:d` or `:delete`: Delete the currently focused phantom.
  - `:q` or `:quit`: Quit the application.
  - `ESC`: Return to Navigation Mode.

### Visualization Capabilities
- **Syntax Highlighting**: Enhance code readability with language-specific syntax coloring.
- **Code Structure Visualization**: (Future Goal) Visually represent the structure within and between code phantoms.
- **Relationship Visualization**: (Future Goal) Illustrate dependencies and connections between different phantoms.

### Project & System
- **Modular Design**: Built with a clean, modular architecture for maintainability and extensibility. For more details, see [Pevi Editor Architecture](ARCHITECTURE.md).
- **Workspace Management**: (Future Goal) Manage collections of files and phantoms as projects.
- **File Browser Integration**: (Future Goal) Navigate and open files from an integrated file browser.


