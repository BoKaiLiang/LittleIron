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

    V2f v = V2fAdd(V2F_X, V2F_Y);
    LogInfo("v = %s", V2fToString(v));

    LogInfo("Vector 4 Y axis: %s", V4fToString(V4F_Y));

    Mat4 m = MAT4_IDENTITY;
    LogInfo("Matrix: %s", Mat4ToString(m));

    // Game loop
    while (IsWindowRunning()) {
        StartScene(COLOR_WHITE);

        // input test

        DrawFirstTexture(&tex, COLOR_WHITE);

        EndScene();
    }

    ReleaseTexture(&tex);

    ReleaseRenderer();
    ReleaseWindow();

    return 0;
}