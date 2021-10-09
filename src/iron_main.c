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

#define BLOCK_UNIT_SZ 40
#define BLOCK_SZ ((V2f){40, 40})

#define MAP_W (WINDOW_W / 40)
#define MAP_H (WINDOW_H / 40)

#define G 10.0f

#define DEBUG_BLOCK_SZ ((V2f){ 3.0f, 3.0f })

static bool DEBUG_MODE = true;

static int G_MAP[MAP_H][MAP_W] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
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

static Entity player;
static V2f player_speed;
static Entity tiles[MAP_W * MAP_H];
static bool tiles_collided[MAP_W * MAP_H];
static int tiles_count = 0;

int main() {

    ResT init_res = CreateRenderWindow(WINDOW_W, WINDOW_H, "LittleIron", 60);
    if (init_res != RES_SUCCESS) {
        ReleaseRenderWindow();
        return init_res;
    }

    // init player
    V2f player_start_pos = { 2.0f, 10.0f };
    player_start_pos = V2fScalef(player_start_pos, BLOCK_UNIT_SZ);
    EntityCreate(&player, player_start_pos, BLOCK_SZ);
    player_speed.x = 100.0f;
    player_speed.y = 100.0f;
    
    // init tiles
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            if (G_MAP[y][x] == 1) {
                tiles_count += 1;

                V2f tile_pos = { x * BLOCK_UNIT_SZ, y * BLOCK_UNIT_SZ };
                EntityCreate(&tiles[tiles_count - 1], tile_pos, BLOCK_SZ);

                tiles_collided[tiles_count - 1] = false;
            }
        }
    }

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
            player.pos = player_start_pos;
            EntityMoveOffset(&player, 0.0f, 0.0f); // reset player state
        }

        // movement
        V2f movement_offset = V2F_ZERO;
        if (IsKeyDown(KEY_RIGHT)) {
            movement_offset.x += player_speed.x * dt;
        }

        if (IsKeyDown(KEY_LEFT)) {
            movement_offset.x -= player_speed.x * dt;
        }

        // NOTE: move up, this is a test for collision
        if (IsKeyDown(KEY_UP)) {
            movement_offset.y -= player_speed.y * dt;
        }

        // NOTE: move down, this is a test for collision
        if (IsKeyDown(KEY_DOWN)) {
            movement_offset.y += player_speed.y * dt;
        }

        EntityMoveOffset(&player, movement_offset.x, movement_offset.y);

        // collision detection
        for (int i = 0; i < tiles_count; i++) {
            tiles_collided[i] = false;
            bool isCollided = EntityIsCollided(&player, &tiles[i]);
            if (isCollided) {
                LogInfo("Collided, player with index: %d", i);
                tiles_collided[i] = true;;
            }
        }


        BeginRednering(COLOR_GRAY);

            // draw playground
            int tile_index = 0;
            for (int y = 0; y < MAP_H; y++) {
                for (int x = 0; x < MAP_W; x++) {
                    if (G_MAP[y][x] == 1) {
                        DrawRectangle(tiles[tile_index].pos, tiles[tile_index].sz, 0.0f, COLOR_GREEN);
                        if (DEBUG_MODE) {
                            Color c = COLOR_WHITE;
                            if (tiles_collided[tile_index])
                                c = COLOR_RED;
                            DrawRectangleGrid(tiles[tile_index].pos, BLOCK_SZ, 0.0f, c);
                        }

                        tile_index += 1;
                    }

                }
            }

            // draw player
            DrawRectangle(player.pos, player.sz, 0.0f, COLOR_PURPLE);
            if (DEBUG_MODE) {
                DrawRectangleGrid(player.pos, player.sz, 0.0f, COLOR_WHITE);
                V2f p0 = player.center_pos;
                V2f p1;
                p1.x = player.center_pos.x + (player.sz.x / 2.0f * player.dir.x);
                p1.y = player.center_pos.y + (player.sz.y / 2.0f * player.dir.y);
                DrawLine(p0, p1, 2, COLOR_WHITE);
            }

        EndRendering();
    }

    ReleaseRenderWindow();

    return 0;
}

