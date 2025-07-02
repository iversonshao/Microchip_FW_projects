/* ************************************************************************** */
/** MotionAPP.c - High-Level OLED Graphic Application API Source

  @Company
    Microchip Technology Inc.

  @File Name
    MotionAPP.c

  @Summary
    Implementation of high-level graphics applications for monochrome OLED displays.

  @Description
    This file implements application-layer drawing functions that leverage the core
    GraphicLib to produce animated and decorative content. These include an animated
    clock (analog and digital), bitmap splash screen with motion, and example graphic
    elements for demonstration or product branding purposes.

    Key Features Implemented:
    - Analog clock with hour, minute, and second hands
    - Digital clock synchronized with analog clock
    - Smooth splash screen animation from any direction
    - Decorative drawings such as Doraemon face and Microchip logo

    Corresponding API Functions:
    - `DrawBalanceBall'
    - `DrawHorizontalBubble'
    - `DrawRotationCube'
    - `ViewRotationCube'
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "definitions.h"
#include "GraphicLib.h"
#include "Math3D.h"
#include "MotionApp.h"
#include "myprintf.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
#define DISABLE_DICE_FACE       0       // Disable 3D Cube Dice Face rendeing (because the MCU computing power)

// ==================== Object Parameters ====================
#define BALL_RADIUS             9       // Radius of the ball in pixels
#define DICE_RADIUS             23      // Radius of the dice cube in pixels
#define CUBE_RADIUS             38      // Radius of the gyro cube in pixels

// ==================== Physics Parameters ====================
#define FRICTION_WATER          0.2f    // Friction value for a bouncy bubble-like behavior
#define FRICTION_SURFACE        0.9f    // Friction value for a stable balance-ball, rolling dice behavior
#define ACCEL_SCALE             0.02f   // Scale factor to convert accelerometer data to velocity
#define ACCEL_THRESHOLD         50.0f   // Threshold to consider accelerometer input as near-zero
#define ACCEL_FILTER            0.2f    // Low-pass filter factor for smoothing accelerometer data
#define VELOCITY_THRESHOLD      0.8f    // Threshold to consider velocity result as smaller for obj bouncing
#define VELOCITY_REDUCE_STEP    0.1f    // Speed decrease per frame when slow
#define VELOCITY_LOCK_THRESHOLD 0.5f    // Threshold to zero the velocity for lock object in boundary
#define CENTERING_FORCE         0.1f    // Strength of the centering force when the system is level
#define CENTERING_FORCE_DISABLE 0.0f    // Value to disable the centering force
#define BOUNCE_DAMPING          1.0f    // Coefficient of restitution (1 = perfect bounce)
#define BOUNCE_DAMPING_DISABLE  0.0f    // Value to disable bounce (no energy retained)

// ==================== Object Physics State ========================
static float obj_x = SCREEN_WIDTH  / 2.0f; // Object's current X position
static float obj_y = SCREEN_HEIGHT / 2.0f; // Object's current Y position
static float vel_x = 0; // Object's velocity along X-axis
static float vel_y = 0; // Object's velocity along Y-axis

// Object's initial rotation matrix (identity matrix ? no rotation)
static Mat3 obj_rotation_matrix = {
    {
        {1, 0, 0 },
        {0, 1, 0 },
        {0, 0, 1 }
    }
};

// Camera's initial rotation matrix (identity matrix ? no rotation)
static Mat3 camera_rotation_matrix = {
    {
        {1, 0, 0 },
        {0, 1, 0 },
        {0, 0, 1 }
    }
};

// Camera direction vector (looking along the negative Z-axis)
static Vec3 camera_dir      = { 0.0f, 0.0f,  -1.0f };
static Vec3 camera_position = { 0.0f, 0.0f, 100.0f };
static Vec3 obj_position    = { 0.0f, 0.0f,   0.0f }; // assume cube is centered at origin

// Z-buffer to store the nearest depth value for each pixel
#define USE_INTEGER_ZBUFFER 1
#if USE_INTEGER_ZBUFFER
// use int16 will save 16KByte SRAM
#define Z_INTEGER_SCALE 3000.0f
static int16_t z_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
#define Z_INFINITY 0x7FFF; // -32768 ~ +32767
#else
#define Z_INTEGER_SCALE 1.0f
static float z_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
#define Z_INFINITY INFINITY
#endif

// Clears the Z-buffer by setting all values to infinity (maximum depth)
static void _ClearZBuffer( )
{
    for( int y = 0; y < SCREEN_HEIGHT; y++ )
        for( int x = 0; x < SCREEN_WIDTH; x++ )
            z_buffer[y][x] = Z_INFINITY;
}

// Cube vertex positions in object space (8 corners of a unit cube centered at origin)
static const Vec3 cube_vertices[8] = {
    {-0.5f, -0.5f, -0.5f },
    { 0.5f, -0.5f, -0.5f },
    { 0.5f,  0.5f, -0.5f },
    {-0.5f,  0.5f, -0.5f },
    {-0.5f, -0.5f,  0.5f },
    { 0.5f, -0.5f,  0.5f },
    { 0.5f,  0.5f,  0.5f },
    {-0.5f,  0.5f,  0.5f }
};

// Vertex index pairs representing the 12 edges of the cube
static const uint8_t cube_edges[12][2] = {
    { 0, 1 },
    { 1, 2 },
    { 2, 3 },
    { 3, 0 }, // Bottom face edges
    { 4, 5 },
    { 5, 6 },
    { 6, 7 },
    { 7, 4 }, // Top face edges
    { 0, 4 },
    { 1, 5 },
    { 2, 6 },
    { 3, 7 } // Vertical edges connecting top and bottom
};

// Face indices that share each edge (used for face-related calculations like visibility)
static const uint8_t edge_faces[12][2] = {
    { 0, 2 }, // Edge {0,1}
    { 0, 5 }, // Edge {1,2}
    { 0, 3 }, // Edge {2,3}
    { 0, 4 }, // Edge {3,0}
    { 1, 2 }, // Edge {4,5}
    { 1, 5 }, // Edge {5,6}
    { 1, 3 }, // Edge {6,7}
    { 1, 4 }, // Edge {7,4}
    { 2, 4 }, // Edge {0,4}
    { 2, 5 }, // Edge {1,5}
    { 5, 3 }, // Edge {2,6}
    { 4, 3 }  // Edge {3,7}
};

// Vertex indices that define each face of the cube (6 quads)
static const uint8_t cube_faces[6][4] = {
    { 0, 1, 2, 3 }, // Back face
    { 4, 5, 6, 7 }, // Front face
    { 0, 1, 5, 4 }, // Bottom face
    { 2, 3, 7, 6 }, // Top face
    { 0, 3, 7, 4 }, // Left face
    { 1, 2, 6, 5 }  // Right face
};

static Vec3 face_normals[6] = {
    { 0, 0,  1 }, { 0, 0, -1 },
    { 0, 1,  0 }, { 0, -1, 0 },
    { 1, 0,  0 }, { -1, 0, 0 }
};

static Vec3 face_centers[6] = {
    { 0,       0,  0.5f }, {     0,     0, -0.5f },
    { 0,    0.5f,     0 }, {     0, -0.5f,     0 },
    { 0.5f,    0,     0 }, { -0.5f,     0,     0 }
};

// Mapping from face index (0?5) to dice number (1?6)
static const int face_index_to_dice_number[6] = {
    1, 6, 5, 2, 3, 4
};

// Return the index (0~5) of the dice face currently facing up (+Z)
static int _GetTopFaceIndex(Mat3 rotation)
{
    int top_face = 0;
    float max_dot = -1.0f;

    for (int i = 0; i < 6; i++)
    {
        Vec3 world_normal = Mat3MulVec3(rotation, face_normals[i]);

        // Use Z component to measure alignment with "up" direction
        float dot = world_normal.z;

        if (dot > max_dot)
        {
            max_dot = dot;
            top_face = i;
        }
    }

    return top_face;
}


/**
 * @brief Generate a rotation matrix that orients a specified face of the object upward (+Z axis)
 *        while preserving a given yaw rotation around the Z axis.
 *
 * This function returns a rotation matrix which rotates the object such that the face indicated
 * by `face_index` is aligned pointing upwards along the +Z axis.
 * After orienting the specified face upwards, it applies an additional rotation (yaw) around
 * the Z axis to preserve the object's yaw angle.
 *
 * @param face_index Index of the face to bring upward (0 to 5).
 *                   Face indices correspond to the cube faces:
 *                   0: +Z (Face 1),
 *                   1: -Z (Face 6),
 *                   2: -Y (Face 5),
 *                   3: +Y (Face 2),
 *                   4: +X (Face 3),
 *                   5: -X (Face 4).
 * @param yaw        The yaw angle in radians to apply after aligning the face upwards.
 *
 * @return Mat3      The resulting rotation matrix that aligns the given face upward
 *                   with the specified yaw rotation applied.
 *
 * @details
 * The function first generates a base rotation matrix to align the specified face with the +Z axis.
 * This involves rotations of 0? ?0? or 180?around the X or Y axes depending on the face.
 * Then, it applies an additional rotation around the Z axis to maintain the yaw angle.
 *
 * This is useful for smoothly snapping an object's orientation to have a particular face up,
 * while retaining rotation around the vertical axis.
 */
