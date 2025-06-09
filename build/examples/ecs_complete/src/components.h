#ifndef COMPONENTS_H
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

#endif // COMPONENTS_H