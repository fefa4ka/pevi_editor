# Claude Development Guide for Pevi

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
- **Current State**: ✅ **3D TEXT RENDERING FIXED** - All compilation, runtime, and rendering issues resolved
- **Repository**: ✅ **FULLY CLEANED** - All build artifacts removed, proper .gitignore, 29 commits synced to remote

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

## Recent Fixes Applied
- ✅ Fixed observer registration syntax by replacing compound literals with explicit descriptor initialization
- ✅ Fixed component registration by using ECS_COMPONENT_DEFINE instead of ECS_COMPONENT
- ✅ Fixed tag registration by using ECS_TAG_DEFINE instead of ECS_TAG  
- ✅ Fixed component hook conflicts by using ecs_set_hooks instead of ecs_component
- ✅ Fixed entity naming conflict by renaming "EditorState" entity to "Editor"
- ✅ Resolved "observer must have at least one term" error completely
- ✅ **NEW**: Fixed InputSystem and PickingSystem query mismatch - systems now use singleton pattern to access Editor and MainCamera entities
- ✅ **NEW**: Fixed system registration and removed problematic pipeline phases
- ✅ **NEW**: Navigation, camera control, and input now working properly in spatial_editor example
- ✅ **LATEST**: Fixed 3D text rendering by moving from TextRenderSystem to main render loop with proper camera context
- ✅ **LATEST**: Optimized text rendering query creation - created once at initialization instead of every frame
- ✅ **LATEST**: Implemented billboard-style 3D text rendering with background for better visibility

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

## Development Workflow
1. **MANDATORY**: Commit after every file modification, addition, or deletion
2. Follow the layered architecture (Core → Framework → Editor)
3. Implement ECS components using Flecs best practices (see docs/ECS_FLECS.md)
4. Use atomic components and proper query design
5. Maintain clear separation between layers

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

## Key Implementation Areas
- ✅ Core ECS setup with Flecs
- ✅ Basic phantom entity creation and management
- ✅ Camera controller implementation
- ✅ 3D text rendering system (billboard-style with proper camera context)
- ✅ Input handling for navigation mode
- [ ] Input handling for edit and command modes
- [ ] File I/O integration
- [ ] Command system implementation

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