static Mat3 _AlignFaceToUp(int face_index, float yaw)
{
    Mat3 base;

    switch (face_index)
    {
    case 0: // Face 1 (+Z): already up
        base = Mat3Identity();
        break;
    case 1: // Face 6 (-Z): flip over X
        base = Mat3RotateX(PI);
        break;
    case 2: // Face 5 (-Y): rotate +90?around X
        base = Mat3RotateX(PI / 2);
        break;
    case 3: // Face 2 (+Y): rotate -90?around X
        base = Mat3RotateX(-PI / 2);
        break;
    case 4: // Face 3 (+X): rotate -90?around Y
        base = Mat3RotateY(-PI / 2);
        break;
    case 5: // Face 4 (-X): rotate +90?around Y
        base = Mat3RotateY(PI / 2);
        break;
    default:
        base = Mat3Identity();
        break;
    }

    // Apply additional yaw around Z axis
    Mat3 yaw_rot = Mat3RotateZ(yaw);
    return Mat3Multiply(yaw_rot, base);
}


/**
 * @brief Estimate the yaw angle (rotation around the Z-axis) from the object's rotation matrix.
 *
 * This function computes the yaw angle based on the current 3x3 rotation matrix `R`
 * and the index of the top-facing face of the object. Since the object's orientation
 * affects which axis should be considered "forward," this function chooses an appropriate
 * reference direction vector depending on which face is currently on top.
 *
 * @param R        The 3x3 rotation matrix representing the object's orientation.
 * @param top_face Index of the top-facing face (0 to 5).
 *                 Faces correspond to cube faces with known local axes.
 *
 * @return The estimated yaw angle in radians, calculated as atan2(y, x) of the chosen direction vector.
 *
 * @details
 * The function selects a direction vector in the XY plane based on the top face:
 * - For faces on +Z or -Z or -Y top, the +X axis is used as forward.
 * - For +Y top, the -X axis is used as forward.
 * - For +X top, the -Z axis is used as forward.
 * - For -X top, the +Z axis is used as forward.
 *
 * The chosen vector is extracted from the rotation matrix columns accordingly,
 * then the yaw angle is computed using atan2 on the XY components.
 *
 * This angle can be used to preserve or manipulate the object's yaw rotation
 * independently of pitch and roll.
 */
static float _EstimateYawFromMatrix(Mat3 R, int top_face)
{
    Vec3 dir;

    switch (top_face)
    {
    case 0: // +Z face on top ? use +X as front
    case 1: // -Z face on top ? use -X as front
    case 2: // -Y face on top ? use +X
        dir.x = R.m[0][0];
        dir.y = R.m[1][0];
        break;

    case 3: // +Y face on top ? use -X
        dir.x = -R.m[0][0];
        dir.y = -R.m[1][0];
        break;

    case 4: // +X face on top ? use -Z as front
        dir.x = -R.m[0][2];
        dir.y = -R.m[1][2];
        break;

    case 5: // -X face on top ? use +Z as front
        dir.x = R.m[0][2];
        dir.y = R.m[1][2];
        break;

    default: // fallback to +X
        dir.x = R.m[0][0];
        dir.y = R.m[1][0];
        break;
    }

    return atan2f(dir.y, dir.x);
}


/**
 * @brief Smoothly aligns the cube's rotation so that the current top face points upward.
 *
 * This function is called when the cube (or dice) is nearly stationary. It determines
 * the currently top-facing face of the cube and smoothly interpolates the rotation matrix
 * to align that face with the +Z axis, preserving the current yaw angle.
 *
 * @param vx Velocity in X direction, used to determine motion state.
 * @param vy Velocity in Y direction, used to determine motion state.
 *
 * @return The dice number (1?6) corresponding to the upward face after alignment.
 *
 * @details
 * - If motion speed is below `VELOCITY_THRESHOLD`, a linear interpolation (lerp) is
 *   performed between the current and target aligned rotation matrix.
 * - The yaw angle is estimated from the current orientation and preserved in the final alignment.
 * - The rotation matrix is orthonormalized after blending to prevent drift or skewing.
 * - If the speed is extremely small, it directly snaps to the aligned rotation matrix.
 *
 * @note Depends on `_GetTopFaceIndex()`, `_EstimateYawFromMatrix()`, and `_AlignFaceToUp()`.
 * @note Uses global `obj_rotation_matrix` and `face_index_to_dice_number[]`.
 */
