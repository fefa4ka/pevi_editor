cmake_minimum_required(VERSION 3.10)

# Build the raylib example
add_executable(raylib_example raylib.c)
target_link_libraries(raylib_example PRIVATE raylib glfw)

# Build the flecs example
add_executable(flecs_example flecs.c)
target_link_libraries(flecs_example PRIVATE flecs::flecs_static)

add_executable(flecs_hierarchy_example flecs_hierarchy.c)
target_link_libraries(flecs_hierarchy_example PRIVATE flecs::flecs_static)

# Build the basic flecs entity component example
add_executable(flecs_basic_entity_component flecs_basic_entity_component.c)
target_link_libraries(flecs_basic_entity_component PRIVATE flecs::flecs_static)

# Build the flecs pevi basic example
add_executable(flecs_pevi_basic flecs_pevi_basic.c)
target_link_libraries(flecs_pevi_basic PRIVATE flecs::flecs_static)

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

# Build the msdf text rendering example
add_executable(msdf_text_3d msdf_text_3d.c)
target_include_directories(msdf_text_3d PRIVATE ${FREETYPE_INCLUDE_DIRS})
if(APPLE)
    target_link_libraries(msdf_text_3d PRIVATE
        glfw
        "-framework OpenGL"
        ${FREETYPE_LIBRARIES}
    )
else()
    target_link_libraries(msdf_text_3d PRIVATE
        glfw
        GL
        ${FREETYPE_LIBRARIES}
    )
endif()

# Add the comprehensive ECS complete example subdirectory
add_subdirectory(ecs_complete)

# Set common properties for examples
set_target_properties(raylib_example joltc_example libuv_example flecs_example flecs_hierarchy_example flecs_basic_entity_component flecs_pevi_basic msdf_text_3d PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

# Add install targets
install(TARGETS raylib_example joltc_example libuv_example flecs_example flecs_hierarchy_example flecs_basic_entity_component flecs_pevi_basic msdf_text_3d
    DESTINATION bin/examples
)
