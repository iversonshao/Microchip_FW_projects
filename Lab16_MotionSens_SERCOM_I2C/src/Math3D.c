/* ************************************************************************** */
/** Math3D.c - 3D Math and Projection Utilities for OLED Rendering

  @Company
    Microchip Technology Inc.

  @File Name
    Math3D.c

  @Summary
    Math utilities for 2D/3D rendering and transformation used in OLED graphics.

  @Description
    Implements vector and matrix operations, projection methods, and Z-buffer line drawing
    for real-time 3D rendering on embedded OLED displays.

    Key Features Implemented:
    - Vec3 structure operations: dot, cross, scale, length, normalize, add, subtract
    - Mat3 structure operations: axis-angle rotation, multiplication, transform
    - 3D to 2D projection with perspective and orthographic blending

    Corresponding API Functions:
    - Vec3Scale, Vec3Dot, Vec3Add, Vec3Sub, Vec3Cross
    - Vec3Normalize, Vec3Length, Vec3LengthSquared, Vec3Lerp
    - Mat3RotateAxisAngle, Mat3Multiply, Mat3MulVec3,
    - Mat3Transform, Mat3RotateZ, Mat3Orthonormalize
    - Project3DTo2D_Perspective
 */
/* ************************************************************************** */
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "definitions.h"
#include "Math3D.h"

/* ************************************************************************** */
/* Vector Operations                                                          */
/* ************************************************************************** */

// Scale a vector by a scalar: v * s
Vec3 Vec3Scale(Vec3 v, float s)
{
    return (Vec3){ v.x * s, v.y * s, v.z * s };
}

