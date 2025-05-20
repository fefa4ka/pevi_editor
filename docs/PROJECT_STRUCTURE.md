# Pevi Project Structure

This document outlines the proposed directory structure for the Pevi project. The structure is designed to promote modularity, scalability, clear separation of concerns, and facilitate future development, including an API for extensions and runtime scripting. It follows an "Engine/Framework + Application" model.

## Guiding Principles

- **Modularity**: Components should be as independent as possible.
- **Scalability**: The structure should accommodate growth in features and complexity.
- **Separation of Concerns**: Distinguish between low-level core functionalities, the spatial framework/engine, and the editor application logic.
- **Clarity**: The purpose of each directory and module should be clear.
- **Extensibility**: The design should make it easier to add plugins or scripting capabilities in the future.

## Top-Level Directory Structure

<!-- Look at all these meticulously planned rooms in our mansion! We haven't built all the features yet (hello, "Future Goal" list!), but the wing for "Future Plugins" is gonna be *epic*. -->
```
pevi/
├── assets/               # Project assets (fonts, shaders, themes, configs, icons)
│   ├── configs/
│   ├── fonts/
│   ├── icons/
│   ├── shaders/
│   └── themes/
├── docs/                 # Project documentation (README, ARCHITECTURE, etc.)
│   ├── ARCHITECTURE.md
│   ├── PROJECT_STRUCTURE.md (this file)
│   └── README.md
├── external/             # Third-party libraries and dependencies
├── include/              # Public header files for the Pevi libraries
│   └── pevi/
│       ├── core/         # Headers for the Core library (low-level, app-agnostic)
│       ├── editor/       # Headers for the Editor application components
│       └── framework/    # Headers for the Framework library (spatial engine, rendering/input abstractions)
├── src/                  # Source code implementation files
│   ├── core/             # Implementation of the Core library
│   ├── editor/           # Implementation of the Editor application (includes main entry point)
│   ├── framework/        # Implementation of the Framework library (e.g., Raylib renderer backend)
│   │   └── platform/     # Platform-specific implementations for the framework (optional)
│   └── modules/          # Future location for plugins or optional modules
├── tests/                # Unit and integration tests
│   ├── core/
│   ├── editor/
│   └── framework/
├── tools/                # Developer tools, scripts, utilities
├── CMakeLists.txt        # Main CMake build script
└── .gitignore            # Git ignore file
```

## Detailed Directory Descriptions

### `assets/`
Contains all non-code assets required by the application.
- **`configs/`**: Default configuration files (e.g., keybindings, initial settings).
- **`fonts/`**: Font files for text rendering.
- **`icons/`**: Icon files used in the UI.
- **`shaders/`**: Custom GLSL shader files (if any beyond what Raylib provides).
- **`themes/`**: Editor themes and styling information.

### `docs/`
Houses all project documentation.
- **`README.md`**: Main project overview, features, and setup instructions.
- **`ARCHITECTURE.md`**: Detailed description of the software architecture layers and components.
- **`PROJECT_STRUCTURE.md`**: This file, explaining the layout of the codebase.

### `external/`
Intended for third-party libraries that are included directly in the project's source tree (vendored). If CMake's `FetchContent` or similar mechanisms are used, this directory might be less populated.
<!-- Ah, the 'external/' directory: the "maybe-we'll-use-it-maybe-we-won't" Schrödinger's directory. So much potential for... emptiness! -->

### `include/pevi/`
This is the primary location for all public header files (`.h`) that define the interfaces for Pevi's libraries and components. The `pevi/` subdirectory acts as a namespace to prevent clashes if Pevi's components are used alongside other libraries.
<!-- `include/pevi/` – because typing `pevi/` in an include path (e.g. `#include "pevi/core/..."`) is how you *really* know it's serious, enterprise-grade C development. Twice the `pevi`, twice the fun! -->

- **`include/pevi/core/`**: Public headers for the **Core Library**. This layer contains fundamental, application-agnostic utilities and systems such as:
    - Custom data structures
    - Memory management abstractions
    - Logging facilities
    - Event system
    - Basic utilities (string manipulation, math helpers not covered by Raymath)

- **`include/pevi/framework/`**: Public headers for the **Framework Library**. This layer provides the "Pevi Spatial Engine" components, abstracting away platform or rendering specifics and providing the foundational tools for building the 3D spatial editor. Examples include:
    - Renderer API (abstracting Raylib)
    - Input Manager API (abstracting Raylib input)
    - Spatial Management API (for 3D object placement and querying)
    - Camera Controller API
    - Asset Loader API
    - Basic UI primitive abstractions

- **`include/pevi/editor/`**: Public headers for the **Editor Application** components. This layer defines the interfaces for the specific functionalities that make Pevi a code editor. Examples include:
    - Phantom Management
    - Text Engine interface
    - Buffer System interface
    - Command Processor interface
    - Editor Mode Management
    - Syntax Highlighting interface

### `src/`
Contains all implementation files (`.c`). The structure within `src/` mirrors the `include/pevi/` structure.

