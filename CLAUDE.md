# Claude Development Guide for Pevi
**Updated: 2025-06-27 by Project Manager**

## Quick Reference
Pevi is a 3D spatial code editor with phantoms (floating 3D code objects) that represent semantic parts of code arranged by mnemonic sense. A single file is edited through multiple phantoms, each displaying functions, classes, or logical code blocks positioned in 3D space for intuitive navigation and understanding. Uses C11, CMake, Raylib, JoltC physics, Flecs ECS, ImGui, and lr_text buffers.

## Mnemonic Sense Definition
**Mnemonic sense** refers to the spatial arrangement of code phantoms based on how developers naturally think about and remember code relationships:

### Spatial Memory Principles
- **Call Flow**: Functions that call each other are positioned closer together
- **Data Flow**: Variables and their usage points form spatial clusters
- **Logical Hierarchy**: Parent-child relationships (classes/methods, modules/functions) use vertical positioning
- **Temporal Sequence**: Code execution order influences left-to-right or front-to-back arrangement
- **Conceptual Grouping**: Related functionality (error handling, initialization, utilities) forms spatial neighborhoods

### Examples of Mnemonic Arrangements
```
3D Space Layout Example:
┌─────────────────────────────────────┐
│ High Level (Y+)                     │
│   [main()]                          │
│      │                              │
│   [init_systems()]  [cleanup()]     │
│      │                   │          │
│ Mid Level                │          │
│   [create_window()]      │          │
│   [setup_physics()]      │          │
│      │                   │          │
│ Low Level (Y-)           │          │
│   [malloc_wrapper()]     │          │
│   [error_handler()]──────┘          │
│                                     │
│ Left: Core Logic    Right: Utilities│
└─────────────────────────────────────┘
```

### Memory-Based Positioning Rules
1. **Vertical Axis (Y)**: Abstraction level (high-level functions up, low-level down)
2. **Horizontal Axis (X)**: Functional domains (core left, utilities right, I/O far right)
3. **Depth Axis (Z)**: Execution frequency (hot paths close, cold paths distant)
4. **Proximity**: Functions that developers mentally associate are spatially close
5. **Visual Paths**: Code flow creates visual "roads" through 3D space that match mental models

## Key Development Info
- **Entry Point**: `src/main.c` (currently minimal Raylib window)
- **Build**: `cmake --build build && ./build/editor`
- **Current State**: ✅ **BUILD SYSTEM FULLY WORKING** - All compilation, runtime, and dependency issues resolved
- **Repository**: ✅ **FULLY CLEANED** - All build artifacts removed, proper .gitignore, clean commits synced to remote

## Critical Issues Status (Updated 2025-06-27)
### 🚨 Open Blocking Issues - Immediate Developer Assignment Needed
- **Issue #8**: MSDF 3D Text Rendering Completely Broken (CRITICAL - blocks core editor functionality)
- **Issue #10**: ECS Component Registration and System Execution Failures (HIGH - core architecture broken)

### ✅ Resolved Issues
- **Issue #5 CLOSED**: Manual Testing Build System - All 5 test cases passed successfully
- **Issue #6 CLOSED**: Git Repository Cleanup - Repository fully cleaned and synced with remote
- **Issue #7 MERGED**: Build System PR - Successfully merged with all fixes applied
- **Issues #2, #3, #4 CLOSED**: Manual testing issues closed - converted to specific bug issues above

## Urgent Developer Actions Required
1. **Text Rendering Expert** needed for Issue #8 - Without 3D text, code phantoms cannot display
2. **Input System Developer** needed for Issue #9 - Without input, editor is unusable
3. **ECS Specialist** needed for Issue #10 - Core architecture validation failing

**Impact**: All three issues block core editor functionality development

## Build System Status
- ✅ **CMake Configuration Working** - Automatic vcpkg dependency discovery implemented
- ✅ **Cross-Platform Support** - arm64-osx, x64-osx, x64-linux, x64/x86-windows triplets supported
- ✅ **All Dependencies Found** - raylib, flecs, libuv, glfw3, freetype automatically discovered
- ✅ **Clean Build Process** - `mkdir build && cd build && cmake .. && cmake --build .` works from scratch
- ✅ **All Targets Build** - Main editor (992KB) + 12 examples compile successfully
- ✅ **Runtime Verified** - Editor launches with Raylib 5.5 + OpenGL/Metal rendering confirmed

