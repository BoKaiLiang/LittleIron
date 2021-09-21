#include "iron.h"

#include <stdio.h>

#define WND_W 800
#define WND_H 600

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
    LogInfo("Texture data: %d, %d, %d", tex.id, tex.w, tex.h);

    V2f pos = { 100.0f, 100.0f };
    V2f pos1 = { 450.0f, 200.0f };
    V2f size = { 50.0f, 50.0f };
    float rotate = 45.0f;

    // Game loop
    while (IsWindowRunning()) {
        StartScene(COLOR_BLACK);

        BeginRendering();

        DrawTexture(tex, pos1, size, 0.0f, COLOR_WHITE);

        DrawRectangle(pos, size, rotate, COLOR_PURPLE);

        EndRendering();

        EndScene();
    }

    ReleaseTexture(&tex);

    ReleaseRenderer();
    ReleaseWindow();

    return 0;
}