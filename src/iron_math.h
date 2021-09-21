#ifndef _IRON_MATH_H_
#define _IRON_MATH_H_

#include "iron_types.h"

// ---------------------------------- //
//                Misc                //
// ---------------------------------- //
int GetRandInt(int min, int max);
float DegToRad(float degree);
float RadToRad(float rad);

// ---------------------------------- //
//              Vector 2              //
// ---------------------------------- //

V2f V2fAdd(V2f a, V2f b);
V2f V2fSub(V2f a, V2f b);
V2f V2fScalef(V2f a, float f);
float V2fDot(V2f a, V2f b);
float V2fLen(V2f v);
V2f V2fNorm(V2f v);
float V2fDist(V2f a, V2f b);

// ---------------------------------- //
//              Matrix                //
// ---------------------------------- //

V4f Mat4MulV4f(Mat4 mat, V4f v);
Mat4 Mat4MulMat4(Mat4 m1, Mat4 m2);

// ---------------------------------- //
//           Transformation           //
// ---------------------------------- //

// [iron_math] orthogrphics matrix 
// --   | 2/(r-l)        0         0      -(r+l)/(r-l) |
// --   |    0        2/(t-b)      0      -(t+b)/(t-b) |
// --   |    0           0      -2/(f-n)  -(f+n)/(f-n) |
// --   |    0           0         0            1      |
Mat4 Mat4Ortho(float left, float right, float bottom, float top, float n, float f);

Mat4 Mat4Translate(Mat4 origin, float x, float y);
Mat4 Mat4EulerRotate(Mat4 origin, float angle);
Mat4 Mat4Scale(Mat4 origin, float x, float y);

#endif // _IRON_MATH_H_