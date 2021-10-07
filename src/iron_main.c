/*
    - simple editor, 2d array
    - collision detection, resolution
    - character movement
    - character jump
    - game camera movement
    - sptial partioning


    - audio
    - sprite animation
 */


#include "iron.h"

#define WINDOW_W 800
#define WINDOW_H 600

#define BLOCK_SZ ((V2f){40, 40})

#define MAP_W (WINDOW_W / 40)
#define MAP_H (WINDOW_H / 40)

static bool DEBUG_MODE = true;

static int G_MAP[MAP_H][MAP_W] = {
    { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0 },
    { 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
};

static struct {
    V2f pos;
    V2f dir;
    Color c;
    V2f speed;
} PLAYER;

static void InitPlayer() {
    PLAYER.pos.x = 2.0f;
    PLAYER.pos.y= 9.0f;

    PLAYER.dir.x = 1.0f;
    PLAYER.dir.y = 0.0f;

    PLAYER.c = COLOR_PURPLE;

    PLAYER.speed.x = 5.0f;
    PLAYER.speed.y = 0.0f;
}

int main() {

    ResT init_res = CreateRenderWindow(WINDOW_W, WINDOW_H, "LittleIron", 60);
    if (init_res != RES_SUCCESS) {
        ReleaseRenderWindow();
        return init_res;
    }

    InitPlayer();

    while (IsWindowRunning()) {
        
        float dt = GetDeltaTime();

        // EXIT the game
        if (IsKeyPressed(KEY_ESCAPE)) {
            CloseRenderWindow();
        }

        if (IsKeyPressed(KEY_F1)) {
            DEBUG_MODE = !DEBUG_MODE;
        }

        if (IsKeyPressed(KEY_R)) {
            InitPlayer(); // reset player state
        }

        // movement
        if (IsKeyDown(KEY_RIGHT)) {
            PLAYER.dir.x = 1.0f;
            PLAYER.pos.x += (PLAYER.dir.x * PLAYER.speed.x *dt);
        }

        if (IsKeyDown(KEY_LEFT)) {
            PLAYER.dir.x = -1.0f;
            PLAYER.pos.x += (PLAYER.dir.x * PLAYER.speed.x *dt);
        }

        BeginRednering(COLOR_GRAY);

            // draw playground
            for (int y = 0; y < MAP_H; y++) {
                for (int x = 0; x < MAP_W; x++) {
                    if (G_MAP[y][x] == 1) {
                        // LogInfo("x = %d, y = %d", x, y);
                        V2f pos = { x * 40, y * 40 };
                        DrawRectangle(pos, BLOCK_SZ, 0.0f, COLOR_GREEN);
                        if (DEBUG_MODE) {
                            DrawRectangleGrid(pos, BLOCK_SZ, 0.0f, COLOR_YELLOW);
                        }
                    }

                }
            }

            // draw player
            V2f player_draw_pos = V2fScalef(PLAYER.pos, 40);
            DrawRectangle(player_draw_pos, BLOCK_SZ, 0.0f, PLAYER.c);
            if (DEBUG_MODE) {
                V2f p0;
                p0.x = player_draw_pos.x + BLOCK_SZ.x / 2.0f;
                p0.y = player_draw_pos.y + BLOCK_SZ.y / 2.0f;
                V2f p1;
                p1.x = p0.x + PLAYER.dir.x * 20.0f;
                p1.y = p0.y + PLAYER.dir.y * 20.0f;
                DrawLine(p0, p1, 1, COLOR_WHITE);
                DrawRectangleGrid(player_draw_pos, BLOCK_SZ, 0.0f, COLOR_WHITE);
            }

            if (DEBUG_MODE) {
                const char* txt_player_pos = StringFormat("Player Pos: %s", V2fToString(PLAYER.pos));
                DrawText(txt_player_pos, V2F_ZERO, 0.5f, COLOR_WHITE);
                const char* txt_player_draw_pos = StringFormat("Player Draw Pos: %s", V2fToString(player_draw_pos));
                DrawText(txt_player_draw_pos, (V2f){ 0.0f, 32.0f }, 0.5f, COLOR_WHITE);
            }

        EndRendering();
    }

    ReleaseRenderWindow();

    return 0;
}

