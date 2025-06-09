# Claude Development Guide for Pevi

## Quick Reference
Pevi is a 3D spatial code editor with phantoms (floating 3D code objects). Uses C11, CMake, Raylib, JoltC physics, Flecs ECS, ImGui, and lr_text buffers.

## Key Development Info
- **Entry Point**: `src/main.c` (currently minimal Raylib window)
- **Build**: `cmake --build build && ./build/editor`
- **Current State**: ✅ **BUILD SUCCESSFUL** - All dependencies working, ready for ECS implementation

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

## Key Implementation Areas
- [ ] Core ECS setup with Flecs
- [ ] Basic phantom entity creation and management
- [ ] Camera controller implementation
- [ ] Text rendering system
- [ ] Input handling for different modes
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
