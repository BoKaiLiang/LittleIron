#include "iron.h"

#define WND_W 800
#define WND_H 600

#define COLOR_WHITE ((Color){ 255, 255, 255, 255 })
#define COLOR_PURPLE ((Color){ 106, 13, 173, 255 })

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

    // Game loop
    while (IsWindowRunning()) {
        StartScene(COLOR_WHITE);

        DrawFirstTexture(&tex, COLOR_WHITE);

        EndScene();
    }

    ReleaseTexture(&tex);

    ReleaseRenderer();
    ReleaseWindow();

    return 0;
}