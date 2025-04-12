# Pevi Editor Architecture

## Overview
A refined architecture designed for a 3D code editor, focusing on modularity, performance, and extensibility:

```
┌─────────────────────────────┐
│       Application Layer     │
├─────────────────────────────┤
│ - Application lifecycle     │
│ - System coordination       │
│ - Plugin management         │
└───────────────┬─────────────┘
                │
┌───────────────▼─────────────┐
│         Core Layer          │
├─────────────────────────────┤
│ 1. ECS Framework            │
│ 2. Event System             │
│ 3. Memory Management        │
│ 4. Error Handling           │
│ 5. Configuration System     │
└───────────────┬─────────────┘
                │
    ┌───────────┴───────────┐
    │                       │
┌───▼───────────┐     ┌─────▼───────────┐
│ Domain Layer  │     │ Services Layer  │
├───────────────┤     ├─────────────────┤
│ 1. Text Engine│     │ 1. Renderer     │
│ 2. Phantom    │     │ 2. Input System │
│ 3. Buffer     │     │ 3. File I/O     │
│ 4. Command    │     │ 4. UI System    │
│ 5. Camera     │     │ 5. Logger       │
└───────────────┘     └─────────────────┘
```

## Key Components

### Enhanced Entity Component System (ECS)
```c
// ecs.h
typedef struct {
    uint32_t id;
    uint32_t version;
} EntityId;

typedef struct {
    void* data;
    size_t size;
    size_t capacity;
    size_t component_size;
    uint32_t* sparse_map;  // For O(1) entity lookups
} ComponentPool;

typedef struct {
    EntityId* entities;
    size_t count;
    size_t capacity;
    ComponentPool* pools;
    size_t pool_count;
    uint32_t next_id;
} Registry;
```

### Improved Event System
```c
// event.h
typedef enum {
    EVENT_INPUT_KEY,
    EVENT_FILE_LOADED,
    EVENT_PHANTOM_SELECTED,
    EVENT_MODE_CHANGED,
    EVENT_BUFFER_MODIFIED
} EventType;

typedef enum {
    EVENT_PRIORITY_HIGH,
    EVENT_PRIORITY_NORMAL,
    EVENT_PRIORITY_LOW
} EventPriority;

typedef struct {
    EventType type;
    EventPriority priority;
    void* data;
    size_t data_size;
    bool handled;  // Allow event consumption
    EntityId source_entity;  // Optional source entity
} Event;

// Event subscription with filtering
typedef bool (*EventFilter)(const Event* event, void* user_data);
typedef void (*EventHandler)(const Event* event, void* user_data);
```

### Memory Management with Contexts
```c
// memory.h
typedef enum {
    MEMORY_CONTEXT_PERSISTENT,  // Long-lived allocations
    MEMORY_CONTEXT_FRAME,       // Cleared every frame
    MEMORY_CONTEXT_TEMP,        // Short-lived allocations
    MEMORY_CONTEXT_ASSET,       // Asset-related allocations
    MEMORY_CONTEXT_UI           // UI-related allocations
} MemoryContext;

typedef struct {
    void* ptr;
    size_t size;
    MemoryContext context;
    const char* file;
    int line;
    const char* function;
    uint64_t timestamp;
    bool freed;
} MemoryAllocation;
```

### Phantom Component System
```c
// Components for ECS
typedef struct {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
} TransformComponent;

typedef struct {
    BufferId buffer_id;  // Reference to buffer
    size_t line_from;
    size_t line_to;
    FontSettings_t font;
    Color color;
} PhantomComponent;

typedef struct {
    size_t line_no;
    size_t pos;
    struct lr_cell* owner;
    struct lr_cell* needle;
} CursorComponent;
```

### Input System with State Machine
```c
// input.h
typedef enum {
    INPUT_STATE_FREE,
    INPUT_STATE_EDIT,
    INPUT_STATE_COMMAND,
    INPUT_STATE_DRAG_START,
    INPUT_STATE_DRAG_ON
} InputState;

typedef struct {
    InputState current_state;
    InputState previous_state;
    
    // State transition table
    bool (*transitions[5][5])(InputEvent_t* event);
    
    // State handlers
    void (*state_handlers[5])(InputEvent_t* event);
    
    // Input contexts
    InputContext_t* contexts[5];
    InputContext_t* active_context;
} InputStateMachine;
```

### Rendering Pipeline with Layers
```c
// render.h
typedef enum {
    RENDER_LAYER_BACKGROUND,
    RENDER_LAYER_GRID,
    RENDER_LAYER_PHANTOMS,
    RENDER_LAYER_UI,
    RENDER_LAYER_DEBUG
} RenderLayer;

typedef struct {
    RenderLayer layer;
    int priority;
    void (*render_fn)(void* user_data);
    void* user_data;
    bool enabled;
} RenderPass;

typedef struct {
    RenderPass* passes;
    size_t pass_count;
    Camera_t* camera;
    bool wireframe_mode;
    bool debug_mode;
} RenderPipeline;
```

### Configuration System
```c
// config.h
typedef enum {
    CONFIG_TYPE_INT,
    CONFIG_TYPE_FLOAT,
    CONFIG_TYPE_STRING,
    CONFIG_TYPE_BOOL,
    CONFIG_TYPE_COLOR,
    CONFIG_TYPE_VECTOR3
} ConfigType;

typedef struct {
    const char* key;
    ConfigType type;
    union {
        int int_value;
        float float_value;
        char* string_value;
        bool bool_value;
        Color color_value;
        Vector3 vector3_value;
    } value;
    void (*on_change)(const char* key, void* old_value, void* new_value);
} ConfigEntry;
```

## Data Flow
```
1. Input → State Machine → Event System → Domain Systems
2. File I/O → Buffer System → Phantom Components
3. ECS Registry → Rendering Pipeline → Display
4. Command → Event → System Updates
5. Config Changes → Event → System Reconfiguration
```

## Implementation Strategy

### Phase 1: Core Infrastructure
1. Implement ECS Framework
2. Build Event System with priorities
3. Develop Memory Management with contexts
4. Create Error Handling system
5. Implement Configuration System

### Phase 2: Domain Layer
1. Text Engine with UTF-8 support
2. Phantom System using ECS components
3. Buffer System with efficient operations
4. Command System with history
5. Camera System with multiple modes

### Phase 3: Services Layer
1. Renderer with layer-based pipeline
2. Input System with state machine
3. File I/O with async operations
4. UI System with immediate-mode interface
5. Logger with multiple outputs

### Phase 4: Application Layer
1. Plugin System with dynamic loading
2. Project Management
3. Integration testing
4. Performance optimization
5. User experience refinement

## Benefits

- **Clear Separation of Concerns**: Domain logic separated from technical services
- **Improved Performance**: ECS with sparse sets for O(1) entity lookups
- **Flexible Event Handling**: Priorities and filtering for better control
- **State-Based Input**: Clear transitions between editor modes
- **Configurable Rendering**: Layer-based approach for extensibility
- **Memory Optimization**: Context-based allocation for better resource management
- **Plugin Architecture**: Designed for extension from the start

## Tech Stack

| Layer          | Technology          |
|----------------|---------------------|
| Core           | C11                 |
| Math           | Raymath             |
| Rendering      | Raylib              |
| Windowing      | Raylib              |
| Build System   | CMake               |
| Testing        | Unity               |
