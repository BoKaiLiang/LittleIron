#include "iron_math.h"

#include <stdlib.h>
#include <math.h>

// ---------------------------------- //
//                Misc                //
// ---------------------------------- //

int GetRandInt(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// ---------------------------------- //
//              Vector 2              //
// ---------------------------------- //

V2f V2fAdd(V2f a, V2f b) {
    return (V2f){ a.x + b.x, a.y + b.y };
}

V2f V2fSub(V2f a, V2f b) {
    return (V2f){ a.x - b.x, a.y - b.y };
}

V2f V2fScalef(V2f a, float f) {
    return (V2f){ a.x * f, a.y + f };
}

float V2fDot(V2f a, V2f b) {
    return a.x * b.x + a.y + b.y;
}

float V2fLen(V2f v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

V2f V2fNorm(V2f v) {
    return V2fScalef(v, (1.0f / V2fLen(v)));
}

float V2fDist(V2f a, V2f b) {
    return sqrtf((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

// ---------------------------------- //
//              Matrix                //
// ---------------------------------- //

V4f Mat4MulV4f(Mat4 mat, V4f v) {
    float product[4];
    for (int i = 0; i < 4; i++)
    {
        float x = mat.ary2d[0][i] * v.x;
        float y = mat.ary2d[1][i] * v.y;
        float z = mat.ary2d[2][i] * v.z;
        float w = mat.ary2d[3][i] * v.w;
        product[i] = x + y + z + w;
    }

    return (V4f) { product[0], product[1], product[2], product[3] };
}

Mat4 Mat4MulMat4(Mat4 m1, Mat4 m2) {
    Mat4 m;

    m.ary2d[0][0] 
        = m1.ary2d[0][0] * m2.ary2d[0][0] + m1.ary2d[1][0] * m2.ary2d[0][1] + m1.ary2d[2][0] * m2.ary2d[0][2] + m1.ary2d[3][0] * m2.ary2d[0][3];
    m.ary2d[0][1] 
        = m1.ary2d[0][1] * m2.ary2d[0][0] + m1.ary2d[1][1] * m2.ary2d[0][1] + m1.ary2d[2][1] * m2.ary2d[0][2] + m1.ary2d[3][1] * m2.ary2d[0][3];
    m.ary2d[0][2] 
        = m1.ary2d[0][2] * m2.ary2d[0][0] + m1.ary2d[1][2] * m2.ary2d[0][1] + m1.ary2d[2][2] * m2.ary2d[0][2] + m1.ary2d[3][2] * m2.ary2d[0][3];
    m.ary2d[0][3] 
        = m1.ary2d[0][3] * m2.ary2d[0][0] + m1.ary2d[1][3] * m2.ary2d[0][1] + m1.ary2d[2][3] * m2.ary2d[0][2] + m1.ary2d[3][3] * m2.ary2d[0][3];

    m.ary2d[1][0] 
        = m1.ary2d[0][0] * m2.ary2d[1][0] + m1.ary2d[1][0] * m2.ary2d[1][1] + m1.ary2d[2][0] * m2.ary2d[1][2] + m1.ary2d[3][0] * m2.ary2d[1][3];
    m.ary2d[1][1] 
        = m1.ary2d[0][1] * m2.ary2d[1][0] + m1.ary2d[1][1] * m2.ary2d[1][1] + m1.ary2d[2][1] * m2.ary2d[1][2] + m1.ary2d[3][1] * m2.ary2d[1][3];
    m.ary2d[1][2] 
        = m1.ary2d[0][2] * m2.ary2d[1][0] + m1.ary2d[1][2] * m2.ary2d[1][1] + m1.ary2d[2][2] * m2.ary2d[1][2] + m1.ary2d[3][2] * m2.ary2d[1][3];
    m.ary2d[1][3] 
        = m1.ary2d[0][3] * m2.ary2d[1][0] + m1.ary2d[1][3] * m2.ary2d[1][1] + m1.ary2d[2][3] * m2.ary2d[1][2] + m1.ary2d[3][3] * m2.ary2d[1][3];

    m.ary2d[2][0] 
        = m1.ary2d[0][0] * m2.ary2d[2][0] + m1.ary2d[1][0] * m2.ary2d[2][1] + m1.ary2d[2][0] * m2.ary2d[2][2] + m1.ary2d[3][0] * m2.ary2d[2][3];
    m.ary2d[2][1] 
        = m1.ary2d[0][1] * m2.ary2d[2][0] + m1.ary2d[1][1] * m2.ary2d[2][1] + m1.ary2d[2][1] * m2.ary2d[2][2] + m1.ary2d[3][1] * m2.ary2d[2][3];
    m.ary2d[2][2] 
        = m1.ary2d[0][2] * m2.ary2d[2][0] + m1.ary2d[1][2] * m2.ary2d[2][1] + m1.ary2d[2][2] * m2.ary2d[2][2] + m1.ary2d[3][2] * m2.ary2d[2][3];
    m.ary2d[2][3] 
        = m1.ary2d[0][3] * m2.ary2d[2][0] + m1.ary2d[1][3] * m2.ary2d[2][1] + m1.ary2d[2][3] * m2.ary2d[2][2] + m1.ary2d[3][3] * m2.ary2d[2][3];

    m.ary2d[3][0] 
        = m1.ary2d[0][0] * m2.ary2d[3][0] + m1.ary2d[1][0] * m2.ary2d[3][1] + m1.ary2d[2][0] * m2.ary2d[3][2] + m1.ary2d[3][0] * m2.ary2d[3][3];
    m.ary2d[3][1] 
        = m1.ary2d[0][1] * m2.ary2d[3][0] + m1.ary2d[1][1] * m2.ary2d[3][1] + m1.ary2d[2][1] * m2.ary2d[3][2] + m1.ary2d[3][1] * m2.ary2d[3][3];
    m.ary2d[3][2] 
        = m1.ary2d[0][2] * m2.ary2d[3][0] + m1.ary2d[1][2] * m2.ary2d[3][1] + m1.ary2d[2][2] * m2.ary2d[3][2] + m1.ary2d[3][2] * m2.ary2d[3][3];
    m.ary2d[3][3] 
        = m1.ary2d[0][3] * m2.ary2d[3][0] + m1.ary2d[1][3] * m2.ary2d[3][1] + m1.ary2d[2][3] * m2.ary2d[3][2] + m1.ary2d[3][3] * m2.ary2d[3][3];

    return m;
}