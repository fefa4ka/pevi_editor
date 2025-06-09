#include "components.h"
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
}