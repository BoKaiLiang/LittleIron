#ifndef _IRON_RENDER_H_
#define _IRON_RENDER_H_

#include "iron_types.h"

// [iron_render] create/initialize 2d renderer
ResT CreateRenderer();

// [iron_render] create/initialize 2d renderer
void ReleaseRenderer();

// [iron_render] TEST: draw a simple rectangle...
void DrawFirstRectangle(Color c);

// [iron_render] TEST: draw texture
void DrawFirstTexture(Texture* texture);

#endif // _IRON_RENDER_H_