## Manual Testing Results - Issue #5
- ✅ **Test Case 1**: Complete clean build from scratch - **SUCCESS**
- ✅ **Test Case 2**: Dependency resolution - **SUCCESS** (all vcpkg packages found)
- ✅ **Test Case 3**: CMake configuration - **SUCCESS** (automatic triplet detection working)
- ✅ **Test Case 4**: Examples build - **SUCCESS** (all 12 examples compile and link)
- ✅ **Test Case 5**: Build artifacts - **SUCCESS** (repository remains clean)
- ✅ **Critical Issues**: All build system dependency discovery problems **RESOLVED**

## Repository Cleanup Status
- ✅ **Clean Working Directory**: No tracked build artifacts or uncommitted files
- ✅ **Proper .gitignore**: Excludes build/, vcpkg_installed/, *.o, *.a, *.so, CMake files
- ✅ **29 Commits Pushed**: All local development history synced to origin/master
- ✅ **Develop Branch Created**: Ready for feature branch workflow
- ⚠️ **Branch Protection Needed**: Repository owner must enable protection via GitHub settings
- ✅ **28MB Dependencies Untracked**: vcpkg libraries removed from version control

## ECS Examples Status
- ✅ `build/examples/ecs_complete/spatial_editor_simple` - Simple ECS demo works
- ✅ `build/examples/ecs_complete/spatial_editor` - **FULLY FIXED!** Complete ECS example now runs successfully with navigation and input working
- ✅ `build/editor` - Main editor executable builds and runs (basic Raylib window)

## ECS Development Guidelines

### Component Registration Pattern
```c
// In header file (.h)
ECS_COMPONENT_DECLARE(ComponentName);
extern ECS_DECLARE(TagName);

// In implementation file (.c)
ECS_COMPONENT_DEFINE(world, ComponentName);
ECS_TAG_DEFINE(world, TagName);
```

### Observer Registration Pattern
```c
// Use explicit descriptor initialization, NOT compound literals
ecs_observer_desc_t observer_desc = {0};
observer_desc.query.terms[0].id = ecs_id(ComponentName);
observer_desc.events[0] = EcsOnSet;
observer_desc.callback = CallbackFunction;
ecs_observer_init(world, &observer_desc);
```

### Component Hooks Pattern
```c
// Set hooks AFTER component registration
ecs_set_hooks(world, ComponentName, {
    .dtor = ComponentName_dtor
});
```

## Development Workflow
1. **MANDATORY**: Create GitHub issues for all non-trivial work
2. **MANDATORY**: All code changes go through Pull Requests to master
3. **MANDATORY**: No direct commits to master branch
4. Follow the layered architecture (Core → Framework → Editor)
5. Implement ECS components using Flecs best practices
6. Use atomic components and proper query design
7. Maintain clear separation between layers

## Development Focus Areas
### ✅ Working Systems
- Core ECS setup with Flecs
- Basic phantom entity creation and management
- Camera controller implementation
- 3D text rendering system (billboard-style with proper camera context)
- Input handling for navigation mode (in examples)
- Build system and dependency management
- Repository management and workflow

### 🚨 Critical Issues (Need Immediate Work)
- **Issue #8**: MSDF text rendering broken - blocks code phantom display
- **Issue #9**: Main editor input system broken - blocks all interaction
- **Issue #10**: ECS component registration failures - breaks core architecture

### 🔄 Next Development Phase (After Critical Issues Fixed)
- Input handling for edit and command modes
- File I/O integration
- Command system implementation
- Multiple phantom management
- Code parsing and phantom generation

## Naming Conventions

### File Naming
- Headers: `snake_case.h` in appropriate `include/pevi/` subdirectory
- Implementation: `snake_case.c` in corresponding `src/` subdirectory
- Follow the three-layer structure for all new components

### Method Naming
Use the pattern: `<object>_<action>` or `<object>_<subobject>_<action>`

**Examples:**
```c
// Basic object actions
phantom_create()
phantom_destroy()
phantom_update()
camera_move()
camera_rotate()
editor_init()

// Subobject actions
phantom_text_content_get()
phantom_text_content_set()
phantom_position_update()
phantom_size_get()
camera_settings_apply()
editor_mode_set()
editor_config_load()

// Complex operations
phantom_file_content_load()
phantom_text_selection_clear()
camera_target_phantom_set()
editor_command_processor_execute()
```

