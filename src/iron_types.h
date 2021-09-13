#ifndef _IRON_TYPES_H_
#define _IRON_TYPES_H_

#include <stdint.h>

typedef enum ResultType {
    RES_SUCCESS                     = 0,

    RES_ERROR_CREATE_WINDOW         = 1,
    RES_ERROR_LOAD_GLAD             = 2,
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

#endif // _IRON_TYPES_H_