static int _CubeFaceUpRotation(float vx, float vy)
{
    float speed = sqrtf(vx * vx + vy * vy);

    // Determine current top-facing face index (0 to 5)
    int current_top_face_index = _GetTopFaceIndex(obj_rotation_matrix);

    if (speed < VELOCITY_THRESHOLD)
    {
        // === Perform alignment when nearly stationary ===

        const float lerp_factor = 0.05f;

        // Estimate current yaw angle (around Z-axis)
        float yaw = _EstimateYawFromMatrix(obj_rotation_matrix, current_top_face_index);

        // Compute target rotation that puts current face on top (+Z), preserving yaw
        Mat3 target_rotation = _AlignFaceToUp(current_top_face_index, yaw);

        // Linearly interpolate each element of the rotation matrix
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                obj_rotation_matrix.m[i][j] =
                    obj_rotation_matrix.m[i][j] * (1.0f - lerp_factor) +
                    target_rotation.m[i][j] * lerp_factor;

        // Normalize to maintain orthonormal basis
        Mat3Orthonormalize(&obj_rotation_matrix);

        // Snap directly to target if nearly stopped
        if (speed < 0.0001f)
        {
            obj_rotation_matrix = target_rotation;
        }
    }

    // Return the dice number (1?6) corresponding to the upward face
    return face_index_to_dice_number[current_top_face_index];
}


/**
 * @brief Updates the 3D object's rotation based on 2D motion vector velocity.
 *
 * This function applies an incremental rotation to the object?s rotation matrix
 * based on motion velocity (vx, vy). When the object is moving, a rotation axis is
 * computed perpendicular to the velocity vector, and an angular rotation is applied
 * proportionally to the motion speed.
 *
 * @param vx Horizontal velocity component of the motion vector.
 * @param vy Vertical velocity component of the motion vector.
 *
 * @return true if rotation was applied due to sufficient speed; false if speed is too low.
 *
 * @note This function does not handle face snapping when stationary.
 *       See the extended version with face_up alignment for that functionality.
 */
static bool _UpdateObjectRotation(float vx, float vy)
{
    // Compute motion speed magnitude
    float speed = sqrtf(vx * vx + vy * vy);

    // Apply rotation only if speed is above threshold
    if (speed >= VELOCITY_THRESHOLD)
    {
        // === Physics-based rotation ===

        // Define axis in XY plane perpendicular to motion vector
        Vec3 axis = { vx, vy, 0 };

        // Normalize the axis
        float axis_length = sqrtf(axis.x * axis.x + axis.y * axis.y);
        if (axis_length > 0.0001f)
        {
            axis.x /= axis_length;
            axis.y /= axis_length;
        }

        // Compute rotation angle proportional to speed
        float angle = speed * 0.05f;

        // Create incremental rotation matrix
        Mat3 R = Mat3RotateAxisAngle(axis, angle);

        // Apply rotation to object's current orientation
        obj_rotation_matrix = Mat3Multiply(R, obj_rotation_matrix);

        // Keep rotation matrix orthonormal to avoid drift
        Mat3Orthonormalize(&obj_rotation_matrix);

        return true;
    }

    // Speed is too low; no rotation applied
    return false;
}


/**
 * @brief Update the camera's rotation matrix using gyroscope and accelerometer data.
 *
 * This function updates a 3D camera's orientation by combining angular velocity
 * from a gyroscope and gravitational direction from an accelerometer. It first
 * applies gyroscope-based incremental rotation for smooth and responsive movement.
 * Then, it uses accelerometer data to gently correct drift in the pitch and roll axes
 * by aligning the camera's down direction with the sensed gravity vector.
 *
 * @param ax Accelerometer reading on the X-axis (linear acceleration including gravity)
 * @param ay Accelerometer reading on the Y-axis
 * @param az Accelerometer reading on the Z-axis
 * @param gx Gyroscope reading on the X-axis (angular velocity)
 * @param gy Gyroscope reading on the Y-axis
 * @param gz Gyroscope reading on the Z-axis
 *
 * @note The gyroscope rotation axis is constructed by rearranging and negating components
 *       to align with the coordinate system used by the rotation matrix.
 *
 * @note Small angular speeds (below 40.0) are ignored to filter out gyroscope noise.
 *
 * @note The gyroscope rotation angle is scaled by a tuning factor (0.00004) to control sensitivity.
 *
 * @note Accelerometer correction gently realigns the camera's down direction (Z-axis)
 *       with the gravity vector using a small-angle rotation, smoothing long-term drift.
 *
 * @note Only pitch and roll are corrected; yaw drift cannot be compensated using accelerometer alone.
 *
 * @note Both gyroscope and accelerometer rotations are applied by pre-multiplying the camera rotation matrix.
 */
static void _UpdateCameraRotation(float ax, float ay, float az, float gx, float gy, float gz)
{
    // === [1] Gyroscope-Based Rotation ===
    // Construct axis of rotation (remapped for visual alignment)
    Vec3 gyro_axis = { gx, gy, gz };
    float gyro_speed = sqrtf(gx * gx + gy * gy + gz * gz);

    // Apply only if above noise threshold
    if (gyro_speed > 40.0f) {
        float angle = gyro_speed * 0.0000433f;  // Tuning sensitivity
        Mat3 R_gyro = Mat3RotateAxisAngle(gyro_axis, angle);
        camera_rotation_matrix = Mat3Multiply(R_gyro, camera_rotation_matrix);
    }

    // === [2] Accelerometer-Based Gravity Correction ===
    // Step 1: Normalize accelerometer vector to get gravity direction
    Vec3 gravity = Vec3Normalize((Vec3){ ax, ay, az });

    // Step 2: Get camera's current Z-axis (down direction in world space)
    Vec3 cam_down = Mat3Transform(camera_rotation_matrix, (Vec3){ 0, 0, -1 });

    // Step 3: Compute angle between camera-down and gravity vector
    float dot = Vec3Dot(cam_down, gravity);
    dot = fmaxf(fminf(dot, 1.0f), -1.0f);  // Clamp to prevent NaNs
    float angle = acosf(dot);

    if (angle > 0.001f && angle < 1.0f) {
        Vec3 correction_axis = Vec3Normalize(Vec3Cross(cam_down, gravity));

        // Apply small correction rotation
        float correction_strength = 0.1f;  // Tuning parameter for smoothness
        Mat3 R_acc = Mat3RotateAxisAngle(correction_axis, angle * correction_strength);
        camera_rotation_matrix = Mat3Multiply(R_acc, camera_rotation_matrix);
    }
}


/**
 * @brief Rasterize a triangle into the Z-buffer without drawing any color.
 *
 * This function performs triangle rasterization using barycentric coordinates
 * to fill the triangle's coverage in the Z-buffer. It interpolates the depth (z)
 * values across the triangle surface and updates the Z-buffer only if the
 * interpolated depth is closer than the existing value, enabling hidden surface
 * removal for subsequent rendering.
 *
 * @param p0    Screen coordinates of the first vertex (Vec2)
 * @param p1    Screen coordinates of the second vertex (Vec2)
 * @param p2    Screen coordinates of the third vertex (Vec2)
 * @param z0    Depth value at the first vertex
 * @param z1    Depth value at the second vertex
 * @param z2    Depth value at the third vertex
 *
 * @note The function assumes a global or externally defined 2D array `z_buffer`
 *       indexed as [y][x], representing depth values for each pixel on screen.
 *
 * @note Degenerate triangles with area close to zero are skipped.
 *
 * @note Coordinates are clamped to screen boundaries defined by SCREEN_WIDTH and SCREEN_HEIGHT.
 */
