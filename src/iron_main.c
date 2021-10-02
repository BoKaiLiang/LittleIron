#include "iron.h"

#define WINDOW_W 800
#define WINDOW_H 600

int main() {

    ResT init_res = CreateRenderWindow(WINDOW_W, WINDOW_H, "LittleIron", 60);
    if (init_res != RES_SUCCESS) {
        ReleaseRenderWindow();
        return init_res;
    }

    V2f pos = { 400.0f, 300.0f };
    V2f sz = { 100.0f, 100.0f };
    V2f sz2 = { 5.0f, 5.0f };

    V2f p0 = { 350.0f, 250.0f };
    V2f p1 = { 450.0f, 350.0f };

    while (IsWindowRunning()) {
        
        // EXIT the game
        if (IsKeyPressed(KEY_ESCAPE)) {
            CloseRenderWindow();
        }

        BeginRednering(COLOR_PINK);

            DrawRectangle(pos, sz, 0.0f, COLOR_RED);

            DrawLine(p0, p1, 5, COLOR_PURPLE);

            // DrawRectangleGrid(pos, sz, 0.0f, COLOR_GREEN);

            // DrawRectangleGrid(pos, sz, 0.0f, COLOR_GREEN);

        EndRendering();
    }

    ReleaseRenderWindow();

    return 0;
}

// create collision module later...
#if 0
#include "iron.h"

#include <stdio.h>
#include <stdbool.h>

#define WND_W 800
#define WND_H 600

#define G 400.0f

typedef struct {
    V2f pos;
    V2f speed;
    V2f size;
    float rotate_deg;
    Color c;

    V2f vert_pos[4];
    V2f norm_axis[4];
    V2f edges[4];
} Entity;

static void CalcVerticesPos(Entity* e) {
    Mat4 m = Mat4EulerRotate(MAT4_IDENTITY, e->rotate_deg);

    e->vert_pos[0].x = e->size.x / 2.0f;
    e->vert_pos[0].y = e->size.y / 2.0f;
    V4f v0 = { e->vert_pos[0].x, e->vert_pos[0].y, 0.0f, 1.0f };
    v0 = Mat4MulV4f(m, v0);
    e->vert_pos[0].x = v0.x + e->pos.x;
    e->vert_pos[0].y = v0.y + e->pos.y;

    e->vert_pos[1].x = -e->size.x / 2.0f;
    e->vert_pos[1].y =  e->size.y / 2.0f;
    V4f v1 = { e->vert_pos[1].x, e->vert_pos[1].y, 0.0f, 1.0f };
    v1 = Mat4MulV4f(m, v1);
    e->vert_pos[1].x = v1.x + e->pos.x;
    e->vert_pos[1].y = v1.y + e->pos.y;

    e->vert_pos[2].x = - e->size.x / 2.0f;
    e->vert_pos[2].y = - e->size.y / 2.0f;
    V4f v2 = { e->vert_pos[2].x, e->vert_pos[2].y, 0.0f, 1.0f };
    v2 = Mat4MulV4f(m, v2);
    e->vert_pos[2].x = v2.x + e->pos.x;
    e->vert_pos[2].y = v2.y + e->pos.y;

    e->vert_pos[3].x =  e->size.x / 2.0f;
    e->vert_pos[3].y = - e->size.y / 2.0f;
    V4f v3 = { e->vert_pos[3].x, e->vert_pos[3].y, 0.0f, 1.0f };
    v3 = Mat4MulV4f(m, v3);
    e->vert_pos[3].x = v3.x + e->pos.x;
    e->vert_pos[3].y = v3.y + e->pos.y;
}

static void CalcEdgesNorm(Entity* e) {
    e->edges[0] = V2fSub(e->vert_pos[0], e->vert_pos[1]); // 0: ( 237.5000, 237.5000 ) 1: ( 162.5000, 237.5000 ) -> (-75, 0) -> (0, 75) -> (0, 1)
    e->edges[1] = V2fSub(e->vert_pos[1], e->vert_pos[2]);
    e->edges[2] = V2fSub(e->vert_pos[2], e->vert_pos[3]);
    e->edges[3] = V2fSub(e->vert_pos[3], e->vert_pos[0]);

#if 0
    for (int i = 0; i < 4; i++) {
        LogInfo("Edge No %d: %s", (i + 1), V2fToString(e->edges[i]));
    }
#endif

    for ( int i = 0 ; i < 4; i++ ) {
        // e->norm_axis[i].x = e->pos.x - e->edges[i].y;
        // e->norm_axis[i].y = e->pos.y + e->edges[i].x;
        V2f normal = { -e->edges[i].y, e->edges[i].x  };
        e->norm_axis[i] = V2fNorm(normal);
    }

#if 0
    for (int i = 0; i < 4; i++) {
        LogInfo("Normal No %d: %s", (i + 1), V2fToString(e->norm_axis[i]));
    }
#endif
}

static float CalcProj(V2f ori, V2f proj_on) {
    float scalar = V2fDot(ori, proj_on);
    return scalar;
}

typedef struct Bound {
    float min, max;
} Bound;

