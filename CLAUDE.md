# Claude Development Guide for Pevi

## Project Overview
Pevi is a 3D spatial code editor that represents code as "phantoms" (floating 3D objects) in a three-dimensional space. This allows for innovative spatial organization and visualization of code relationships.

## Core Technologies
- **Language**: C11
- **Build System**: CMake  
- **Graphics**: Raylib (rendering, windowing, input)
- **Physics**: JoltC (physics engine)
- **ECS**: Flecs (Entity Component System)
- **UI**: Dear ImGui (via cimgui)
- **Text Processing**: lr_text library (linked ring buffers)
- **Math**: Raymath (vector/matrix operations)

## Project Structure Key Points
- `include/pevi/` - Public headers organized by layer (core/, framework/, editor/)
- `src/` - Implementation files mirroring include structure
- `src/main.c` - Application entry point (currently minimal Raylib window)
- `assets/` - Project assets (fonts, shaders, themes, configs)
- `docs/` - Architecture documentation

## Architecture Layers
1. **Core Layer** (`include/pevi/core/`, `src/core/`)
   - Component framework, event system, memory management
   - Basic utilities, logging, spatial management

2. **Framework Layer** (`include/pevi/framework/`, `src/framework/`)
   - Renderer API (abstracting Raylib)
   - Input Manager, Spatial Management, Camera Controller
   - Asset Loader, UI primitive abstractions

3. **Editor Layer** (`include/pevi/editor/`, `src/editor/`)
   - Phantom Management, Text Engine, Buffer System
   - Command Processor, Editor Mode Management
   - Syntax Highlighting

## ECS Architecture (Flecs)
### Key Entity Types:
- **Editor**: Singleton with navigation/edit/command modes
- **Camera**: Position, rotation, camera settings
- **Phantom**: Atomic components (Position, PhantomSize, TextContent, FileInfo)

### Component Design Principles:
- **Atomic components** over large composite ones
- Use **tags** for boolean states (Visible, Focused, Selected)
- **Relationships** for hierarchies (ChildOf) and references
- **Prefabs** for entity templates

### Current Mode System:
- NavigationMode (default): WASD camera, mouse look, phantom selection
- EditMode: Text editing within focused phantom
- CommandMode: Vim-like command interface (:e, :w, :q)

## Dependencies Status
Current CMakeLists.txt fetches:
- joltc (physics)
- cimgui (UI)
- lr_text (text buffers) 
- raylib, flecs, libuv, glfw3, freetype (system packages)

## Current State
- Basic Raylib window setup in `src/main.c`
- Complete project structure defined
- Architecture documentation in place
- Build system configured with all dependencies
- Ready for component implementation

## Development Workflow
1. Each development iteration should be committed to git
2. Follow the layered architecture (Core → Framework → Editor)
3. Implement ECS components using Flecs best practices
4. Use atomic components and proper query design
5. Maintain clear separation between layers

## Key Implementation Areas
- [ ] Core ECS setup with Flecs
- [ ] Basic phantom entity creation and management
- [ ] Camera controller implementation
- [ ] Text rendering system
- [ ] Input handling for different modes
- [ ] File I/O integration
- [ ] Command system implementation

## Build Commands
```bash
cmake -B build
cmake --build build
./build/editor
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