static void _FillTriangleZBuffer( Vec2 p0, Vec2 p1, Vec2 p2, float z0, float z1, float z2 )
{
    // Compute bounding box of the triangle
    int minX = ( int )fminf(fminf(p0.x, p1.x), p2.x);
    int maxX = ( int )fmaxf(fmaxf(p0.x, p1.x), p2.x);
    int minY = ( int )fminf(fminf(p0.y, p1.y), p2.y);
    int maxY = ( int )fmaxf(fmaxf(p0.y, p1.y), p2.y);

    // Clamp to screen boundaries
    minX = fmaxf(minX, 0);
    maxX = fminf(maxX, SCREEN_WIDTH - 1);
    minY = fmaxf(minY, 0);
    maxY = fminf(maxY, SCREEN_HEIGHT - 1);

    // Calculate triangle area (for barycentric coordinates)
    float area = (p1.x - p0.x)*(p2.y - p0.y) - (p2.x - p0.x)*(p1.y - p0.y);
    if( fabsf(area) < 1e-5f ) return; // Degenerate triangle, skip

    // Loop through bounding box pixels
    for( int y = minY; y <= maxY; y++ )
    {
        for( int x = minX; x <= maxX; x++ )
        {
            // Compute barycentric weights
            float w0 = ((p1.x - x)*(p2.y - y) - (p2.x - x)*(p1.y - y)) / area;
            float w1 = ((p2.x - x)*(p0.y - y) - (p0.x - x)*(p2.y - y)) / area;
            float w2 = 1.0f - w0 - w1;

            // If pixel is inside the triangle
            if( w0 >= 0 && w1 >= 0 && w2 >= 0 )
            {
                // Interpolate z-depth
                float z = w0 * z0 + w1 * z1 + w2 * z2;
                // Update Z-buffer if this pixel is closer
                if( z*Z_INTEGER_SCALE < z_buffer[y][x] )
                {
                    z_buffer[y][x] = z*Z_INTEGER_SCALE;
                }
            }
        }
    }
}


/**
 * @brief Determine if a face on a 3D object is visible from the camera viewpoint.
 *
 * This function checks whether a polygonal face of a 3D object is facing towards
 * the camera by computing the dot product between the face normal vector and
 * the vector from the face center to the camera position. It assumes the face normal
 * and center are given in the object's local space and transforms them to world space.
 *
 * @param R                 Rotation matrix representing the object's orientation in world space
 * @param obj_pos           World space position of the object
 * @param face_normal_local Face normal vector in the object's local coordinate system (should be normalized)
 * @param face_center_local Center point of the face in the object's local coordinate system
 * @param camera_pos        Camera position in world coordinates
 *
 * @return true if the face is visible (facing towards the camera), false otherwise.
 *
 * @note The face is considered visible if the angle between the face normal (in world space)
 *       and the vector from the face center to the camera is less than 90 degrees,
 *       i.e., their dot product is positive.
 */
static bool _IsFaceVisible(Mat3 R, Vec3 obj_pos, Vec3 face_normal_local, Vec3 face_center_local, Vec3 camera_pos)
{
    // Transform local face normal to world space
    Vec3 face_normal_world = Mat3MulVec3(R, face_normal_local);

    // Transform local face center to world space
    Vec3 face_center_world = Mat3MulVec3(R, face_center_local);
    face_center_world = Vec3Add(face_center_world, obj_pos);

    // Vector from face to camera
    Vec3 to_camera = Vec3Sub(camera_pos, face_center_world);

    // Dot product: if positive, face is visible
    return Vec3Dot(face_normal_world, to_camera) > 0.0f;
}

#define BUZZER_TICK_ON 0
static void _BuzzerTick( bool OnOff )
{
    if( BUZZER_TICK_ON )
    {
        // Output the Tick sound to Buzzer (Jumper to DACO.0 PA02)
        PORT_REGS->GROUP[0].PORT_DIR |= 0x0004U;
        PORT_REGS->GROUP[0].PORT_PINCFG[2] = 0x0U;
        if( OnOff ) (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 2U));
        else        (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 2U));
    }
}


/**
 * @brief Handles boundary collision and velocity control for one axis (X or Y).
 *
 * Checks if the object has collided with the screen boundary on a given axis, applies bounce
 * response, and conditionally locks the velocity to 0 when bouncing has stopped.
 *
 * @param pos                   Pointer to the object's position (x or y)
 * @param vel                   Pointer to the object's velocity (vel_x or vel_y)
 * @param radius                Radius of the object
 * @param limit                 Maximum size of the screen (width or height)
 * @param lock_neg              Pointer to flag indicating lock on negative direction (left or up)
 * @param lock_pos              Pointer to flag indicating lock on positive direction (right or down)
 * @param last_vel              Pointer to last velocity on this axis (for bounce stop detection)
 * @param bounce_enable         Whether bounce behavior is currently enabled
 * @param damping               Damping factor applied when bouncing
 * @param velocity_lock_threshold Threshold of velocity change under which object is considered stopped and locked
 */
void _HandleBoundary( float* pos, float* vel, float radius, int limit,
                      bool* lock_neg, bool* lock_pos, float* last_vel,
                      bool bounce_enable, float damping,
                      float velocity_lock_threshold )
{
    bool hit = true;

    if( *pos <= radius )
    {
        *pos = radius;
        *vel = bounce_enable ? -*vel * damping : 0.0f;

        if( fabsf(*vel) - fabsf(*last_vel) < velocity_lock_threshold )
        {
            *lock_neg = true;
            *lock_pos = false;
        }
        if( *lock_neg )
            *vel = 0.0f;
        else
            *last_vel = *vel;
    }
    else if( *pos >= limit - radius - 1 )
    {
        *pos = limit - radius - 1;
        *vel = bounce_enable ? -*vel * damping : 0.0f;

        if( fabsf(*vel) - fabsf(*last_vel) < velocity_lock_threshold )
        {
            *lock_neg = false;
            *lock_pos = true;
        }
        if( *lock_pos )
            *vel = 0.0f;
        else
            *last_vel = *vel;
    }
    else
    {
        *lock_neg = *lock_pos = false;
        *last_vel = *vel;
        hit = false;
    }
    _BuzzerTick(hit);
}


/**
 * @brief Simulates a physics-driven object affected by gravity using accelerometer input.
 *
 * This function models a 2D object (e.g., ball or dice) moving under the influence of
 * raw accelerometer input, with optional features including:
 * - Low-pass filtered sensor data
 * - Velocity and position integration
 * - Optional centering force when device is level
 * - Friction-based damping
 * - Collision detection and bounce against screen edges
 *
 * @param ax              Raw accelerometer reading along the X-axis
 * @param ay              Raw accelerometer reading along the Y-axis
 * @param obj_radius      Radius of the object (used for boundary collision)
 * @param center_force    Centering force strength; use CENTERING_FORCE_DISABLE to disable
 * @param friction        Damping coefficient applied to velocity each frame
 * @param bounce_damping  Bounce energy retention coefficient (0.0?1.0); use BOUNCE_DAMPING_DISABLE to disable
 * @param reset           If true, resets object position, velocity, and accelerometer offset
 * @param layer           Drawing layer ID (reserved for external rendering control)
 *
 * @details
 * - On reset, the object's position is centered, velocity cleared, and accelerometer offset calibrated.
 * - Accelerometer input is smoothed using a low-pass filter and scaled before applying to velocity.
 * - If centering is enabled and the device is level, a restoring force pulls the object toward screen center.
 * - Friction is applied to gradually reduce motion.
 * - If velocity falls below a threshold and no centering is used, velocity decays to zero over time.
 * - If bouncing is enabled and the object hits screen edges, it rebounds with reduced energy.
 * - Locked-edge behavior prevents jitter by freezing velocity when speed is below a small threshold.
 *
 * @note Uses global variables: obj_x, obj_y, vel_x, vel_y, and several motion state flags.
 */
