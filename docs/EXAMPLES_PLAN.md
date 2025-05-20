# Pevi - Example-Driven Development Plan

## 1. Introduction

This document outlines a plan for **Example-Driven Development (EDD)** for the Pevi project. The goal of EDD is to create small, focused, and self-contained example programs to test, understand, and validate individual technologies, libraries, and core concepts before integrating them into the main Pevi application.

This approach helps to:
- Isolate and solve problems early.
- Understand the capabilities and limitations of chosen libraries.
- Provide clear, working demonstrations of specific functionalities.
- Reduce complexity during the integration phase.
- Serve as a learning resource and a testbed for new ideas.

## 2. Core Concepts & Technologies to Test via Examples

The following areas are critical to Pevi's functionality and will benefit from dedicated examples:

1.  **Flecs (ECS - Entity Component System)**: For managing phantoms, their properties, and behaviors.
2.  **Raylib**: For 3D rendering, windowing, input, and basic UI.
3.  **`lr_text` (Text Engine)**: For text buffer management and manipulation.
4.  **Input Handling**: Capturing and processing keyboard and mouse input for navigation, interaction, and text editing.
5.  **Phantom Representation**: Combining ECS entities, 3D visuals, and text content.
6.  **Command System**: Parsing and executing user commands.
7.  **File I/O**: Loading and saving phantom content.
8.  **CImGui**: For creating debug UIs and potentially some editor UI elements.
9.  **Jolt Physics (`joltc`)**: (Optional, for advanced spatial interactions like collision)
10. **Libuv**: (Optional, for asynchronous operations, e.g., language servers, file watching)

## 3. Proposed Examples

Examples should be created in the `examples/` directory. Each example should be a standalone `.c` file and compiled via `examples/CMakeLists.txt`.

### 3.1. Flecs ECS Examples

*   **`flecs_basic_entity_component.c`**:
    *   **Goal**: Understand basic Flecs entity creation, component definition (structs), adding components to entities, and retrieving component data.
    *   **Steps**:
        1. Initialize a Flecs world.
        2. Define simple components (e.g., `Position {float x, y, z;}`, `Velocity {float dx, dy, dz;}`).
        3. Create a few entities.
        4. Add components to these entities with initial values.
        5. Iterate over entities with `Position` and print their coordinates.
*   **`flecs_simple_system.c`**:
    *   **Goal**: Implement a basic system that modifies component data.
    *   **Steps**:
        1. Building on `flecs_basic_entity_component.c`.
        2. Define a system that iterates entities with `Position` and `Velocity`.
        3. The system updates `Position` based on `Velocity` and a delta time.
        4. Run `ecs_progress()` in a loop and print updated positions.
*   **`flecs_tag_query.c`**:
    *   **Goal**: Understand how to use tags and query for entities with specific tags or combinations of components/tags.
    *   **Steps**:
        1. Define a tag component (e.g., `IsVisible`).
        2. Create entities, some with `IsVisible` tag, some without.
        3. Write a query/system to iterate only over entities that have `Position` AND `IsVisible`.
*   **`flecs_hierarchy_example.c`** (Already exists, can be used as a reference for parent-child relationships if needed for phantom grouping).

### 3.2. Raylib Examples

*   **`raylib_3d_scene.c`**:
    *   **Goal**: Setup a basic 3D scene with a camera and simple shapes.
    *   **Steps**:
        1. Initialize Raylib window.
        2. Setup a 3D camera (e.g., `Camera3D` with perspective projection).
        3. Implement basic camera controls (e.g., orbit around a point, or WASD movement).
        4. Draw simple 3D primitives (e.g., `DrawCube`, `DrawSphere`).
        5. Basic lighting.
*   **`raylib_3d_text.c`**:
    *   **Goal**: Render text in 3D space.
    *   **Steps**:
        1. Building on `raylib_3d_scene.c`.
        2. Load a font (`LoadFont`).
        3. Use `DrawText3D` or `DrawTextCodepoint3D` to render text at a specific 3D position.
        4. Experiment with font size and text alignment.
*   **`raylib_billboard_text.c`**:
    *   **Goal**: Make 3D text always face the camera (billboarding).
    *   **Steps**:
        1. Building on `raylib_3d_text.c`.
        2. Implement billboarding for text objects (using `DrawBillboard` or manual matrix transformation).
