#include "file_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

// Helper function to create phantom from text line
ecs_entity_t CreatePhantomFromLine(ecs_world_t *world, const char* line_text, int line_number, 
                                   const char* filepath, Vector3 position, ecs_entity_t parent) {
    // Create phantom entity for this line
    ecs_entity_t phantom = ecs_new_w_pair(world, EcsChildOf, parent);
    
    // Set spatial components
    ecs_set(world, phantom, Position, position);
    ecs_set(world, phantom, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
    ecs_set(world, phantom, Scale, {1.0f, 1.0f, 1.0f});
    ecs_set(world, phantom, Transform, {.needs_update = true});
    
    // Set text content
    TextContent text_content = {
        .font_size = 1.0f,
        .color = WHITE,
        .billboard_mode = false
    };
    strncpy(text_content.text, line_text, sizeof(text_content.text) - 1);
    text_content.text[sizeof(text_content.text) - 1] = '\0';
    ecs_set(world, phantom, TextContent, text_content);
    
    // Set file reference
    FileReference file_ref = {
        .line_number = line_number,
        .last_modified = time(NULL)
    };
    strncpy(file_ref.filepath, filepath, sizeof(file_ref.filepath) - 1);
    file_ref.filepath[sizeof(file_ref.filepath) - 1] = '\0';
    ecs_set(world, phantom, FileReference, file_ref);
    
    // Add bounding sphere for selection
    ecs_set(world, phantom, BoundingSphere, {0.5f, {0.0f, 0.0f, 0.0f}});
    
    // Make visible by default
    ecs_add(world, phantom, Visible);
    
    return phantom;
}

// Load text file and create phantom entities for each line
void LoadFileAsPhantoms(ecs_world_t *world, const char* filepath, Vector3 start_position) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        printf("Failed to open file: %s\n", filepath);
        
        // Create a placeholder entity even if file doesn't exist
        ecs_entity_t placeholder = ecs_new(world);
        ecs_set_name(world, placeholder, filepath);
        ecs_set(world, placeholder, Position, start_position);
        ecs_set(world, placeholder, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
        ecs_set(world, placeholder, Scale, {1.0f, 1.0f, 1.0f});
        ecs_set(world, placeholder, Transform, {.needs_update = true});
        
        TextContent error_text = {.font_size = 1.5f, .color = RED, .billboard_mode = false};
        snprintf(error_text.text, sizeof(error_text.text), "FILE NOT FOUND: %s", filepath);
        ecs_set(world, placeholder, TextContent, error_text);
        
        return;
    }
    
    char line_buffer[1024];
    int line_number = 0;
    float line_spacing = 1.5f;
    
    // Create file container entity
    ecs_entity_t file_entity = ecs_entity(world, {0});
    ecs_set_name(world, file_entity, filepath);
    ecs_set(world, file_entity, Position, start_position);
    ecs_set(world, file_entity, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
    ecs_set(world, file_entity, Scale, {1.0f, 1.0f, 1.0f});
    ecs_set(world, file_entity, Transform, {.needs_update = true});
    
    // Set file reference for container
    FileReference file_ref = {
        .line_number = 0,
        .last_modified = time(NULL)
    };
    strncpy(file_ref.filepath, filepath, sizeof(file_ref.filepath) - 1);
    file_ref.filepath[sizeof(file_ref.filepath) - 1] = '\0';
    ecs_set(world, file_entity, FileReference, file_ref);
    
    // Set file title
    TextContent file_title = {.font_size = 2.0f, .color = BLUE, .billboard_mode = false};
    const char* filename = strrchr(filepath, '/');
    filename = filename ? filename + 1 : filepath;
    strncpy(file_title.text, filename, sizeof(file_title.text) - 1);
    file_title.text[sizeof(file_title.text) - 1] = '\0';
    ecs_set(world, file_entity, TextContent, file_title);
    
    // Create phantom for each line
    while (fgets(line_buffer, sizeof(line_buffer), file)) {
        // Remove newline character
        line_buffer[strcspn(line_buffer, "\n")] = 0;
        
        // Skip empty lines
        if (strlen(line_buffer) == 0) {
            line_number++;
            continue;
        }
        
        // Calculate line position
        Vector3 line_position = {
            start_position.x,
            start_position.y - (line_number * line_spacing),
            start_position.z
        };
        
        CreatePhantomFromLine(world, line_buffer, line_number, filepath, line_position, file_entity);
        line_number++;
    }
    
    fclose(file);
    printf("Loaded %d lines from %s as phantoms\n", line_number, filepath);
}

// Batch create entities from multiple files
void LoadProjectAsPhantoms(ecs_world_t *world, const char* project_path) {
    // Create some example files if they don't exist
    const char* source_files[] = {
        "main.c",
        "utils.c", 
        "components.h",
        "systems.c"
    };
    
    Vector3 file_positions[] = {
        {0.0f, 0.0f, 0.0f},
        {15.0f, 0.0f, 0.0f},
        {-15.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 15.0f}
    };
    
    // Create example files if project directory doesn't exist
    struct stat st = {0};
    if (stat(project_path, &st) == -1) {
        printf("Project path %s doesn't exist, creating example files in memory\n", project_path);
        
        // Create in-memory representations
        for (int i = 0; i < 4; i++) {
            char full_path[512];
            snprintf(full_path, sizeof(full_path), "%s/%s", project_path, source_files[i]);
            
            // Create phantom file entity with example content
            ecs_entity_t file_entity = ecs_new(world);
            ecs_set_name(world, file_entity, source_files[i]);
            ecs_set(world, file_entity, Position, file_positions[i]);
            ecs_set(world, file_entity, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
            ecs_set(world, file_entity, Scale, {1.0f, 1.0f, 1.0f});
            ecs_set(world, file_entity, Transform, {.needs_update = true});
            
            TextContent file_text = {.font_size = 2.0f, .color = BLUE, .billboard_mode = false};
            strncpy(file_text.text, source_files[i], sizeof(file_text.text) - 1);
            ecs_set(world, file_entity, TextContent, file_text);
            
            // Add some example code lines
            const char* example_lines[] = {
                "#include <stdio.h>",
                "",
                "int main() {",
                "    printf(\"Hello, World!\\n\");",
                "    return 0;",
                "}"
            };
            
            for (int j = 0; j < 6; j++) {
                if (strlen(example_lines[j]) > 0) {
                    Vector3 line_pos = {
                        file_positions[i].x,
                        file_positions[i].y - ((j + 1) * 1.5f),
                        file_positions[i].z
                    };
                    CreatePhantomFromLine(world, example_lines[j], j, full_path, line_pos, file_entity);
                }
            }
        }
        return;
    }
    
    // Load actual files if project path exists
    for (int i = 0; i < 4; i++) {
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", project_path, source_files[i]);
        LoadFileAsPhantoms(world, full_path, file_positions[i]);
    }
}