static Bound CalcMinMaxVertexProj(Entity* e, V2f proj_on) {
    Bound res = { CalcProj(e->vert_pos[0], proj_on), CalcProj(e->vert_pos[0], proj_on) };
    for (int i = 1; i < 4; i++) {
        float proj = CalcProj(e->vert_pos[i], proj_on);

        if (res.max < proj) {
            res.max = proj;
        }

        if (res.min > proj) {
            res.min = proj;
        }
    }

    return res;
}

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
    box.pos.y = 450.0f;
    box.speed.x = 0.0f;
    box.speed.y = 0.0f;
    box.c = COLOR_WHITE;
    box.size.x = 450.0f;
    box.size.y = 60.0f;
    box.rotate_deg = 0.0f;
    CalcVerticesPos(&box);
    CalcEdgesNorm(&box);

    Entity p;
    p.pos.x = 200.0f;
    p.pos.y = 200.0f;
    p.speed.x = 300.0f;
    p.speed.y = 0.0f;
    p.c = COLOR_WHITE;
    p.size.x = 75.0f;
    p.size.y = 75.0f;
    p.rotate_deg = 0.0f;
    CalcVerticesPos(&p);
    CalcEdgesNorm(&p);

    SetTargetFPS(120);

    V2f debug_point_sz = { 8.0f, 8.0f };

    bool is_stand_on_floor = false;
    bool can_jump = false;

    // Game loop
    while (IsWindowRunning()) {

        bool move_player = false;
        bool move_box = false;

        float dt = GetDeltaTime();
        if (IsKeyDown(KEY_RIGHT)) {
            p.pos.x += p.speed.x * dt;
            move_player = true;
        } else if (IsKeyDown(KEY_LEFT)) {
            p.pos.x -= p.speed.x * dt;
            move_player = true;
        }

        if (IsKeyDown(KEY_SPACE) && can_jump) {
            LogInfo("Pressed jump");
            p.pos.y += (-1500.0f * dt);
            move_player = true;
            can_jump = false;
        }

        if (!is_stand_on_floor) {
            LogInfo("In air");
            p.pos.y += p.speed.y * dt;
            p.speed.y += G * dt;
            can_jump = false;
            move_player = true;
        } else {
            LogInfo("On ground");
            can_jump = true;
            p.speed.y = 0.0f; 
            move_player = true;
        }


#if 0
        // rotate
        if (IsKeyDown(KEY_E)) {
            p.rotate_deg += 3.0f;
            if (p.rotate_deg >= 360.0f) {
                p.rotate_deg = 0.0f;
            }
            move_player = true;
        }

        if (IsKeyDown(KEY_Q)) {
            box.rotate_deg += 3.0f;
            if (box.rotate_deg >= 360.0f) {
                box.rotate_deg = 0.0f;
            }
            move_box = true;
        }
#endif

#if 0
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
#endif


        // SAT
        // FAILED!! result is not correct
#if 1
        if (move_player) {
            CalcVerticesPos(&p);
            CalcEdgesNorm(&p);
        }

        if (move_box) {
            CalcVerticesPos(&box);
            CalcEdgesNorm(&box);
        }

        
        bool player_separate_axis_found = false;
        bool box_separate_axis_found = false;



        // iterate player rect edge normals
        for (int i = 0; i < 4; i++) {
            Bound player_bound = CalcMinMaxVertexProj(&p, p.norm_axis[i]);
            Bound box_bound = CalcMinMaxVertexProj(&box, p.norm_axis[i]);

            if (box_bound.min > player_bound.max || player_bound.min > box_bound.max) {
                player_separate_axis_found = true;
            }
        }

        // iterate box rect edge normals
        
        for (int i = 0; i < 4; i++) {
            Bound player_bound = CalcMinMaxVertexProj(&p, box.norm_axis[i]);
            Bound box_bound = CalcMinMaxVertexProj(&box, box.norm_axis[i]);

            if (box_bound.min > player_bound.max || player_bound.min > box_bound.max) {
                box_separate_axis_found = true;
            }
        }
        


        if (player_separate_axis_found || box_separate_axis_found) {
            p.c = COLOR_WHITE;
            box.c = COLOR_WHITE;

            is_stand_on_floor = false;
        } else {
            p.c = COLOR_PURPLE;
            box.c = COLOR_ORANGE;

            is_stand_on_floor = true;
        }

#endif   

        StartScene(COLOR_BLACK);
        BeginRendering();

        DrawRectangle(box.pos, box.size, box.rotate_deg, box.c);

#if 0
        for (int i = 0; i < 4; i++) {
            DrawRectangle(box.vert_pos[i], debug_point_sz, 0.0f, COLOR_BLUE);
        }

        for (int i = 0; i < 4; i++) {
            DrawRectangle(box.norm_axis[i], debug_point_sz, 0.0f, COLOR_CYAN);
        }
#endif
        DrawRectangle(p.pos, p.size, p.rotate_deg, p.c);

#if 0
        for (int i = 0; i < 4; i++) {
            DrawRectangle(p.vert_pos[i], debug_point_sz, 0.0f, COLOR_RED);
        }

        for (int i = 0; i < 4; i++) {
            DrawRectangle(p.norm_axis[i], debug_point_sz, 0.0f, COLOR_PINK);
        }
#endif

        EndRendering();
        EndScene();
    }

    ReleaseTexture(&tex);

    ReleaseRenderer();
    ReleaseWindow();

    return 0;
}
#endif