*   **`raylib_mouse_picking_3d.c`**:
    *   **Goal**: Select 3D objects using mouse clicks.
    *   **Steps**:
        1. Building on `raylib_3d_scene.c` (with multiple objects).
        2. Implement raycasting from mouse cursor into the 3D scene (`GetMouseRay`).
        3. Check for intersections between the ray and object bounding boxes (`GetRayCollisionBox`).
        4. Highlight or identify the selected object.

### 3.3. `lr_text` (Text Engine) Examples

*   **`lr_text_basic_buffer.c`**:
    *   **Goal**: Understand `lr_text` buffer creation, text insertion, and deletion.
    *   **Steps**:
        1. Include `lr_text.h`.
        2. Create an `lr_text_buffer`.
        3. Insert some sample text (lines, characters).
        4. Delete some text.
        5. Iterate through the buffer content and print it to the console.
        6. Free the buffer.
*   **`lr_text_render_with_raylib.c`**:
    *   **Goal**: Display content from an `lr_text_buffer` using Raylib (2D text rendering for simplicity first).
    *   **Steps**:
        1. Combine `lr_text_basic_buffer.c` concepts with Raylib window.
        2. Iterate through the `lr_text_buffer`.
        3. For each line/character, use Raylib's `DrawText` to render it on screen.
        4. Handle basic scrolling if text exceeds window height.

### 3.4. Input Handling Examples

*   **`raylib_keyboard_input_modes.c`**:
    *   **Goal**: Implement a simple state machine for different input modes (e.g., Navigation, Typing).
    *   **Steps**:
        1. Define an enum for modes.
        2. In Navigation mode, WASD keys move a conceptual cursor/camera.
        3. Pressing 'I' switches to Typing mode.
        4. In Typing mode, key presses append to a string buffer.
        5. Pressing 'ESC' switches back to Navigation mode.
        6. Display current mode and typed text on screen.
*   **`raylib_mouse_drag_3d.c`**:
    *   **Goal**: Implement dragging of a 3D object with the mouse.
    *   **Steps**:
        1. Combine `raylib_mouse_picking_3d.c`.
        2. When an object is clicked (selected), store its initial state.
        3. While mouse button is held and moved, update the object's position based on mouse movement projected onto a plane or sphere.

### 3.5. Phantom Representation & Interaction Examples

