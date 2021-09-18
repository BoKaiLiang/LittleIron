#ifndef _IRON_MATH_H_
#define _IRON_MATH_H_

#include "iron_types.h"

// ---------------------------------- //
//                Misc                //
// ---------------------------------- //
int GetRandInt(int min, int max);

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


#endif // _IRON_MATH_H_