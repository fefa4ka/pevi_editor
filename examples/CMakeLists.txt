cmake_minimum_required(VERSION 3.10)

# Build the raylib example
add_executable(raylib_example raylib.c)
target_link_libraries(raylib_example PRIVATE raylib glfw)

# Build the flecs example
add_executable(flecs_example flecs.c)
target_link_libraries(flecs_example PRIVATE flecs::flecs_static)

# Build the joltc physics example
add_executable(joltc_example joltc.c)
target_link_libraries(joltc_example PRIVATE 
    joltc
    stdc++
)

# Build the libuv example
add_executable(libuv_example libuv_example.c)
target_link_libraries(libuv_example PRIVATE 
    libuv::uv_a
)

# Set common properties for examples
set_target_properties(raylib_example joltc_example libuv_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

# Add install targets
install(TARGETS raylib_example joltc_example libuv_example
    DESTINATION bin/examples
)