void _GravityObject( short ax, short ay, float obj_radius, float center_force, float friction, float bounce_damping, bool reset, uint8_t layer )
{
    bool is_stationary = false; // Flag to indicate if the object is stationary
    bool velocity_small = false; // Flag to indicate if the object is in very small velocity
    static float offset_ax = 0.0f, offset_ay = 0.0f;
    static float filtered_ax = 0.0f, filtered_ay = 0.0f;
    static float speed = 1.0f;
    static float last_vel_x = 0.0f, last_vel_y = 0.0f;
    static bool is_locked_left = false, is_locked_right = false;
    static bool is_locked_up   = false, is_locked_down  = false;

    // Low-pass filter
    filtered_ax = filtered_ax * ACCEL_FILTER + ( float )ax * (1.0f - ACCEL_FILTER);
    filtered_ay = filtered_ay * ACCEL_FILTER + ( float )ay * (1.0f - ACCEL_FILTER);

    if( reset )
    {
        offset_ax = filtered_ax;
        offset_ay = filtered_ay;
        obj_x = SCREEN_WIDTH / 2.0f;
        obj_y = SCREEN_HEIGHT / 2.0f;
        vel_x = vel_y = 0.0f;
        return;
    }

    float ax_adj = filtered_ax - offset_ax;
    float ay_adj = filtered_ay - offset_ay;

    // Detect level state
    is_stationary = sqrtf(ax_adj * ax_adj + ay_adj * ay_adj) < ACCEL_THRESHOLD;

    if( !is_stationary )
    {
        vel_x += filtered_ax * ACCEL_SCALE;
        vel_y += -filtered_ay * ACCEL_SCALE;
    }

    // Centering force
    if( center_force != CENTERING_FORCE_DISABLE && is_stationary )
    {
        float cx = SCREEN_WIDTH / 2.0f;
        float cy = SCREEN_HEIGHT / 2.0f;
        vel_x += (cx - obj_x) * center_force * SCREEN_WIDTH / SCREEN_HEIGHT;
        vel_y += (cy - obj_y) * center_force;
    }

    // Friction
    vel_x *= friction;
    vel_y *= friction;

    // Check for slow motion
    velocity_small = sqrtf(vel_x * vel_x + vel_y * vel_y) < VELOCITY_THRESHOLD;

    if( velocity_small && center_force == CENTERING_FORCE_DISABLE )
    {
        if( speed <= VELOCITY_REDUCE_STEP )
        {
            vel_x = vel_y = 0.0f;
        }
        else
        {
            speed -= VELOCITY_REDUCE_STEP;
            vel_x *= speed;
            vel_y *= speed;
        }
    }
    else
    {
        speed = 1.0f;
    }

    // Update position
    obj_x += vel_x;
    obj_y += vel_y;

    bool bounce_enable = !(is_stationary && velocity_small) && (bounce_damping != BOUNCE_DAMPING_DISABLE);

    // Handle collisions and bounce
    _HandleBoundary(&obj_x, &vel_x, obj_radius, SCREEN_WIDTH,
                    &is_locked_left, &is_locked_right, &last_vel_x,
                    bounce_enable, bounce_damping, VELOCITY_LOCK_THRESHOLD);

    _HandleBoundary(&obj_y, &vel_y, obj_radius, SCREEN_HEIGHT,
                    &is_locked_up, &is_locked_down, &last_vel_y,
                    bounce_enable, bounce_damping, VELOCITY_LOCK_THRESHOLD);
}


/**
 * @brief Draw a line of constant latitude (parallel) on a 3D sphere with rotation.
 *
 * This function draws a horizontal circle (latitude line) on the surface of a sphere,
 * transformed by the current object rotation. It supports optional back-face culling
 * to skip hidden segments behind the sphere.
 *
 * @param cx          X coordinate of the sphere center on screen
 * @param cy          Y coordinate of the sphere center on screen
 * @param radius      Projected radius of the sphere in pixels
 * @param lat_angle   Latitude angle in radians (-?/2 = South Pole, 0 = Equator, ?/2 = North Pole)
 * @param transparent If 0, hides back-facing segments; if 1, draws full circle
 */
static void _DrawLatitudeLine( int cx, int cy, float radius, float lat_angle, int transparent )
{
    const int steps = 72;
    Vec3 prev_p = { 0, 0, 0 };
    int prev_valid = 0;

    for( int i = 0; i <= steps; i++ )
    {
        float theta = ( float )i / steps * 2.0f * M_PI;

        // Point on the latitude circle in sphere local space
        Vec3 p = {
            cosf(theta) * cosf(lat_angle),
            sinf(lat_angle),
            sinf(theta) * cosf(lat_angle)
        };

        // Rotate by object's rotation matrix
        Vec3 p_rot = Mat3MulVec3(obj_rotation_matrix, p);

        // Back-face culling
        if( !transparent && Vec3Dot(p_rot, camera_dir) < 0.0f )
        {
            prev_valid = 0;
            continue;
        }

        // Project to 2D
        float x, y;
        Project3DTo2D_Perspective(Vec3Scale(p_rot, radius), cx, cy, 1.0f, &x, &y, 1.0f);

        // Draw segment
        if( prev_valid )
            GPL_DrawLine(( int )prev_p.x, ( int )prev_p.y, ( int )x, ( int )y, PIXEL_CLEAN);

        prev_p.x = x;
        prev_p.y = y;
        prev_valid = 1;
    }
}


/**
 * @brief Draw a line of constant longitude (meridian) on a 3D sphere with rotation.
 *
 * This function draws a vertical circle (longitude line) on a rotating sphere.
 * It rotates the sphere using the current object rotation matrix and
 * optionally performs back-face culling for realism.
 *
 * @param cx          X coordinate of the sphere center on screen
 * @param cy          Y coordinate of the sphere center on screen
 * @param radius      Projected radius of the sphere in pixels
 * @param lon_angle   Longitude angle in radians (0 to 2?)
 * @param transparent If 0, hides back-facing segments; if 1, draws full circle
 */
