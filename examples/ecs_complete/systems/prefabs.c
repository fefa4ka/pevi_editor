#include "prefabs.h"
#include <string.h>
#include <stdio.h>

// Create prefabs for common code editor elements
void CreatePrefabs(ecs_world_t *world) {
    // Function block prefab
    ecs_entity_t function_prefab = ecs_new(world);
    ecs_add_id(world, function_prefab, EcsPrefab);
    ecs_set_name(world, function_prefab, "FunctionPrefab");
    
    // Set default components for function prefab
    ecs_set(world, function_prefab, Position, {0.0f, 0.0f, 0.0f});
    ecs_set(world, function_prefab, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
    ecs_set(world, function_prefab, Scale, {1.0f, 1.0f, 1.0f});
    ecs_set(world, function_prefab, EcsTransform, {.needs_update = true});
    ecs_set(world, function_prefab, TextContent, {"function()", 1.5f, GREEN, false});
    ecs_set(world, function_prefab, BoundingSphere, {1.0f, {0.0f, 0.0f, 0.0f}});
    
    // Code block prefab (child of function)
    ecs_entity_t block_prefab = ecs_new_w_pair(world, EcsChildOf, function_prefab);
    ecs_set_name(world, block_prefab, "CodeBlockPrefab");
    ecs_set(world, block_prefab, Position, {1.0f, -1.0f, 0.0f});
    ecs_set(world, block_prefab, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
    ecs_set(world, block_prefab, Scale, {1.0f, 1.0f, 1.0f});
    ecs_set(world, block_prefab, EcsTransform, {.needs_update = true});
    ecs_set(world, block_prefab, TextContent, {"{}", 1.2f, GRAY, false});
    
    // File structure prefab
    ecs_entity_t file_prefab = ecs_new(world);
    ecs_add_id(world, file_prefab, EcsPrefab);
    ecs_set_name(world, file_prefab, "FilePrefab");
    ecs_set(world, file_prefab, Position, {0.0f, 0.0f, 0.0f});
    ecs_set(world, file_prefab, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
    ecs_set(world, file_prefab, Scale, {1.0f, 1.0f, 1.0f});
    ecs_set(world, file_prefab, EcsTransform, {.needs_update = true});
    ecs_set(world, file_prefab, TextContent, {"file.c", 2.0f, BLUE, false});
    
    // Header section slot
    ecs_entity_t header_slot = ecs_new_w_pair(world, EcsChildOf, file_prefab);
    ecs_add_pair(world, header_slot, EcsSlotOf, file_prefab);
    ecs_set_name(world, header_slot, "HeaderSection");
    ecs_set(world, header_slot, Position, {0.0f, 3.0f, 0.0f});
    ecs_set(world, header_slot, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
    ecs_set(world, header_slot, Scale, {1.0f, 1.0f, 1.0f});
    ecs_set(world, header_slot, EcsTransform, {.needs_update = true});
    ecs_set(world, header_slot, TextContent, {"#include", 1.2f, PURPLE, false});
    
    // Function section slot
    ecs_entity_t func_slot = ecs_new_w_pair(world, EcsChildOf, file_prefab);
    ecs_add_pair(world, func_slot, EcsSlotOf, file_prefab);
    ecs_set_name(world, func_slot, "FunctionSection");
    ecs_set(world, func_slot, Position, {0.0f, 0.0f, 0.0f});
    ecs_set(world, func_slot, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
    ecs_set(world, func_slot, Scale, {1.0f, 1.0f, 1.0f});
    ecs_set(world, func_slot, EcsTransform, {.needs_update = true});
    ecs_set(world, func_slot, TextContent, {"functions...", 1.0f, WHITE, false});
}

// Instantiate prefabs for specific files
ecs_entity_t CreateFunctionInstance(ecs_world_t *world, const char* name, Vector3 position) {
    ecs_entity_t function_prefab = ecs_lookup(world, "FunctionPrefab");
    if (!function_prefab) {
        printf("Error: FunctionPrefab not found\n");
        return 0;
    }
    
    ecs_entity_t instance = ecs_new_w_pair(world, EcsIsA, function_prefab);
    
    // Override specific properties
    ecs_set_name(world, instance, name);
    ecs_set(world, instance, Position, {position.x, position.y, position.z});
    
    TextContent text_content = {.font_size = 1.5f, .color = GREEN, .billboard_mode = false};
    strncpy(text_content.text, name, sizeof(text_content.text) - 1);
    ecs_set_ptr(world, instance, TextContent, &text_content);
    
    ecs_set(world, instance, EcsTransform, {.needs_update = true});
    
    return instance;
}

ecs_entity_t CreateFileInstance(ecs_world_t *world, const char* filename) {
    ecs_entity_t file_prefab = ecs_lookup(world, "FilePrefab");
    if (!file_prefab) {
        printf("Error: FilePrefab not found\n");
        return 0;
    }
    
    ecs_entity_t file_instance = ecs_new_w_pair(world, EcsIsA, file_prefab);
    ecs_set_name(world, file_instance, filename);
    
    // Set file-specific text content
    TextContent text_content = {.font_size = 2.0f, .color = BLUE, .billboard_mode = false};
    strncpy(text_content.text, filename, sizeof(text_content.text) - 1);
    ecs_set_ptr(world, file_instance, TextContent, &text_content);
    
    ecs_set(world, file_instance, EcsTransform, {.needs_update = true});
    
    // Access child slots for customization
    ecs_iter_t slot_it = ecs_children(world, file_instance);
    while (ecs_children_next(&slot_it)) {
        for (int i = 0; i < slot_it.count; i++) {
            const char* slot_name = ecs_get_name(world, slot_it.entities[i]);
            if (slot_name && strcmp(slot_name, "HeaderSection") == 0) {
                TextContent header_text = {.font_size = 1.2f, .color = PURPLE, .billboard_mode = false};
                strncpy(header_text.text, "#include <stdio.h>", sizeof(header_text.text) - 1);
                ecs_set_ptr(world, slot_it.entities[i], TextContent, &header_text);
            }
        }
    }
    
    return file_instance;
}

// Create spatial hierarchy for code structure
void CreateCodeHierarchy(ecs_world_t *world) {
    // Create root scene entity
    ecs_entity_t scene_root = ecs_entity(world, { .name = "SceneRoot" });
    ecs_set(world, scene_root, Position, {0.0f, 0.0f, 0.0f});
    ecs_set(world, scene_root, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
    ecs_set(world, scene_root, Scale, {1.0f, 1.0f, 1.0f});
    ecs_set(world, scene_root, EcsTransform, {.needs_update = true});
    
    // Create file entity as child of scene root
    ecs_entity_t code_file = ecs_entity(world, { .name = "main.c" });
    ecs_add_pair(world, code_file, EcsChildOf, scene_root);
    ecs_set(world, code_file, Position, {0.0f, 10.0f, 0.0f});
    ecs_set(world, code_file, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
    ecs_set(world, code_file, Scale, {1.0f, 1.0f, 1.0f});
    ecs_set(world, code_file, EcsTransform, {.needs_update = true});
    ecs_set(world, code_file, TextContent, {"main.c", 2.0f, BLUE, false});
    ecs_set(world, code_file, FileReference, {"./src/main.c", 0, 0});
    
    // Create function entity as child of file
    ecs_entity_t main_function = ecs_entity(world, { .name = "main_function" });
    ecs_add_pair(world, main_function, EcsChildOf, code_file);
    ecs_set(world, main_function, Position, {3.0f, -2.0f, 0.0f});
    ecs_set(world, main_function, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
    ecs_set(world, main_function, Scale, {1.0f, 1.0f, 1.0f});
    ecs_set(world, main_function, EcsTransform, {.needs_update = true});
    ecs_set(world, main_function, TextContent, {"int main()", 1.5f, GREEN, false});
    
    // Create code block as child of function  
    ecs_entity_t if_block = ecs_entity(world, { .name = "if_statement" });
    ecs_add_pair(world, if_block, EcsChildOf, main_function);
    ecs_set(world, if_block, Position, {2.0f, -1.5f, 0.0f});
    ecs_set(world, if_block, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
    ecs_set(world, if_block, Scale, {1.0f, 1.0f, 1.0f});
    ecs_set(world, if_block, EcsTransform, {.needs_update = true});
    ecs_set(world, if_block, TextContent, {"if (condition)", 1.2f, ORANGE, false});
}

// Custom relationships for code dependencies
void SetupCodeDependencies(ecs_world_t *world) {
    // Create entities for different code elements
    ecs_entity_t header_file = ecs_entity(world, { .name = "stdio.h" });
    ecs_entity_t source_file = ecs_entity(world, { .name = "main.c" });
    ecs_entity_t printf_func = ecs_entity(world, { .name = "printf" });
    ecs_entity_t main_func = ecs_entity(world, { .name = "main" });
    
    // Establish dependency relationships
    ecs_add_pair(world, source_file, Includes, header_file);
    ecs_add_pair(world, main_func, References, printf_func);
    ecs_add_pair(world, printf_func, Contains, header_file);
    
    // Position entities based on dependencies
    ecs_set(world, header_file, Position, {-5.0f, 5.0f, 0.0f});
    ecs_set(world, header_file, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
    ecs_set(world, header_file, Scale, {1.0f, 1.0f, 1.0f});
    ecs_set(world, header_file, EcsTransform, {.needs_update = true});
    ecs_set(world, header_file, TextContent, {"stdio.h", 1.0f, PURPLE, false});
    
    ecs_set(world, source_file, Position, {0.0f, 0.0f, 0.0f});
    ecs_set(world, source_file, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
    ecs_set(world, source_file, Scale, {1.0f, 1.0f, 1.0f});
    ecs_set(world, source_file, EcsTransform, {.needs_update = true});
    ecs_set(world, source_file, TextContent, {"main.c", 1.5f, BLUE, false});
    
    ecs_set(world, main_func, Position, {2.0f, -2.0f, 0.0f});
    ecs_set(world, main_func, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
    ecs_set(world, main_func, Scale, {1.0f, 1.0f, 1.0f});
    ecs_set(world, main_func, EcsTransform, {.needs_update = true});
    ecs_set(world, main_func, TextContent, {"main()", 1.2f, GREEN, false});
    
    ecs_set(world, printf_func, Position, {-2.0f, -2.0f, 0.0f});
    ecs_set(world, printf_func, Rotation, {0.0f, 0.0f, 0.0f, 1.0f});
    ecs_set(world, printf_func, Scale, {1.0f, 1.0f, 1.0f});
    ecs_set(world, printf_func, EcsTransform, {.needs_update = true});
    ecs_set(world, printf_func, TextContent, {"printf()", 1.2f, YELLOW, false});
}
