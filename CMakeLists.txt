cmake_minimum_required(VERSION 3.10)
project(pevi C)

# Set the C standard to C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_EXTENSIONS OFF) # Optional: Disable GNU extensions if strict C11 is desired

# --- Find Dependencies ---
include(FetchContent)

# Clone and build joltc
FetchContent_Declare(
    joltc
    GIT_REPOSITORY https://github.com/amerkoleci/joltc.git
    GIT_TAG main
)

# Clone and build cimgui
FetchContent_Declare(
    cimgui
    GIT_REPOSITORY https://github.com/cimgui/cimgui.git
    GIT_TAG master
)

# Clone and build msdfgen
FetchContent_Declare(
    msdfgen
    GIT_REPOSITORY https://github.com/Chlumsky/msdfgen.git
    GIT_TAG master
)

# Clone and build msdf-atlas-gen
FetchContent_Declare(
    msdf-atlas-gen
    GIT_REPOSITORY https://github.com/Chlumsky/msdf-atlas-gen.git
    GIT_TAG master
)

FetchContent_MakeAvailable(joltc cimgui msdfgen msdf-atlas-gen)
 
find_library(LR_TEXT_LIB lr_text REQUIRED)
find_library(LR_LIB lr REQUIRED)
message(STATUS "LR_TEXT_LIB: ${LR_LIB}")
find_path(LR_TEXT_INCLUDE_DIR lr_text.h REQUIRED)
include_directories(${LR_TEXT_INCLUDE_DIR})
if(NOT LR_TEXT_LIB)
  include(FetchContent)
  FetchContent_Declare(
    lr_text
    GIT_REPOSITORY https://github.com/fefa4ka/lr_text.git
    GIT_TAG master)
  FetchContent_MakeAvailable(lr_text) # Get linked_ring source directory
  FetchContent_GetProperties(lr_text SOURCE_DIR LINKED_RING_SOURCE_DIR)
  set(LR_TEXT_LIB lr_text)
endif()                

# Find the Raylib library
# This assumes Raylib has been installed in a way CMake can find it
# (e.g., system-wide, or RAYLIB_ROOT environment variable is set)
find_package(raylib REQUIRED)
find_package(libuv REQUIRED)
find_package(flecs REQUIRED)
find_package(glfw3 REQUIRED)

# --- Project Sources ---
# Define the source files for the main executable
# Assuming the main entry point is src/main.c
add_executable(editor src/main.c)

# --- Include Directories ---
# Add the 'src' directory to the include path
target_include_directories(editor PRIVATE 
    src
    ${raylib-cimgui_SOURCE_DIR}/src
)

# --- Link Dependencies ---
# Link the executable against dependencies
target_include_directories(editor PRIVATE ${joltc_SOURCE_DIR}/include)
target_link_libraries(editor PRIVATE
    glfw
    raylib
    joltc
    flecs::flecs_static
    libuv::uv_a
    cimgui
    stdc++
    ${LR_LIB}
    ${LR_TEXT_LIB}
)

# --- Build Configuration ---
# Set a default build type if none is specified (e.g., Release, Debug)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug Release RelWithDebInfo MinSizeRel)" FORCE)
  message(STATUS "Setting build type to '${CMAKE_BUILD_TYPE}' as none was specified.")
endif()

# --- Optional: Compiler Flags ---
# Add common warning flags (adjust as needed for your compiler)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(editor PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Add examples subdirectory
add_subdirectory(examples)

# --- Output ---
# Print configuration summary
message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Raylib Found: ${raylib_FOUND}")
message(STATUS "Raylib Include Dirs: ${raylib_INCLUDE_DIRS}")
message(STATUS "Raylib Libraries: ${raylib_LIBRARIES}")
# Flecs
message(STATUS "Flecs Found: ${flecs_FOUND}")
message(STATUS "Flecs Include Dirs: ${flecs_INCLUDE_DIRS}")
message(STATUS "Flecs Libraries: ${flecs_LIBRARIES}")
message(STATUS "Configuring done")

# --- Installation (Optional Example) ---
# Define installation rules if needed
# install(TARGETS editor DESTINATION bin)
# install(FILES src/my_header.h DESTINATION include)