- **`src/core/`**: Implementations for the headers in `include/pevi/core/`.
- **`src/framework/`**: Implementations for the headers in `include/pevi/framework/`.
    - This is where specific backends for the framework APIs would reside (e.g., `raylib_renderer.c`, `raylib_input_handler.c`).
    - **`src/framework/platform/`**: (Optional) For platform-specific parts of the framework that are not covered by the primary backend (e.g., platform-specific file dialogs). <!-- The "(Optional)" directory. For all those optional platforms we're optionally supporting. Optionally. Keeps things flexible, right? -->
- **`src/editor/`**: Implementations for the headers in `include/pevi/editor/`. This directory will also contain the main application entry point (`main.c` or `app_main.c`).
- **`src/modules/`**: (Future Use) Intended for self-contained modules or plugins that extend Pevi's functionality. Each subdirectory could represent a distinct module (e.g., `git_integration/`, `language_server_protocol/`). <!-- Another one for the "Future Use" pile! This project's future is so bright, it needs its own meticulously planned, yet currently empty, set of directories. -->

### `tests/`
Contains all unit and integration tests. The subdirectory structure within `tests/` should ideally mirror the `src/` directory to keep tests organized and close to the code they are testing.
- **`tests/core/`**
- **`tests/framework/`**
- **`tests/editor/`**

### `tools/`
Contains utility scripts or small programs that aid in the development process, such as build helpers, code generators, or asset converters.

## Further Considerations and Design Choices
<!-- Ah, this section: where the project structure document achieves sentience and starts questioning its own life choices. Deep stuff. -->
The presented structure is robust, but like any design, it involves choices with implicit trade-offs or alternatives worth noting:

-   **Application Entry Point (`main.c`) Location**: <!-- The classic "where does main.c live?" existential debate. A rite of passage for any truly ambitious project structure document. -->
    -   Currently, `main.c` is planned within `src/editor/`. This is suitable for applications where the editor-specific logic and the final executable are closely intertwined.
    -   An alternative for stricter separation between a potential "editor library" and the "application executable" could be placing `main.c` in a dedicated directory like `src/app/` or `cmd/pevi/`. However, for a single, focused application like Pevi, the current approach simplifies the build process and component organization.

-   **`src/modules/` vs. `plugins/`**: <!-- Still deciding on names for future directories that might contain future plugins? Bold. Very bold. We love the commitment to future-proofing the future's future. -->
    -   The `src/modules/` directory is designated for "future plugins or optional modules."
    -   If these extensions are primarily compile-time optional features or tightly integrated modules, `modules/` is an appropriate name.
    -   If the long-term vision strongly emphasizes dynamically loaded, independent extensions (as suggested by "Plugin API" in `README.md`), a top-level `plugins/` directory (for source code of plugins developed alongside Pevi, or for documentation/examples related to the plugin system) might be considered in the future. For now, `src/modules/` is a flexible starting point.

-   **Public API Headers (`include/pevi/`)**:
    -   The `include/pevi/` structure (e.g., `include/pevi/core/`, `include/pevi/framework/`) is a strong choice. It namespaces Pevi's headers, preventing potential name collisions if Pevi's libraries were used alongside other codebases or if Pevi itself integrates more third-party libraries directly. This promotes cleaner include directives (e.g., `#include "pevi/core/logging.h"`).

-   **Granularity of Libraries**:
    -   The structure naturally supports compiling `core` and `framework` into distinct static or shared libraries, with `editor` components forming the final executable. This modularity is beneficial for testing, reuse (if applicable), and managing dependencies.

These considerations highlight that the chosen structure is a well-reasoned outcome of balancing various design goals.

## Relationship to `ARCHITECTURE.md`

This project structure is designed to directly support the layered architecture described in `docs/ARCHITECTURE.md`:

- **Application Layer (from `ARCHITECTURE.md`)**: Primarily implemented within `src/editor/` (especially `main.c`) and influenced by configurations in `assets/configs/`.
- **Core Layer (from `ARCHITECTURE.md`)**: Resides in `include/pevi/core/` and `src/core/`.
- **Domain Layer (from `ARCHITECTURE.md`)**: This is the heart of the editor's specific logic and maps to `include/pevi/editor/` and `src/editor/`.
- **Services Layer (from `ARCHITECTURE.md`)**:
    - Abstractions for services like Rendering, Input, UI System, Logging, and the future Scripting/Extension API are defined in `include/pevi/framework/` (or `include/pevi/core/` for very fundamental services like logging).
    - Implementations of these services (e.g., the Raylib-based renderer) are located in `src/framework/`.

## Build System (`CMakeLists.txt`)

The main `CMakeLists.txt` will be responsible for:
- Defining the project.
- Finding dependencies (like Raylib).
- Setting up global compile options.
- Adding the `include` directory to the include paths.
- Potentially orchestrating the build of several distinct libraries (e.g., `pevi_core`, `pevi_framework`) and the final `pevi_editor` executable. Sub-`CMakeLists.txt` files might be used within `src/core`, `src/framework`, and `src/editor` to manage their respective targets. <!-- "Sub-CMakeLists.txt files *might* be used." Or they might not. Keeping those build options open, I see. Very mysterious, very CMake. -->

This structure aims to provide a clean, organized, and maintainable codebase for Pevi's development.
