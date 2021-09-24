#include "iron.h"

#include <stdio.h>

#define WND_W 800
#define WND_H 600

typedef struct {
    V2f pos;
    V2f speed;
    V2f size;
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

    Entity box;
    box.pos.x = 400.0f;
    box.pos.y = 300.0f;
    box.speed.x = 0.0f;
    box.speed.y = 0.0f;
    box.c = COLOR_WHITE;
    box.size.x = 200.0f;
    box.size.y = 200.0f;

    Entity p;
    p.pos.x = 200.0f;
    p.pos.y = 200.0f;
    p.speed.x = 300.0f;
    p.speed.y = 300.0f;
    p.c = COLOR_WHITE;
    p.size.x = 75.0f;
    p.size.y = 75.0f;

    V2f wnd_sz =  GetWindowWH();

    SetTargetFPS(60);

    // Game loop
    while (IsWindowRunning()) {

        float dt = GetDeltaTime();
        if (IsKeyDown(KEY_RIGHT)) {
            p.pos.x += p.speed.x * dt;
        } else if (IsKeyDown(KEY_LEFT)) {
            p.pos.x -= p.speed.x * dt;
        }

        if (IsKeyDown(KEY_DOWN)) {
            p.pos.y += p.speed.y * dt;
        } else if (IsKeyDown(KEY_UP)) {
            p.pos.y -= p.speed.y * dt;
        }

        // AABB
        float box_x_min = box.pos.x - box.size.x / 2.0f;
        float box_x_max = box.pos.x + box.size.x / 2.0f;
        float box_y_min = box.pos.y - box.size.y / 2.0f;
        float box_y_max = box.pos.y + box.size.y / 2.0f;

        float p_x_min = p.pos.x - p.size.x / 2.0f;
        float p_x_max = p.pos.x + p.size.x / 2.0f;
        float p_y_min = p.pos.y - p.size.y / 2.0f;
        float p_y_max = p.pos.y + p.size.y / 2.0f;

        if (p_x_max >= box_x_min && p_x_min <= box_x_max
            && p_y_max >= box_y_min && p_y_min <= box_y_max) {
            p.c = COLOR_PURPLE;
            box.c = COLOR_ORANGE;
        } else {
            p.c = COLOR_WHITE;
            box.c = COLOR_WHITE;
        }

        StartScene(COLOR_BLACK);
        BeginRendering();

        DrawRectangle(box.pos, box.size, 0.0f, box.c);

        DrawRectangle(p.pos, p.size, 0.0f, p.c);

        EndRendering();
        EndScene();
    }

    ReleaseTexture(&tex);

    ReleaseRenderer();
    ReleaseWindow();

    return 0;
}