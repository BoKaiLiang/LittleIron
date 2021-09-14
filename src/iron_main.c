#include "iron.h"

#define WND_W 800
#define WND_H 600

#define COLOR_WHITE ((Color){ 255, 255, 255, 255 })
#define COLOR_PURPLE ((Color){ 106, 13, 173, 255 })

int main() {

    // Init the window
    ResT res = CreateWindow(WND_W, WND_H, "Little Iron");
    if (res == RES_ERROR_CREATE_WINDOW) {
        ReleaseWindow();
        return RES_ERROR_CREATE_WINDOW;
    }

    ResT res1 = CreateRenderer();
    if (res == RES_ERROR_CREATE_WINDOW) {
        ReleaseWindow();
        ReleaseRenderer();
        return RES_ERROR_CREATE_WINDOW;
    }

    Texture tex;
    ResT res2 = LoadTextureFile(&tex, "good_image");
    // TODO: handle error

    // Game loop
    while (IsWindowRunning()) {
        StartScene(COLOR_WHITE);

        // DrawFirstRectangle(COLOR_PURPLE);
        DrawFirstTexture(&tex);

        EndScene();
    }

    ReleaseWindow();
    ReleaseRenderer();

    return 0;
}