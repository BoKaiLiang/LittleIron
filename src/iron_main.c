#include "iron.h"

#include <stdio.h>

#define WND_W 800
#define WND_H 600

typedef struct {
    V2f pos;
    V2f speed;
    Color c;
} Entity;

int main() {

    // Init the window
    ResT create_wnd_res = CreateWindow(WND_W, WND_H, "Little Iron");
    if (create_wnd_res != RES_SUCCESS) {
        ReleaseWindow();
        return create_wnd_res;
    }

    ResT create_rdr_res = CreateRenderer();
    if (create_rdr_res != RES_SUCCESS) {
        ReleaseWindow();
        ReleaseRenderer();
        return create_rdr_res;
    }

    Texture tex;
    ResT load_tex_res = LoadTextureFile(&tex, "assets/kenney_animalpackredux/rabbit.png");
    if (load_tex_res != RES_SUCCESS) {
        ReleaseWindow();
        ReleaseRenderer();
        return load_tex_res;
    }

    Entity e;
    e.pos.x = 0.0f;
    e.pos.y = 300.0f;
    e.speed.x = 150.0f;
    e.speed.y = 0.0f;
    e.c = COLOR_PURPLE;
    
    V2f size = {
        25.0f,
        25.0f
    };

    V2f wnd_sz =  GetWindowWH();

    SetTargetFPS(10);
    
    float current_time = GetTime();
    float previous_time = GetTime();
    float time_elapsed = 0.0f;

    int test_count = 0;

    // Game loop
    while (IsWindowRunning()) {
        
        if (e.pos.x > 800.0f) {
            current_time = GetTime();
            time_elapsed = current_time - previous_time;
            previous_time = current_time;

            LogInfo("Time elapsed: %f", time_elapsed);
            e.pos.x = 0.0f;
        }

        e.pos = V2fAdd(e.pos, V2fScalef(e.speed, GetDeltaTime()));

        StartScene(COLOR_BLACK);
        BeginRendering();

        DrawRectangle(e.pos, size, 0.0f, e.c);

        EndRendering();
        EndScene();
    }

    ReleaseTexture(&tex);

    ReleaseRenderer();
    ReleaseWindow();

    return 0;
}