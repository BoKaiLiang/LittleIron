#include "iron.h"

#include <stdio.h>

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

        // input test

        // LogInfo("delta time: %.6f", GetDeltaTime());

        if (IsKeyPressed(KEY_W))
            LogInfo("you pressed 'w'");
        if (IsKeyReleased(KEY_W))
            printf("you released 'w'\n");
        // if (IsKeyUp(KEY_W))
        //    printf("'w' is up\n");
        // if (IsKeyDown(KEY_W))
        //    printf("'w' is down\n");

        LogInfo("Color purple: %s", ColorToString(COLOR_PURPLE));

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            printf("you clicked left button\n");
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            printf("you released left button\n");

        V2f cursor = GetMousePosition();
        V2f tmp_cursor;
        if (cursor.x != tmp_cursor.x && cursor.y != tmp_cursor.y) {
            // LogInfo("cursor pos: %s", V2fToString(cursor));
            tmp_cursor = cursor;
        }

        float scroll = GetScrollYOffset();
        if (scroll != 0.0f) {
            printf("mouse scroll value: %.3f\n", scroll);
        }

        DrawFirstTexture(&tex, COLOR_WHITE);

        EndScene();
    }

    ReleaseTexture(&tex);

    ReleaseRenderer();
    ReleaseWindow();

    return 0;
}