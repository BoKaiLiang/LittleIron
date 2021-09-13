#include "iron_util.h"

// [iron_util] covert color to vec4f
V4f ColorToVec4f(Color c) {
    return (V4f) {
        c.r / 255.0f,
        c.g / 255.0f,
        c.b / 255.0f,
        c.a / 255.0f,
    };
}