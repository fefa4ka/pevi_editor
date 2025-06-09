#include <stdio.h>
#include <raylib.h>

int main() {
    InitWindow(800, 600, "Hello Pevi");
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Hello, 3D Code Editor!", 190, 200, 20, WHITE);
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}