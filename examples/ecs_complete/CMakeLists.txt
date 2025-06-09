cmake_minimum_required(VERSION 3.11)
project(PeviECSComplete C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Find required packages
find_package(raylib REQUIRED)
find_package(flecs REQUIRED)

# Collect all source files
file(GLOB_RECURSE SOURCES 
    "components/*.c"
    "systems/*.c"
)

# Create executable
add_executable(spatial_editor 
    main.c
    ${SOURCES}
)

# Set up include directories
target_include_directories(spatial_editor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/components
    ${CMAKE_CURRENT_SOURCE_DIR}/systems
)

# Link libraries
target_link_libraries(spatial_editor PRIVATE 
    raylib
    flecs::flecs_static
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(spatial_editor PRIVATE winmm)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(spatial_editor PRIVATE m pthread dl)
elseif(APPLE)
    target_link_libraries(spatial_editor PRIVATE 
        "-framework CoreVideo"
        "-framework IOKit"
        "-framework Cocoa"
        "-framework GLUT"
        "-framework OpenGL"
    )
endif()

# Compiler flags for optimization and warnings
target_compile_options(spatial_editor PRIVATE
    $<$<CONFIG:Debug>:-g -O0 -Wall -Wextra -Wpedantic>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
)

# Enable additional warnings for better code quality
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(spatial_editor PRIVATE
        -Wno-unused-parameter  # ECS systems often have unused parameters
        -Wno-missing-field-initializers  # Common with component initialization
    )
endif()

# Create example source directory structure if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src)

# Create example source files for demonstration
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/src/main.c 
"#include <stdio.h>
#include <raylib.h>

int main() {
    InitWindow(800, 600, \"Hello Pevi\");
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(\"Hello, 3D Code Editor!\", 190, 200, 20, WHITE);
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}")

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/src/utils.c 
"#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Utility functions for Pevi editor
char* load_file_content(const char* filepath) {
    FILE *file = fopen(filepath, \"r\");
    if (!file) return NULL;
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *content = malloc(length + 1);
    fread(content, 1, length, file);
    content[length] = '\\0';
    
    fclose(file);
    return content;
}")

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/src/components.h 
"#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <flecs.h>
#include <raylib.h>

// Example component definitions
typedef struct {
    float x, y, z;
} Position;

typedef struct {
    char text[256];
    Color color;
} TextContent;

#endif // COMPONENTS_H")

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/src/systems.c 
"#include \"components.h\"
#include <stdio.h>

// Example system implementations
void RenderSystem(ecs_iter_t *it) {
    Position *positions = ecs_field(it, Position, 0);
    TextContent *texts = ecs_field(it, TextContent, 1);
    
    for (int i = 0; i < it->count; i++) {
        Vector3 pos = {positions[i].x, positions[i].y, positions[i].z};
        DrawText3D(GetFontDefault(), texts[i].text, pos, 
                  12.0f, 1.0f, 0.0f, true, texts[i].color);
    }
}")

# Print configuration info
message(STATUS "Pevi ECS Complete Example Configuration:")
message(STATUS "  C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Raylib: ${raylib_VERSION}")
message(STATUS "  Flecs: ${flecs_VERSION}")
message(STATUS "  Sources: ${SOURCES}")
message(STATUS "  Target: spatial_editor")
