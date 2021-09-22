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
    e.speed.x = 300.0f;
    e.speed.y = 0.0f;
    e.c = COLOR_PURPLE;
    
    V2f size = {
        25.0f,
        25.0f
    };

    V2f wnd_sz =  GetWindowWH();

    int fps_count = 0;
    float time_count = 0.0f;

    SetTargetFPS(60);

    // Game loop
    while (IsWindowRunning()) {
#if 0
        if ((time_count += GetDeltaTime()) >= 1.0f) {
            time_count = 0.0f;
            LogInfo("FPS: %d", fps_count);
            fps_count = 0;
        }
#endif

        LogInfo("Ddelta time: %f", GetDeltaTime());

        StartScene(COLOR_BLACK);

        BeginRendering();

        

        EndRendering();

        EndScene();
        fps_count += 1;
    }

    ReleaseTexture(&tex);

    ReleaseRenderer();
    ReleaseWindow();

    return 0;
}