*   **`flecs_raylib_lr_text_phantom.c`**:
    *   **Goal**: Create a single "phantom" represented by a Flecs entity, visualized by Raylib, containing text from `lr_text`.
    *   **Steps**:
        1. **Flecs**: Define an entity. Components: `Position`, `Rotation`, `Dimensions`, `TextBufferRef (pointer to lr_text_buffer)`.
        2. **`lr_text`**: Create a text buffer and populate it. Store its pointer in the `TextBufferRef` component.
        3. **Raylib**:
            *   In a Flecs system (or main loop iterating entities):
                *   Get `Position`, `Dimensions` for the phantom.
                *   Draw a 3D box (the phantom's body) at `Position` with `Dimensions`.
                *   Get `TextBufferRef`, access the `lr_text_buffer`.
                *   Render the text content onto the phantom's surface (using `raylib_3d_text.c` or `raylib_billboard_text.c` techniques).
*   **`phantom_focus_grab.c`**:
    *   **Goal**: Implement focusing on a phantom and then "grabbing" it to move.
    *   **Steps**:
        1. Building on `flecs_raylib_lr_text_phantom.c` (with multiple phantoms).
        2. Use `raylib_mouse_picking_3d.c` to select a phantom.
        3. When selected, add a `IsFocused` tag component to its Flecs entity (remove from others).
        4. Visually indicate focus (e.g., change color).
        5. If 'G' is pressed while a phantom is focused, add an `IsGrabbed` tag.
        6. While `IsGrabbed`, use `raylib_mouse_drag_3d.c` logic to update its `Position` component.
        7. Clicking again or pressing 'G' again releases the grab (removes `IsGrabbed` tag).

### 3.6. Command System Examples

*   **`simple_command_parser.c`**:
    *   **Goal**: Parse basic string commands into a command and arguments.
    *   **Steps**:
        1. Read a line of text (e.g., from console input or a hardcoded string).
        2. Split the string into a command token and argument tokens (e.g., ":e myfile.txt" -> cmd="e", args=["myfile.txt"]).
        3. Print the parsed command and arguments.
*   **`command_executor_flecs.c`**:
    *   **Goal**: Execute parsed commands that interact with a Flecs world.
    *   **Steps**:
        1. Combine `simple_command_parser.c` with a basic Flecs setup.
        2. Implement handlers for commands like:
            *   `:new_phantom x y z` -> creates a new entity with `Position {x,y,z}`.
            *   `:list_phantoms` -> iterates entities with `Position` and prints their ID and position.

### 3.7. File I/O Examples

*   **`file_to_lr_text.c`**:
    *   **Goal**: Load content from a text file into an `lr_text_buffer`.
    *   **Steps**:
        1. Prompt for a filename or use a hardcoded one.
        2. Open and read the file line by line.
        3. Insert each line into an `lr_text_buffer`.
        4. Print the buffer content to verify.
*   **`lr_text_to_file.c`**:
    *   **Goal**: Save content from an `lr_text_buffer` to a text file.
    *   **Steps**:
        1. Populate an `lr_text_buffer` (manually or from `file_to_lr_text.c`).
        2. Prompt for a filename.
        3. Iterate through the `lr_text_buffer` and write its content to the file.

### 3.8. CImGui Examples (Optional - for Debug UI)

*   **`cimgui_hello_world.c`**:
    *   **Goal**: Integrate CImGui with Raylib to show a simple debug window.
    *   **Steps**:
        1. Setup Raylib window.
        2. Initialize CImGui (using a Raylib backend like `rlImGui`).
        3. In the game loop, start a new ImGui frame.
        4. Create a simple ImGui window (`igBegin`, `igText`, `igEnd`).
        5. Render ImGui draw data.
*   **`cimgui_flecs_inspector.c`**:
    *   **Goal**: Display information about Flecs entities and components in an ImGui window.
    *   **Steps**:
        1. Combine `cimgui_hello_world.c` with a Flecs setup.
        2. Iterate through Flecs entities.
        3. For each entity, display its ID and list its components and their values in the ImGui window.

### 3.9. Jolt Physics Examples (Optional - Advanced)

*   **`jolt_basic_collision.c`**: (Similar to existing `joltc.c` but simplified)
    *   **Goal**: Setup a minimal Jolt physics scene with a few bodies and detect collisions.
    *   **Steps**:
        1. Initialize Jolt.
        2. Create a floor and a few dynamic bodies (e.g., spheres, boxes).
        3. Step the physics simulation.
        4. Implement a contact listener to print when collisions occur.

### 3.10. Libuv Examples (Optional - Advanced)

*   **`libuv_simple_timer.c`**:
    *   **Goal**: Use libuv to run a simple asynchronous timer.
    *   **Steps**:
        1. Initialize a libuv loop.
        2. Create a `uv_timer_t`.
        3. Start the timer to fire after a few seconds.
        4. In the timer callback, print a message.
        5. Run the libuv loop.

## 4. Example Structure and Build Process

*   All examples will reside in the `examples/` directory.
*   Each example will be a single `.c` file (e.g., `examples/flecs_basic_entity_component.c`).
*   The `examples/CMakeLists.txt` file will be updated to add an executable target for each new example.
    ```cmake
    # In examples/CMakeLists.txt
    # For a new example named my_new_example.c
    add_executable(my_new_example my_new_example.c)
    target_link_libraries(my_new_example PRIVATE
        raylib     # If using Raylib
        flecs::flecs_static # If using Flecs
        # ... other necessary libraries (lr_text, joltc, libuv, cimgui)
    )
    set_target_properties(my_new_example PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
    )
    install(TARGETS my_new_example DESTINATION bin/examples)
    ```

## 5. Development Workflow using Examples

1.  **Identify Concept**: Choose a specific feature or technology to explore (e.g., rendering 3D text).
2.  **Create Example**: Write a new, minimal `.c` file in `examples/` focusing solely on that concept.
3.  **Build & Test**: Add it to `examples/CMakeLists.txt`, build, and run the example. Iterate until the concept is working correctly and understood.
4.  **Learn & Document**: Note any key learnings, challenges, or useful patterns.
5.  **Integrate**: Once the concept is proven in the example, integrate the knowledge and relevant code patterns into the main Pevi application codebase (`src/`).
6.  **Refactor**: Adapt the example code to fit the Pevi architecture and coding standards.
7.  **Commit**: Commit the example (for future reference) and the integrated feature in the main application.

This example-driven approach will provide a solid foundation for building Pevi incrementally and robustly.
