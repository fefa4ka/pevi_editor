#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    float x, y, z;
} Position;

typedef struct {
    float dx, dy, dz;
} Velocity;

typedef struct {
    int id;
    char name[32];
    Position pos;
    Velocity vel;
} Entity;

void update_position(Entity* entity, float dt) {
    entity->pos.x += entity->vel.dx * dt;
    entity->pos.y += entity->vel.dy * dt;
    entity->pos.z += entity->vel.dz * dt;
}

int main() {
    printf("Debug Demo Starting...\n");
    
    // Create some entities
    Entity entities[3];
    
    // Initialize first entity
    entities[0].id = 1;
    strcpy(entities[0].name, "Player");
    entities[0].pos = (Position){0.0f, 0.0f, 0.0f};
    entities[0].vel = (Velocity){1.0f, 0.0f, 0.0f};
    
    // Initialize second entity  
    entities[1].id = 2;
    strcpy(entities[1].name, "Enemy");
    entities[1].pos = (Position){10.0f, 5.0f, 0.0f};
    entities[1].vel = (Velocity){-0.5f, 0.2f, 0.0f};
    
    // Initialize third entity
    entities[2].id = 3;
    strcpy(entities[2].name, "Projectile");
    entities[2].pos = (Position){0.0f, 0.0f, 0.0f};
    entities[2].vel = (Velocity){5.0f, 0.0f, 0.0f};
    
    float dt = 0.016f; // 60 FPS
    
    printf("Initial positions:\n");
    for (int i = 0; i < 3; i++) {
        printf("Entity %s: pos(%.2f, %.2f, %.2f)\n", 
               entities[i].name, 
               entities[i].pos.x, entities[i].pos.y, entities[i].pos.z);
    }
    
    // Simulate a few frames
    for (int frame = 0; frame < 5; frame++) {
        printf("\nFrame %d:\n", frame + 1);
        
        for (int i = 0; i < 3; i++) {
            update_position(&entities[i], dt);
            printf("Entity %s: pos(%.2f, %.2f, %.2f)\n", 
                   entities[i].name, 
                   entities[i].pos.x, entities[i].pos.y, entities[i].pos.z);
        }
    }
    
    printf("Debug Demo Finished.\n");
    return 0;
}
