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

static Rect tiles[MAP_W * MAP_H];
static bool tiles_collided[MAP_W * MAP_H];
static int tiles_count = 0;

// this is copy from: https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Videos/OneLoneCoder_PGE_Rectangles.cpp
static bool RayVsRect(V2f ray_origin, V2f ray_dir, Rect target, V2f* contact_point, V2f* contact_norm, float* hit_near) {
    contact_norm->x = 0.0f;
    contact_norm->y = 0.0f;

    contact_norm->x = 0.0f;
    contact_norm->y = 0.0f;

    V2f invdir;
    invdir.x = 1.0f / ray_dir.x;
    invdir.y = 1.0f / ray_dir.y;

    V2f tar_pos = { target.x, target.y }; 
    V2f near = V2fMul(V2fSub(tar_pos, ray_origin), invdir);
    V2f far = V2fMul(V2fSub((V2f){ target.x + target.w, target.y + target.h }, ray_origin), invdir);

    if (isnan(near.x) || isnan(near.y)) {
        return false;
    }

    if (isnan(far.x) || isnan(far.y)) {
        return false;
    }

    if (near.x > far.x) SwapFloat(&near.x, &far.x);
    if (near.y > far.y) SwapFloat(&near.y, &far.y);

    // core condition
    if (near.x > far.y || near.y > far.x) return false;

    *hit_near = MaxFloat(near.x, near.y);
    float hit_far = MinFloat(far.x, far.y);

    if (hit_far < 0.0f) {
        return false;
    }

    *contact_point = V2fAdd(ray_origin, V2fScalef(ray_dir, *hit_near));

    if (near.x > near.y) {
        if (invdir.x < 0) {
            contact_norm->x = 1.0f;
            contact_norm->y = 0.0f;
        } else {
            contact_norm->x = -1.0f;
            contact_norm->y = 0.0f;
        }
    } else if (near.x < near.y) {
        if (invdir.y < 0) {
            contact_norm->x = 0.0f;
            contact_norm->y = 1.0f;
        } else {
            contact_norm->x = 0.0f;
            contact_norm->y = -1.0f;
        }
    }

    return true;
}

static bool DyRectVsRect(Rect dy_rec, V2f speed, float dt, Rect static_rec, V2f* contact_point, V2f* contact_norm, float* hit_near) {

    if (speed.x == 0.0f && speed.y == 0)
        return false;

    Rect expand_rec;
    expand_rec.x = static_rec.x - dy_rec.w / 2.0f;
    expand_rec.y = static_rec.y - dy_rec.h / 2.0f;
    expand_rec.w = static_rec.w + dy_rec.w;
    expand_rec.h = static_rec.h + dy_rec.h;

    V2f dy_rec_center = { dy_rec.x + dy_rec.w / 2.0f, dy_rec.y + dy_rec.h / 2.0f };
    V2f dir = V2fScalef(speed, dt);

    if (RayVsRect(dy_rec_center, dir, expand_rec, contact_point, contact_norm, hit_near)) {
        return (*hit_near) >= 0.0f && (*hit_near) < 1.0f;
    }

    return false;
}

int main() {

    ResT init_res = CreateRenderWindow(WINDOW_W, WINDOW_H, "LittleIron", 60);
    if (init_res != RES_SUCCESS) {
        ReleaseRenderWindow();
        return init_res;
    }

    Rect player = { 200.0f, 100.0f, BLOCK_UNIT_SZ, BLOCK_UNIT_SZ };
    Rect block = { 150.0f, 200.0f, 200.0f, 200.0f };

    // init tiles
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            if (G_MAP[y][x] == 1) {
                tiles_count += 1;

                V2f tile_pos = { x * BLOCK_UNIT_SZ, y * BLOCK_UNIT_SZ };
                tiles[tiles_count - 1].pos = tile_pos;
                tiles[tiles_count - 1].sz = BLOCK_SZ;
                LogInfo("Tile - %i: pos: %s, size: %s", tiles_count - 1, V2fToString(tiles[tiles_count - 1].pos), V2fToString(tiles[tiles_count - 1].sz));

                tiles_collided[tiles_count - 1] = false;
            }
        }
    }

    V2f player_vel = V2F_ZERO;

    V2f contact_point = V2F_ZERO;
    V2f contact_norm = V2F_ZERO;
    float hit_near = 0.0f;
    int jump = 1;

    while (IsWindowRunning()) {
        
        float dt = GetDeltaTime();

        // EXIT the game
        if (IsKeyPressed(KEY_ESCAPE)) {
            CloseRenderWindow();
        }

        if (IsKeyPressed(KEY_F1)) {
            DEBUG_MODE = !DEBUG_MODE;
        }

        contact_point = V2F_ZERO;
        contact_norm = V2F_ZERO;
        hit_near = 0.0f;

        if (IsKeyDown(KEY_RIGHT)) {
            player_vel.x = 300.0f;
        } else if (IsKeyUp(KEY_RIGHT)) {
            player_vel.x = 0.0f;
        }

        if (IsKeyDown(KEY_LEFT)) {
            player_vel.x = -300.0f;
        } else if (IsKeyUp(KEY_RIGHT)) {
            player_vel.x = 0.0f;
        }

        player_vel.y += 300.0f * dt;

        if (IsKeyDown(KEY_SPACE) && jump > 0) {
            // LogInfo("Press jump");
            jump = 0;
            player_vel.y += -250.0f;
        }
        
        int tile_index = 0;
        for (int y = 0; y < MAP_H; y++) {
            for (int x = 0; x < MAP_W; x++) {
                if (G_MAP[y][x] == 1) {
                    if (DyRectVsRect(player, player_vel, dt, tiles[tile_index], &contact_point, &contact_norm, &hit_near)) {

                        V2f abs_vel = V2fAbs(player_vel);
                        V2f move = V2fMul(contact_norm, abs_vel);
                        player_vel.x += move.x * (1 - hit_near);
                        player_vel.y += move.y * (1 - hit_near);

                        if (player_vel.y == 0) {
                            jump = 1;
                            // LogInfo("Can jump");
                        }
                    }
                    tile_index += 1;
                }
            }
        }

        player.pos = V2fAdd(player.pos, V2fScalef(player_vel, dt));

        BeginRednering(COLOR_GRAY);

            // DrawRectangle(block.pos, block.sz, 00.0f, COLOR_WHITE);
            tile_index = 0;
            for (int y = 0; y < MAP_H; y++) {
                for (int x = 0; x < MAP_W; x++) {
                    if (G_MAP[y][x] == 1) {
                        DrawRectangle(tiles[tile_index].pos, tiles[tile_index].sz, 0.0f, COLOR_GREEN);
                        tile_index += 1;
                    }
                }
            }

            DrawRectangle(player.pos, player.sz, 00.0f, COLOR_PURPLE);
        
        EndRendering();
    }

    ReleaseRenderWindow();

    return 0;
}

