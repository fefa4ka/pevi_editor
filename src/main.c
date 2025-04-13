#include <stdio.h>
#include <raylib.h>

int main() {                
    TraceLog(LOG_INFO, "Starting game...");

    // Initialize window
    int WIDTH = 1280;
    int HEIGHT = 800;
    InitWindow(WIDTH, HEIGHT, "My Game");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
    TraceLog(LOG_INFO, "Stopping game...");

  return 0;
}