static void _DrawLongitudeLine( int cx, int cy, float radius, float lon_angle, int transparent )
{
    const int steps = 72;
    Vec3 prev_p = { 0, 0, 0 };
    int prev_valid = 0;

    for( int i = 0; i <= steps; i++ )
    {
        float phi = ( float )i / steps * 2.0f * M_PI;

        // Point on the longitude circle
        Vec3 p = {
            cosf(lon_angle) * sinf(phi),
            cosf(phi),
            sinf(lon_angle) * sinf(phi)
        };

        Vec3 p_rot = Mat3MulVec3(obj_rotation_matrix, p);

        if( !transparent && Vec3Dot(p_rot, camera_dir) < 0.0f )
        {
            prev_valid = 0;
            continue;
        }

        float x, y;
        Project3DTo2D_Perspective(Vec3Scale(p_rot, radius), cx, cy, 1.0f, &x, &y, 1.0f);

        if( prev_valid )
            GPL_DrawLine(( int )prev_p.x, ( int )prev_p.y, ( int )x, ( int )y, PIXEL_CLEAN);

        prev_p.x = x;
        prev_p.y = y;
        prev_valid = 1;
    }
}


/**
 * @brief Draw a rotating 3D ball using latitude and longitude lines.
 *
 * This function updates the rotation of a virtual sphere based on angular velocity input (vx, vy),
 * then draws the equator and two vertical (longitude) lines to simulate a 3D ball grid.
 *
 * @param cx     X coordinate of the ball center on screen
 * @param cy     Y coordinate of the ball center on screen
 * @param radius Radius of the ball in screen pixels
 */
static void _DrawBallLatLonLines( uint16_t cx, uint16_t cy, uint8_t radius )
{
    // Equator (latitude 0?, and major longitudes
    _DrawLatitudeLine(cx, cy, radius, 0.0f, 0); // Equator
    _DrawLongitudeLine(cx, cy, radius, 1.0f * M_PI / 2.0f, 0); // 90?
    _DrawLongitudeLine(cx, cy, radius, 2.0f * M_PI / 2.0f, 0); // 180?
}


/**
 * @brief Draw a line on the screen using Z-buffer-based depth testing.
 *
 * This function rasterizes a 2D line from (x0, y0) to (x1, y1), while linearly interpolating
 * the depth value (z) between the endpoints. For each pixel, it updates the Z-buffer
 * and sets the pixel only if it is closer to the viewer than previously drawn geometry.
 *
 * The algorithm is based on Bresenham's line drawing with floating-point depth interpolation.
 *
 * @param x0 Starting point X coordinate
 * @param y0 Starting point Y coordinate
 * @param z0 Starting point depth (Z)
 * @param x1 Ending point X coordinate
 * @param y1 Ending point Y coordinate
 * @param z1 Ending point depth (Z)
 */
static void _DrawLineWithZBuffer(int x0, int y0, float z0, int x1, int y1, float z1)
{
    // Setup for Bresenham's line algorithm
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    // Determine the number of steps along the line for depth interpolation
    int length = (dx > -dy) ? dx : -dy;
    if (length == 0) length = 1;  // Avoid division by zero

    int step_count = 0;

    // Bresenham loop with Z-buffer test per pixel
    while (true)
    {
        // Linearly interpolate z-depth at the current pixel step
        float t = (float)step_count / (float)length;
        float z = z0 + (z1 - z0) * t;

        // Check if pixel is inside screen bounds
        if (x0 >= 0 && x0 < SCREEN_WIDTH && y0 >= 0 && y0 < SCREEN_HEIGHT)
        {
            // Update pixel only if closer than existing depth (with small margin)
            if ((z-0.3)*Z_INTEGER_SCALE < z_buffer[y0][x0])
            {
                z_buffer[y0][x0] = z*Z_INTEGER_SCALE;
                GPL_DrawPoint(x0, y0, PIXEL_SET);
            }
        }

        // Exit when line endpoint is reached
        if (x0 == x1 && y0 == y1) break;

        // Bresenham step
        e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }

        step_count++;
    }
}


/**
 * @brief Draw solid dice dots (pips) on the given cube face with 3D perspective and Z-buffering.
 *
 * @param face_index Index of the cube face to draw (0?5)
 * @param rotation   Current cube rotation matrix
 * @param cx         Screen center X (e.g., 64 for 128x64 screen)
 * @param cy         Screen center Y
 * @param scale      Scaling factor for projection
 */
static void _DrawDiceDotsOnFace3D(int face_index, Mat3 rotation, int cx, int cy, float scale)
{
    // Normalized local 2D positions for dice dot patterns (centered at 0, range -0.5 to +0.5)
    static const Vec2 dot_patterns[6][6] = {
        { {     0,      0}, {     0,     0}, {    0,      0}, {    0,     0}, {     0, 0}, {    0, 0} }, // 1
        { {-0.25f, -0.25f}, { 0.25f, 0.25f}, {    0,      0}, {    0,     0}, {     0, 0}, {    0, 0} }, // 2
        { {-0.25f, -0.25f}, {     0,     0}, {0.25f,  0.25f}, {    0,     0}, {     0, 0}, {    0, 0} }, // 3
        { {-0.25f, -0.25f}, {-0.25f, 0.25f}, {0.25f, -0.25f}, {0.25f, 0.25f}, {     0, 0}, {    0, 0} }, // 4
        { {-0.25f, -0.25f}, {-0.25f, 0.25f}, {0.25f, -0.25f}, {0.25f, 0.25f}, {     0, 0}, {    0, 0} }, // 5
        { {-0.25f, -0.25f}, {-0.25f, 0.25f}, {0.25f, -0.25f}, {0.25f, 0.25f}, {-0.25f, 0}, {0.25f, 0} }  // 6
    };

    // Get dice number (1?6) corresponding to the face index (0?5)
    int dice_number = face_index_to_dice_number[face_index];

    // Number of dots = dice number (e.g., 1 dot for dice number 1)
    int dot_count = dice_number;
    const Vec2* dots = dot_patterns[dice_number - 1];

    // Get three corner vertices of the face (v0, v1, v3) to define local X and Y axes
    Vec3 v0 = cube_vertices[cube_faces[face_index][0]];
    Vec3 v1 = cube_vertices[cube_faces[face_index][1]];
    Vec3 v3 = cube_vertices[cube_faces[face_index][3]];

    Vec3 local_x = Vec3Normalize(Vec3Sub(v1, v0)); // Direction along one edge
    Vec3 local_y = Vec3Normalize(Vec3Sub(v3, v0)); // Direction along adjacent edge

    for (int i = 0; i < dot_count; i++)
    {
        float fx = dots[i].x;
        float fy = dots[i].y;

        // Compute dot center position in 3D, offset from v0 by normalized X/Y
        Vec3 base = Vec3Add(
                        v0,
                        Vec3Add(Vec3Scale(local_x, fx + 0.5f),
                                Vec3Scale(local_y, fy + 0.5f)));

        float dot_radius = 0.09f;

        // Draw filled circle by angle and radius sampling
        for (float angle = 0; angle < 2 * PI; angle += 0.3f)
        {
            for (float r = 0; r <= dot_radius; r += 0.01f)
            {
                float dx = r * cosf(angle);
                float dy = r * sinf(angle);

                Vec3 offset = Vec3Add(Vec3Scale(local_x, dx), Vec3Scale(local_y, dy));
                Vec3 pt = Vec3Add(base, offset); // Local point on face
                Vec3 pr = Mat3MulVec3(rotation, pt); // Rotate into world space

                float sx, sy;
                Project3DTo2D_Perspective(pr, cx, cy, scale, &sx, &sy, 1.0f);

                int px = (int)sx;
                int py = (int)sy;

                // Z-buffer check and draw
                if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT)
                {
                    if (pr.z*Z_INTEGER_SCALE < z_buffer[py][px])
                    {
                        z_buffer[py][px] = pr.z*Z_INTEGER_SCALE;
                        GPL_DrawPoint(px, py, PIXEL_SET);
                    }
                }
            }
        }
    }
}


