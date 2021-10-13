#include "iron_entity.h"

#include <math.h>

#include "iron_util.h"

void EntityCreate(Entity* e, V2f pos, V2f sz) {
    if (e == NULL) {
        LogError("EntityCreate, Arguemnt is null");
        return;
    }
    
    e->pos = pos;
    e->sz = sz;

    e->dir = V2F_ZERO;
    
    e->rec.x = e->pos.x;
    e->rec.y = e->pos.y;
    e->rec.w = sz.x;
    e->rec.h = sz.y;
    // LogInfo("Rect: %s", RectToString(e->rec));
    
    e->center_pos.x = e->pos.x + sz.x / 2.0f;
    e->center_pos.y = e->pos.x + sz.y / 2.0f;
}

void EntitySetPostiion(Entity* e, V2f pos) {
    if (e != NULL) {
        LogError("EntitySetPosition, Arguemnt is null");
        return;
    }

    e->pos = pos;

    e->dir = V2F_ZERO;

    e->center_pos.x = e->pos.x + e->sz.x / 2.0f;
    e->center_pos.y = e->pos.y + e->sz.y / 2.0f;

    e->rec.x = e->pos.x;
    e->rec.y = e->pos.y;
}

void EntityMoveOffset(Entity* e, float xoffset, float yoffset) {
    if (e == NULL) {
        LogError("EntityMoveOffset, Arguemnt is null");
        return;
    }

    e->pos.x += xoffset;
    e->pos.y += yoffset;

    if (xoffset < 0.0f) {
        e->dir.x = -1.0f;
    } else if (xoffset > 0.0f) {
        e->dir.x = 1.0f;
    } else {
        e->dir.x = 0.0f;
    }

    if (yoffset < 0.0f) {
        e->dir.y = -1.0f;
    } else if (yoffset > 0.0f) {
        e->dir.y = 1.0f;
    } else {
        e->dir.y = 0.0f;
    }

    e->center_pos.x = e->pos.x + e->sz.x / 2.0f;
    e->center_pos.y = e->pos.y + e->sz.y / 2.0f;

    e->rec.x = e->pos.x;
    e->rec.y = e->pos.y;
}

bool EntityIsCollided(Entity* e1, Entity* e2) {
    float e1_xmin = e1->rec.x;
    float e1_xmax = e1->rec.x + e1->rec.w;
    float e1_ymin = e1->rec.y;
    float e1_ymax = e1->rec.y + e1->rec.h;

    float e2_xmin = e2->rec.x;
    float e2_xmax = e2->rec.x + e2->rec.w;
    float e2_ymin = e2->rec.y;
    float e2_ymax = e2->rec.y + e2->rec.h;

    return e1_xmax >= e2_xmin && e2_xmax >= e1_xmin
        && e1_ymax >= e2_ymin && e2_ymax >= e1_ymin;
}