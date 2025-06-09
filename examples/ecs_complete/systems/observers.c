#include "observers.h"
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

// Selection change observer
void OnSelectionChanged(ecs_iter_t *it) {
    Selected *selections = ecs_field(it, Selected, 0);
    
    for (int i = 0; i < it->count; i++) {
        ecs_entity_t entity = it->entities[i];
        
        if (selections[i].is_selected) {
            // Add visual selection highlight
            if (ecs_has(it->world, entity, TextContent)) {
                TextContent *text = ecs_get_mut(it->world, entity, TextContent);
                if (text) {
                    text->color = YELLOW;  // Highlight selected entities
                    text->billboard_mode = true;
                }
            }
            printf("Entity %llu selected at time %.2f\n", entity, selections[i].selection_time);
        } else {
            // Restore normal appearance
            if (ecs_has(it->world, entity, TextContent)) {
                TextContent *text = ecs_get_mut(it->world, entity, TextContent);
                if (text) {
                    text->color = WHITE;
                    text->billboard_mode = false;
                }
            }
        }
    }
}

// File modification observer
void OnFileModified(ecs_iter_t *it) {
    FileReference *file_refs = ecs_field(it, FileReference, 0);
    
    for (int i = 0; i < it->count; i++) {
        ecs_entity_t file_entity = it->entities[i];
        
        // Check if file needs reloading
        struct stat file_stat;
        if (stat(file_refs[i].filepath, &file_stat) == 0) {
            if (file_stat.st_mtime > file_refs[i].last_modified) {
                printf("File %s modified, reloading phantoms\n", file_refs[i].filepath);
                
                // Find all phantom entities using this file
                ecs_query_t *phantom_query = ecs_query(it->world, {
                    .terms = {{ ecs_id(FileReference) }}
                });
                
                ecs_iter_t phantom_it = ecs_query_iter(it->world, phantom_query);
                while (ecs_query_next(&phantom_it)) {
                    FileReference *phantom_refs = ecs_field(&phantom_it, FileReference, 0);
                    for (int j = 0; j < phantom_it.count; j++) {
                        if (strcmp(phantom_refs[j].filepath, file_refs[i].filepath) == 0) {
                            // Trigger phantom reload
                            ecs_add(it->world, phantom_it.entities[j], NeedsReload);
                        }
                    }
                }
                ecs_query_fini(phantom_query);
                
                file_refs[i].last_modified = file_stat.st_mtime;
            }
        }
    }
}

// Register observers
void RegisterObservers(ecs_world_t *world) {
    // Selection change observer
    ecs_observer_desc_t selection_observer_desc = {0};
    selection_observer_desc.query.terms[0].id = ecs_id(Selected);
    selection_observer_desc.events[0] = EcsOnSet;
    selection_observer_desc.callback = OnSelectionChanged;
    ecs_observer_init(world, &selection_observer_desc);
    
    // File modification observer  
    ecs_observer_desc_t file_observer_desc = {0};
    file_observer_desc.query.terms[0].id = ecs_id(FileReference);
    file_observer_desc.events[0] = EcsOnAdd;
    file_observer_desc.events[1] = EcsOnSet;
    file_observer_desc.callback = OnFileModified;
    ecs_observer_init(world, &file_observer_desc);
}
