#ifndef _IRON_TYPES_H_
#define _IRON_TYPES_H_

#include <stdint.h>

#include "glad/gl.h"

typedef enum ResultType {
    RES_SUCCESS                     = 0,

    // error for create window
    RES_ERROR_CREATE_WINDOW         = 1,

    // error for start up glad
    RES_ERROR_LOAD_GLAD             = 2,

    // error for loading assets
    RES_ERROR_LOAD_VERTEX_SHADER        = 3,
    RES_ERROR_LOAD_FRAGMENT_SHADER      = 4,
    RES_ERROR_CREATE_SHADER_PROGRAM     = 5,
    RES_ERROR_GET_SHADER_ATTRIBUTE      = 6,
    RES_ERROR_GET_SHADER_UNIFORM        = 6,
    
    RES_ERROR_LOAD_IMAGE_FILE           = 7,
} ResT;

typedef struct Vec2f {
    float x, y;
} V2f;

typedef struct Color {
    uint8_t r, g, b, a; // uint8_t == unsigned char
} Color;

typedef struct Vec4f {
    union {
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };
    };
} V4f;

typedef enum ShaderAttribType {
    SHADER_ATTRIB_VEC2_POS,
    SHADER_ATTRIB_VEC2_TEXCOORD,
    SHADER_ATTRIB_VEC4_COLOR,
    SHADER_ATTRIB_SAMPLER2D_TEXTURE,

    MAX_SHADER_ATTTRIBS,
} ShaderAttribT;

typedef struct Shader {
    unsigned int id;
    int attribs_locations[MAX_SHADER_ATTTRIBS];
} Shader;

typedef enum GL_TextureFmt{
    TEX_FMT_WHITE_BLACK = GL_RED,
    TEX_FMT_GRAYSCALE   = GL_RG,
    TEX_FMT_RGB         = GL_RGB,
    TEX_FMT_RGBA        = GL_RGBA,
} TextureFmt;

typedef struct Texture {
    unsigned int id;
    int w, h;
    TextureFmt gl_fmt;
} Texture;

#endif // _IRON_TYPES_H_