/**
 * @brief Draw a 3D cube with hidden line removal using Z-buffering and face visibility checks.
 *
 * This function performs the following steps:
 * 1. Applies 3D rotation to all 8 cube vertices.
 * 2. Projects rotated vertices to 2D screen space.
 * 3. Fills Z-buffer using the cube's visible faces (via back-face culling).
 * 4. Draws dice dots on visible faces using face-local coordinates.
 * 5. Draws cube edges using Z-buffer-aware line rendering, skipping fully hidden edges.
 *
 * @param cx       Screen center X (e.g., 64 for a 128x64 display)
 * @param cy       Screen center Y
 * @param scale    Scaling factor for 3D-to-2D projection
 * @param rotation 3x3 rotation matrix for transforming cube orientation
 */
static void _DrawCube(int cx, int cy, float scale, Mat3 rotation)
{
    Vec3 rotated_vertices[8];     // Rotated 3D coordinates of cube corners
    Vec2 projected_vertices[8];   // 2D projected screen coordinates
    float rotated_z[8];           // Z-depth of each vertex after rotation

    _ClearZBuffer(); // Clear depth buffer before rendering new frame

    // Apply rotation and perspective projection to each vertex
    for (int i = 0; i < 8; i++)
    {
        rotated_vertices[i] = Mat3MulVec3(rotation, cube_vertices[i]);
        rotated_z[i] = rotated_vertices[i].z;

        Project3DTo2D_Perspective(
            rotated_vertices[i], cx, cy, scale,
            &projected_vertices[i].x,
            &projected_vertices[i].y,
            1.0f);
    }

    bool face_visible[6];

    // Iterate over each of the 6 cube faces
    for (int f = 0; f < 6; f++)
    {
        // Determine if the face is front-facing using dot(face_normal, view_dir)
        face_visible[f] = _IsFaceVisible(rotation, obj_position, face_normals[f], face_centers[f], camera_position);

#if DISABLE_DICE_FACE
        // If the face is visible, render dice dots onto it
        if (f==0 && face_visible[f])
            _DrawDiceDotsOnFace3D(f, rotation, cx, cy, scale);
        continue;
#endif
        // If the face is visible, render dice dots onto it
        if (face_visible[f])
            _DrawDiceDotsOnFace3D(f, rotation, cx, cy, scale);

        // Get 3D and 2D coordinates for the 4 corners of the face
        Vec3 v0 = rotated_vertices[cube_faces[f][0]];
        Vec3 v1 = rotated_vertices[cube_faces[f][1]];
        Vec3 v2 = rotated_vertices[cube_faces[f][2]];
        Vec3 v3 = rotated_vertices[cube_faces[f][3]];

        Vec2 p0 = projected_vertices[cube_faces[f][0]];
        Vec2 p1 = projected_vertices[cube_faces[f][1]];
        Vec2 p2 = projected_vertices[cube_faces[f][2]];
        Vec2 p3 = projected_vertices[cube_faces[f][3]];

        // Fill the face using two triangles, writing to the Z-buffer
        _FillTriangleZBuffer(p0, p1, p2, v0.z, v1.z, v2.z);
        _FillTriangleZBuffer(p2, p3, p0, v2.z, v3.z, v0.z);
    }

    // Draw all 12 cube edges with hidden line removal
    for (int e = 0; e < 12; e++)
    {
        int f1 = edge_faces[e][0]; // One face adjacent to the edge
        int f2 = edge_faces[e][1]; // The other face

        // Skip edge if both adjacent faces are hidden
        if (!face_visible[f1] && !face_visible[f2])
            continue;

        int v0 = cube_edges[e][0];
        int v1 = cube_edges[e][1];

        int x0 = (int)projected_vertices[v0].x;
        int y0 = (int)projected_vertices[v0].y;
        float z0 = rotated_z[v0];

        int x1 = (int)projected_vertices[v1].x;
        int y1 = (int)projected_vertices[v1].y;
        float z1 = rotated_z[v1];

        // Z-aware line rendering for visible edges
        _DrawLineWithZBuffer(x0, y0, z0, x1, y1, z1);
    }
}


// **************************************************************************
// Application: Gravity Applications - Level Bubble
// **************************************************************************
/**
 * @brief Draws a spirit level-style bubble indicator using simulated gravity physics.
 *
 * This function simulates a bubble affected by device tilt using accelerometer input.
 * A centering force pulls the bubble back to the center, while bouncing is disabled to emulate
 * fluid damping. The result is drawn as a ball within concentric circles, mimicking a bubble level.
 *
 * @param ax     Raw X-axis accelerometer reading
 * @param ay     Raw Y-axis accelerometer reading
 * @param reset  If true, resets bubble position and velocity to center
 * @param layer  Display layer identifier
 *
 * @note This function reverses the accelerometer direction to simulate how a bubble rises
 *       opposite to the direction of tilt (gravity).
 *
 * @note The bubble is drawn as a circle with an arc-shaped highlight to enhance visual realism.
 */
void DrawLevelBubble( short ax, short ay, bool reset, uint8_t layer )
{
    // Prepare layer
    GPL_LayerSet(layer);
    GPL_LayerClean(layer);
    GPL_SetPenSize(1);

    // Axis convert for device orientation correction
    // Quadrant 1 Normal Sensor Axis      : -ax, -ay
    // Quadrant 1 APP055 Sensor Axis      : +ay, -ax -> -ay, +ax (Bubbles reverse gravity)
    // Quadrant 1 APP-Kick-014 Sensor Axis: +ax, +ay -> -ax, -ay (Bubbles reverse gravity)
    // Update ball physics (with centering force, no bounce)
    _GravityObject(-ay, ax, BALL_RADIUS, CENTERING_FORCE, FRICTION_WATER, BOUNCE_DAMPING_DISABLE, reset, layer);

    // Draw bubble reference circles
    GPL_DrawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 30, PIXEL_SET);
    GPL_DrawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 3, PIXEL_SET);

    // Draw ball as a circle
    GPL_DrawCircle(( uint16_t )obj_x, ( uint16_t )obj_y, BALL_RADIUS, PIXEL_SET);

    // Draw highlight reflection on ball
    GPL_SetPenSize(2);
    GPL_DrawArc(( uint16_t )obj_x, ( uint16_t )obj_y, BALL_RADIUS - 3, BALL_RADIUS - 3, 70, 80, PIXEL_SET);
    GPL_DrawArc(( uint16_t )obj_x, ( uint16_t )obj_y, BALL_RADIUS - 3, BALL_RADIUS - 3, 5, 50, PIXEL_SET);
}


