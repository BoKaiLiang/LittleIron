#ifndef _IRON_ENTITY_H_
#define _IRON_ENTITY_H_

#include <stdbool.h>

#include "iron_types.h"

void EntityCreate(Entity* e, V2f pos, V2f sz);

void EntitySetPostiion(Entity* e, V2f pos);

void EntityMoveOffset(Entity* e, float xoffset, float yoffset);

bool EntityIsCollided(Entity* e1, Entity* e2);

#endif // _IRON_ENTITY_H_