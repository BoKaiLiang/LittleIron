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

static bool RayVsRect(V2f ray_origin, V2f ray_dir, Entity target, V2f* contact_point, V2f* contact_norm, float* hit_near) {
    contact_norm->x = 0.0f;
    contact_norm->y = 0.0f;

    contact_norm->x = 0.0f;
    contact_norm->y = 0.0f;

    V2f invdir;
    invdir.x = 1.0f / ray_dir.x;
    invdir.y = 1.0f / ray_dir.y;

    V2f near = V2fMul(V2fSub(target.pos, ray_origin), invdir);
    V2f far = V2fMul(V2fSub((V2f){ target.pos.x + target.rec.w, target.pos.y + target.rec.h }, ray_origin), invdir);

    if (near.x == 0.0f || near.y == 0.0f) {
        return false;
    }

    if (far.x == 0.0f || far.y == 0.0f) {
        return false;
    }

    if (near.x > far.x) SwapFloat(&near.x, &far.x);
    if (near.y > far.y) SwapFloat(&near.y, &far.y);

    // core condition
    if (near.x > far.y || near.y > far.x) return false;

    *hit_near = MaxFloat(near.x, near.y);
    float hit_far = MinFloat(near.x, near.y);

    if (hit_far < 0.0f) {
        return false;
    }

    *contact_point = V2fAdd(ray_origin, V2fScalef(ray_dir, *hit_near));

    if (near.x > near.y) {
        if (invdir.x < 0) {
            contact_point->x = 1.0f;
            contact_point->y = 0.0f;
        } else {
            contact_point->x = -1.0f;
            contact_point->y = 0.0f;
        }
    } else if (near.x < near.y) {
        if (invdir.y < 0) {
            contact_point->x = 0.0f;
            contact_point->y = 1.0f;
        } else {
            contact_point->x = 0.0f;
            contact_point->y = -1.0f;
        }
    }

    return true;
}

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

    Entity obstacle;
    EntityCreate(&obstacle, (V2f){ 200.0f, 200.0f }, V2fScalef(BLOCK_SZ, 3.0f));

    // TEST!
    V2f start_point = V2F_ZERO;
    V2f end_point = V2F_ZERO;
    V2f ray_dir;
    V2f contact_point;
    V2f contact_norm;
    float hit_near;
    bool is_hit = true;

    Color c = COLOR_WHITE;

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

        if (IsKeyDown(KEY_F2)) {
            start_point = V2F_ZERO;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            start_point = GetMousePosition();
        }

        EntityMoveOffset(&player, movement_offset.x, movement_offset.y);
        
        if (start_point.x != 0.0f && start_point.y != 0.0f) {
            end_point = GetMousePosition();
            ray_dir = V2fSub(end_point, start_point);

            // why `hit_near <= 1.0f`
            if (RayVsRect(start_point, ray_dir, obstacle, &contact_point, &contact_norm, &hit_near) && hit_near <= 1.0f) {
                c = COLOR_YELLOW;
            } else {
                c = COLOR_WHITE;
            }

#if 0
            // ------------- caluate ray vs rect -------------
            ray_dir = V2F_ZERO;
            contact_point = V2F_ZERO;
            contact_norm = V2F_ZERO;
            hit_near = false;
            is_hit = true;

            ray_dir = V2fSub(end_point, start_point);

            V2f min_pos = obstacle.pos;
            V2f near = V2fSub(min_pos, start_point);
            near.x /= ray_dir.x;
            near.y /= ray_dir.y;

            V2f max_pos = { obstacle.pos.x + obstacle.rec.w, obstacle.pos.y + obstacle.rec.h };
            V2f far = V2fSub( max_pos, start_point);
            far.x /= ray_dir.x;
            far.y /= ray_dir.y;

            if (near.x > far.x) {
                float tmp = near.x;
                near.x = far.x;
                far.x = tmp;
            }

            if (near.y > far.y) {
                float tmp = near.y;
                near.y = far.y;
                far.y = tmp;
            }

            if (near.x > far.y || near.y > far.x) {
                is_hit = false;
            }

            if (near.x > near.y) {
                hit_near = near.x;
            } else {
                hit_near = near.y;
            }

            contact_point = V2fAdd(V2fScalef(ray_dir, hit_near), start_point);

            if (near.x > near.y) {
                if (ray_dir.x < 0.0f) {
                    contact_norm.x = 1.0f;
                    contact_norm.y = 0.0f;
                } else {
                    contact_norm.x = -1.0f;
                    contact_norm.y = 0.0f;
                }
            } else if (near.x < near.y) {
                if (ray_dir.x < 0.0f) {
                    contact_norm.x = 0.0f;
                    contact_norm.y = -1.0f;
                } else {
                    contact_norm.x = 0.0f;
                    contact_norm.y = 1.0f;
                }
            }

            if (hit_near > 1.0f) {
                is_hit = false;
            }
            // ------------- end -------------
#endif
        }

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

            DrawRectangle(obstacle.pos, obstacle.sz, 0.0f, COLOR_WHITE);

            if (start_point.x != 0.0f, start_point.y != 0.0f) {
                DrawRectangle(start_point, (V2f){ 5.0f, 5.0f }, 0.0f, COLOR_BLUE);
                DrawLine(start_point, end_point, 2, COLOR_BLUE);
                DrawRectangleGrid(obstacle.pos, obstacle.sz, 0.0f, c);

                // Color c = COLOR_WHITE;
                if (is_hit) {
                    // c = COLOR_RED;
                    
                    // DrawRectangle(contact_point, (V2f){ 10.0f, 10.0f }, 0.0f, COLOR_ORANGE);
                    // V2f contact_point_end = (V2f){ contact_point.x + contact_norm.x * -10.0f, contact_point.y + contact_norm.y * -10.0f };
                    // DrawLine(contact_point, contact_point_end, 2.0f, COLOR_PURPLE);
                }
            }

        EndRendering();
    }

    ReleaseRenderWindow();

    return 0;
}