// Compute the dot product of two vectors: a ?b
float Vec3Dot(Vec3 a, Vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Add two vectors: a + b
Vec3 Vec3Add(Vec3 a, Vec3 b)
{
    return (Vec3){ a.x + b.x, a.y + b.y, a.z + b.z };
}

// Subtract two vectors: a - b
Vec3 Vec3Sub(Vec3 a, Vec3 b)
{
    return (Vec3){ a.x - b.x, a.y - b.y, a.z - b.z };
}

// Compute the cross product of two vectors: a ?b
Vec3 Vec3Cross(Vec3 a, Vec3 b)
{
    return (Vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// Normalize a vector to unit length (returns zero vector if length is near zero)
Vec3 Vec3Normalize(Vec3 v)
{
    float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len < 1e-8f) return (Vec3){ 0, 0, 0 }; // Prevent division by zero
    return Vec3Scale(v, 1.0f / len);
}

// Compute the magnitude (length) of a vector
float Vec3Length(Vec3 v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Compute the squared length of a vector
float Vec3LengthSquared(Vec3 v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

// Linearly interpolate between vectors a and b by factor t (0 <= t <= 1)
Vec3 Vec3Lerp(Vec3 a, Vec3 b, float t)
{
    return (Vec3){
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t
    };
}

/* ************************************************************************** */
/* Matrix Operations                                                          */
/* ************************************************************************** */
// An identity matrix is a square matrix
Mat3 Mat3Identity( void )
{
    Mat3 m = {{
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    }};
    return m;
}

// Generates a 3x3 rotation matrix from an axis-angle representation
Mat3 Mat3RotateAxisAngle(Vec3 axis, float angle)
{
    axis = Vec3Normalize(axis);
    float c = cosf(angle);
    float s = sinf(angle);
    float t = 1.0f - c;
    float x = axis.x, y = axis.y, z = axis.z;

    Mat3 R = {{
        {t * x * x + c,     t * x * y - s * z, t * x * z + s * y},
        {t * x * y + s * z, t * y * y + c,     t * y * z - s * x},
        {t * x * z - s * y, t * y * z + s * x, t * z * z + c    }
    }};
    return R;
}

// Multiplies two 3x3 matrices: C = A * B
Mat3 Mat3Multiply(Mat3 A, Mat3 B)
{
    Mat3 C;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            C.m[i][j] = 0;
            for (int k = 0; k < 3; k++)
                C.m[i][j] += A.m[i][k] * B.m[k][j];
        }
    return C;
}

// Multiplies a 3x3 matrix with a 3D vector: result = M * v
Vec3 Mat3MulVec3(Mat3 M, Vec3 v)
{
    return (Vec3){
        M.m[0][0] * v.x + M.m[0][1] * v.y + M.m[0][2] * v.z,
        M.m[1][0] * v.x + M.m[1][1] * v.y + M.m[1][2] * v.z,
        M.m[2][0] * v.x + M.m[2][1] * v.y + M.m[2][2] * v.z
    };
}

// Transforms a vector using a 3x3 matrix (same as Mat3MulVec3)
Vec3 Mat3Transform(Mat3 m, Vec3 v)
{
    return Mat3MulVec3(m, v);
}

// Generate a 3x3 rotation matrix for a rotation about the X axis
Mat3 Mat3RotateX(float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    Mat3 m = {{
        {1, 0, 0},
        {0, c, -s},
        {0, s,  c}
    }};
    return m;
}

// Generate a 3x3 rotation matrix for a rotation about the Y axis
Mat3 Mat3RotateY(float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    Mat3 m = {{
        { c, 0, s},
        { 0, 1, 0},
        {-s, 0, c}
    }};
    return m;
}

// Generate a 3x3 rotation matrix for a rotation about the Z axis
Mat3 Mat3RotateZ(float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    Mat3 R = { .m = {
        { c, -s, 0 },
        { s,  c, 0 },
        { 0,  0, 1 }
    }};
    return R;
}

// This ensures that the matrix represents a valid rotation:
// - Columns become mutually perpendicular (orthogonal)
// - Each column is normalized (unit length)
// This function uses the Gram-Schmidt process to re-orthonormalize
// the first two column vectors, then reconstructs the third via cross product.
void Mat3Orthonormalize(Mat3* m)
{
    // Extract column vectors from the matrix
    // x = first column, y = second column
    Vec3 x = { m->m[0][0], m->m[1][0], m->m[2][0] };
    Vec3 y = { m->m[0][1], m->m[1][1], m->m[2][1] };
    Vec3 z;

    // Normalize x to ensure it has unit length
    float len_x = sqrtf(x.x * x.x + x.y * x.y + x.z * x.z);
    if (len_x > 1e-6f)
    {
        x.x /= len_x;
        x.y /= len_x;
        x.z /= len_x;
    }

    // Make y orthogonal to x using projection subtraction:
    // y = y - proj_x(y) = y - dot(x, y) * x
    float dot_xy = x.x * y.x + x.y * y.y + x.z * y.z;
    y.x -= dot_xy * x.x;
    y.y -= dot_xy * x.y;
    y.z -= dot_xy * x.z;

    // Normalize y after making it orthogonal to x
    float len_y = sqrtf(y.x * y.x + y.y * y.y + y.z * y.z);
    if (len_y > 1e-6f)
    {
        y.x /= len_y;
        y.y /= len_y;
        y.z /= len_y;
    }

    // Compute z as the cross product of x and y to complete the orthonormal basis
    z.x = x.y * y.z - x.z * y.y;
    z.y = x.z * y.x - x.x * y.z;
    z.z = x.x * y.y - x.y * y.x;

    // Store orthonormalized vectors back into matrix as columns
    m->m[0][0] = x.x; m->m[1][0] = x.y; m->m[2][0] = x.z;
    m->m[0][1] = y.x; m->m[1][1] = y.y; m->m[2][1] = y.z;
    m->m[0][2] = z.x; m->m[1][2] = z.y; m->m[2][2] = z.z;
}


/* ************************************************************************** */
/* Projection                                     */
/* ************************************************************************** */
// Projects a 3D point to 2D screen coordinates with perspective/orthographic blending
void Project3DTo2D_Perspective(Vec3 v, int cx, int cy, float scale, float* out_x, float* out_y, float perspective_weight)
{
    float raw_perspective = 1.0f / (v.z + 2.0f); // Shift Z to avoid divide-by-zero
    float blend = perspective_weight + (1.0f - perspective_weight) * raw_perspective;
    *out_x = cx + v.x * scale * blend;
    *out_y = cy - v.y * scale * blend; // Flip Y for screen space
}

/* *****************************************************************************
 End of File
 */