// **************************************************************************
// Application: Gravity Applications - Banalance Ball
// **************************************************************************
/**
 * @brief Simulates a freely moving ball affected by tilt, with bounce and rotation.
 *
 * This function renders a balance ball that moves under simulated gravity using
 * raw accelerometer input. It supports bouncing against screen boundaries and
 * visualizes the ball with 3D-like rotation using latitude and longitude lines.
 *
 * @param ax     Raw X-axis accelerometer reading
 * @param ay     Raw Y-axis accelerometer reading
 * @param reset  If true, resets ball position and velocity
 * @param layer  Display layer identifier
 *
 * @note This function disables centering force to allow free movement, and
 *       enables bouncing on screen edges. It also updates the rotation matrix
 *       based on the simulated velocity.
 */
void DrawBalanceBall( short ax, short ay, bool reset, uint8_t layer )
{
    // Prepare layer
    GPL_LayerSet(layer);
    GPL_LayerClean(layer);
    GPL_SetPenSize(1);

    // Axis convert for device orientation correction
    // Quadrant 1 Normal Sensor Axis      : -ax, -ay
    // Quadrant 1 APP055 Sensor Axis      : +ay, -ax
    // Quadrant 1 APP-Kick-014 Sensor Axis: +ax, +ay
    // Update ball physics (no centering, with bounce)
    _GravityObject(ay, -ax, BALL_RADIUS, CENTERING_FORCE_DISABLE, FRICTION_SURFACE, BOUNCE_DAMPING, reset, layer);

    // Draw crosshair at center
    GPL_DrawLine(SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2,     SCREEN_WIDTH / 2 + 5, SCREEN_HEIGHT / 2,     PIXEL_SET);
    GPL_DrawLine(SCREEN_WIDTH / 2,     SCREEN_HEIGHT / 2 - 5, SCREEN_WIDTH / 2,     SCREEN_HEIGHT / 2 + 5, PIXEL_SET);

    // Draw filled ball
    GPL_FillCircle(( uint16_t )obj_x, ( uint16_t )obj_y, BALL_RADIUS, PIXEL_SET);

    // Quadrant 1 Normal Sensor Axis: -ax, -ay
    // Rotate Object by swap Quadrant 1 Sensor Axis: -vy, -vx
    _UpdateObjectRotation(-vel_y, -vel_x);

    // Draw 3D Latitude and Longitude line on ball for rotate simulation
    _DrawBallLatLonLines(( uint16_t )obj_x, ( uint16_t )obj_y, BALL_RADIUS);
}


// **************************************************************************
// Application: Gravity Applications - 3D Cube
// **************************************************************************
/**
 * @brief Updates cube rotation using accelerometer input and renders the cube.
 *
 * This function simulates the physics of a gravity-driven object to move the cube?s position.
 * The cube is rotated based on the object's velocity and rendered from a fixed camera viewpoint.
 *
 * @param ax     Raw X-axis acceleration
 * @param ay     Raw Y-axis acceleration
 * @param reset  If true, resets the cube position and velocity
 * @param layer  Display layer identifier
 *
 * @note This function assumes a fixed camera view and moves the cube based on input tilt.
 *       Velocity is integrated from acceleration and optionally bounced off screen edges.
 */
void DrawRotationCube( short ax, short ay, bool reset, uint8_t layer )
{
    // Prepare layer
    GPL_LayerSet(layer);
    GPL_LayerClean(layer);
    GPL_SetPenSize(1);

    // Axis convert for device orientation correction
    // Quadrant 1 Normal Sensor Axis      : -ax, -ay
    // Quadrant 1 APP055 Sensor Axis      : +ay, -ax
    // Quadrant 1 APP-Kick-014 Sensor Axis: +ax, +ay
    // Update ball physics (no centering, with bounce)
    _GravityObject(ay, -ax, DICE_RADIUS, CENTERING_FORCE_DISABLE, FRICTION_SURFACE, BOUNCE_DAMPING, reset, layer);

    // Quadrant 1 Normal Sensor Axis: -ax, -ay
    // Rotate Object by swap Quadrant 1 Sensor Axis: -vy, -vx
    if( !_UpdateObjectRotation(-vel_y, -vel_x) )
    {
        // Align to Face Up after cube rotation stopped and get the dice number
#if DISABLE_DICE_FACE
        _CubeFaceUpRotation(-vel_y, -vel_x);
#else
        // Print current dice number
        GPL_Printf(0, 0, BG_TRANSPARENT, TEXT_NORMAL, "Dice : %d", _CubeFaceUpRotation(-vel_y, -vel_x) );
#endif
    }

    // View the cube by fixed camera porition moving Cube to obj_x, obj_y with scale
    _DrawCube(obj_x, obj_y, DICE_RADIUS, obj_rotation_matrix);
}


/**
 * @brief Updates camera rotation using accelerometer and gyroscope input and renders the cube.
 *
 * This function applies sensor fusion logic (gyro-integrated rotation with accelerometer stabilization)
 * to update the camera?s rotation matrix. It then draws a 3D cube as viewed from the camera.
 *
 * @param ax     Raw X-axis acceleration
 * @param ay     Raw Y-axis acceleration
 * @param az     Raw Z-axis acceleration
 * @param gx     Raw X-axis gyroscope (angular velocity)
 * @param gy     Raw Y-axis gyroscope
 * @param gz     Raw Z-axis gyroscope
 * @param reset  If true, resets the camera orientation to identity
 * @param layer  Display layer identifier
 *
 * @note This function is typically used to render a stable camera view of a rotating object
 *       using IMU input. The accelerometer and gyro axes may be remapped for board orientation.
 */
void ViewRotationCube( short ax, short ay, short az, short gx, short gy, short gz, bool reset, uint8_t layer )
{
    // Reset camera rotation and gyro zero offset
    if( reset )
    {
        camera_rotation_matrix = (Mat3){
            {
                {1, 0, 0 },
                {0, 1, 0 },
                {0, 0, 1 }
            }
        };
    }

    // Update camera rotation matrix based on accelerometer and gyro input
    // Axis convert for device orientation correction, camera at -az
    // Normal Sensor Axis      : +ax, +ay, +az, +gx, +gy, +gz
    // APP055 Sensor Axis      : -ay, +ax, +az, -gy, +gx, +gz -> -ay, +ax, -az, -gy, +gx, -gz (camera at -az)
    // APP-Kick-014 Sensor Axis: -ax, -ay, +az, -gx, -gy, +gz -> -ax, -ay, -az, -gx, -gy, -gz (camera at -az)
    _UpdateCameraRotation( -ay,  ax, -az, -gy,  gx, -gz );

    // Prepare drawing layer and settings
    GPL_LayerSet(layer);
    GPL_LayerClean(layer);
    GPL_SetPenSize(1);

    // View the cube by camera porition centered at screen center with scale
    _DrawCube(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, CUBE_RADIUS, camera_rotation_matrix);
}

/* *****************************************************************************
 End of File
 */

