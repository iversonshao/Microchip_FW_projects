/* ************************************************************************** */
/** Math3D.h - Math Utilities for OLED 3D Graphics

  @Company
    Microchip Technology Inc.

  @File Name
    Math3D.h

  @Summary
    Header file for 2D/3D math functions used in OLED graphics rendering.

  @Description
    This header defines structures and math utility functions for 2D/3D vector and
    matrix operations, used in rendering and animation on monochrome OLED displays.

    Math Features:
    - 3D vector math: scaling, dot/cross product, length, normalization, lerp
    - 3x3 matrix math: rotation matrix generation, multiplication, transformation
    - 3D to 2D projection: perspective/orthographic blend

    Provided API Functions:
    - Vec3Scale, Vec3Dot, Vec3Add, Vec3Sub, Vec3Cross
    - Vec3Normalize, Vec3Length, Vec3LengthSquared, Vec3Lerp
    - Mat3RotateAxisAngle, Mat3Multiply, Mat3MulVec3,
    - Mat3Transform, Mat3RotateZ, Mat3Orthonormalize
    - Project3DTo2D_Perspective

 */
/* ************************************************************************** */

#ifndef _MATH3D_H    /* Guard against multiple inclusion */
#define _MATH3D_H

#ifdef __cplusplus
extern "C" {
#endif

/* ************************************************************************** */
/* Section: Macros                                                           */
/* ************************************************************************** */
#define PI 3.14159265358979323846f
#define MAX(a, b)               ((a) > (b) ? (a) : (b))
#define MIN(a, b)               ((a) > (b) ? (b) : (a))

/* ************************************************************************** */
/* Section: Data Types                                                       */
/* ************************************************************************** */

// 3D vector structure
typedef struct
{
    float x, y, z;
} Vec3;

// 2D vector structure
typedef struct
{
    float x, y;
} Vec2;

// 3x3 matrix structure
typedef struct
{
    float m[3][3];
} Mat3;

/* ************************************************************************** */
/* Section: Public Interface Functions                                       */
/* ************************************************************************** */
Vec3 Vec3Scale(Vec3 v, float s);
float Vec3Dot(Vec3 a, Vec3 b);
Vec3 Vec3Add(Vec3 a, Vec3 b);
Vec3 Vec3Sub(Vec3 a, Vec3 b);
Vec3 Vec3Cross(Vec3 a, Vec3 b);
Vec3 Vec3Normalize(Vec3 v);
float Vec3Length(Vec3 v);
float Vec3LengthSquared(Vec3 v);
Vec3 Vec3Lerp(Vec3 a, Vec3 b, float t);
Mat3 Mat3Identity( void );
Mat3 Mat3RotateAxisAngle(Vec3 axis, float angle);
Mat3 Mat3Multiply(Mat3 A, Mat3 B);
Vec3 Mat3MulVec3(Mat3 M, Vec3 v);
Vec3 Mat3Transform(Mat3 m, Vec3 v);
Mat3 Mat3RotateX(float angle);
Mat3 Mat3RotateY(float angle);
Mat3 Mat3RotateZ(float angle);
void Mat3Orthonormalize(Mat3* m);

void Project3DTo2D_Perspective(Vec3 v, int cx, int cy, float scale, float* out_x, float* out_y, float perspective_weight);

#ifdef __cplusplus
}
#endif

#endif /* _MATH3D_H */

/* *****************************************************************************
 End of File
 */
