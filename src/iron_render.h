#ifndef _IRON_RENDER_H_
#define _IRON_RENDER_H_

#include "iron_types.h"

// [iron_render] create/initialize 2d renderer
ResT CreateRenderer();

// [iron_render] create/initialize 2d renderer
void ReleaseRenderer();

// [iron_render] TEST: draw a simple rectangle...
void DrawRectangle(Color c);

// [iron_render] TEST: draw texture
void DrawTexture(Texture texture, Color c);

#endif // _IRON_RENDER_H_