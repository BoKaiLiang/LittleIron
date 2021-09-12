#include "iron.h"

#define WND_W 800
#define WND_H 600

int main() {

    // Init the window
    ResT res = CreateWindow(WND_W, WND_H, "Little Iron");
    if (res == RES_ERROR_CREATE_WINDOW) {
        ReleaseWindow();
        return RES_ERROR_CREATE_WINDOW;
    }

    // Game loop
    while (IsWindowRunning()) {
        StartScene();


        EndScene();
    }

    ReleaseWindow();

    return 0;
}