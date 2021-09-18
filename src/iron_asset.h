#ifndef _IRON_ASSET_H_
#define _IRON_ASSET_H_

#include "iron_util.h"
#include "iron_types.h"

// [iron_asset] load shader text content for creating shader program
ResT LoadShaderCode(Shader* shader, const char* vertex_code, const char* fragment_code);

// [iron_asset] release shader
void ReleaseShader(Shader* shader);

// [iron_asset] load image file and store to opengl texture
ResT LoadTextureFile(Texture* texture, const char* file_name);

// [iron_asset] release texture
void ReleaseTexture(Texture* texture);

#endif // _IRON_ASSET_H_