**Naming Rules:**
- Use snake_case for all identifiers
- Object comes first (phantom, camera, editor, etc.)
- Subobject is optional middle component (text, position, settings, etc.)
- Action is always last (get, set, create, destroy, update, etc.)
- Be consistent with action verbs across the codebase

### Struct Naming
Use PascalCase for struct type names, snake_case for instances.

**Struct Type Examples:**
```c
// Core structs
typedef struct {
    float x, y, z;
} Position;

typedef struct {
    float pitch, yaw, roll;
} Rotation;

typedef struct {
    float width, height;
} PhantomSize;

// Component structs
typedef struct {
    char* content;
    size_t length;
    size_t capacity;
} TextContent;

typedef struct {
    char* filepath;
    bool is_dirty;
    time_t last_modified;
} FileInfo;

// Complex structs
typedef struct {
    Position position;
    Rotation rotation;
    PhantomSize size;
    TextContent text;
    FileInfo* file_info;  // Optional
} PhantomData;

typedef struct {
    Position position;
    Rotation rotation;
    float fov;
    float near_plane;
    float far_plane;
} CameraSettings;
```

**Instance Naming Examples:**
```c
// Simple instances
Position phantom_position;
Rotation camera_rotation;
PhantomSize default_size;

// Descriptive instances
TextContent buffer_content;
FileInfo current_file;
CameraSettings main_camera;

// Multiple instances
Position phantom_positions[MAX_PHANTOMS];
PhantomData active_phantoms[MAX_ACTIVE];
CameraSettings camera_presets[NUM_PRESETS];

// Usage in functions
void phantom_position_set(EntityID phantom_id, Position new_position);
Position phantom_position_get(EntityID phantom_id);
void camera_settings_apply(CameraSettings settings);
```

**Struct Naming Rules:**
- **Types**: PascalCase (Position, TextContent, PhantomData)
- **Instances**: snake_case, descriptive names (phantom_position, buffer_content)
- **Arrays**: plural form (phantom_positions, camera_presets)
- **Pointers**: same as instances, but consider nullability in naming
- **Temporary/local**: short names acceptable (pos, rot, size)
- **Global/static**: full descriptive names required

## Debugging with LLDB

### Available Examples for Debugging
- `build/examples/flecs_basic_entity_component` - Basic ECS entity creation
- `build/examples/raylib_example` - Basic Raylib window example
- `build/editor` - Main editor executable
- `debug_demo` - Custom debugging demo with entities and components

### Basic LLDB Commands
```bash
# Start LLDB and load program
lldb build/examples/flecs_basic_entity_component

# Set breakpoints
(lldb) breakpoint set --name main
(lldb) breakpoint set --file main.c --line 20

# Run and step through
(lldb) run
(lldb) next     # Step over
(lldb) step     # Step into
(lldb) continue # Continue execution

# Examine variables and memory
(lldb) frame variable          # Show local variables
(lldb) print variable_name     # Print specific variable
(lldb) print *struct_pointer   # Examine struct contents

# Call stack and threads
(lldb) backtrace              # Show call stack
(lldb) thread list            # List all threads
(lldb) frame select 0         # Select stack frame

# Memory examination
(lldb) memory read address    # Examine memory
(lldb) disassemble           # Show assembly code
(lldb) register read         # Show CPU registers

# Watchpoints
(lldb) watchpoint set variable var_name  # Watch for changes

# Advanced
(lldb) expression expr       # Evaluate expressions
(lldb) process info         # Process information
```

### Debugging Workflow Example
1. Compile with debug symbols: `gcc -g -o program source.c`
2. Load in LLDB: `lldb program`
3. Set breakpoints at key functions
4. Run and step through code
5. Examine ECS world state, entities, and components
6. Use watchpoints to track data changes
7. Analyze memory layout and assembly when needed

## Project Management Guidelines
- **Main Goal**: Keep developers unblocked and productive
- **Issue Management**: Focus on actionable, specific issues
- **PR Flow**: Quick reviews and merges to prevent stale work
- **No Waste**: Delete unnecessary documentation and close irrelevant issues
- **Team Coordination**: Assign clear ownership to prevent conflicts
- **Priority**: Critical bugs first, features second
