/* ************************************************************************** */
/** GraphicAPP.c - High-Level OLED Graphic Application API Source

  @Company
    Microchip Technology Inc.

  @File Name
    GraphicAPP.c

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
    - `DrawClockAnimationInit`
    - `DrawClockAnimation`
    - `DrawGraphicElements`
    - `DrawDoraemonFace`
    - `DrawMicrochipLogo`
    - `DrawSplashAnimation`
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

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
#define CLOCK_CENTER_X   31
#define CLOCK_CENTER_Y   31
#define CLOCK_RADIUS     30
#define DIGITAL_CLOCK_X  70
#define DIGITAL_CLOCK_Y  26
#define HOUR_HAND_LEN    (CLOCK_RADIUS * 0.5)
#define MIN_HAND_LEN     (CLOCK_RADIUS * 0.75)
#define SEC_HAND_LEN     (CLOCK_RADIUS * 0.95)
int clock_hour, clock_min, clock_sec;
void DrawClockAnimationInit(void)
{
    // __TIME__ format: "HH:MM:SS"
    sscanf(__TIME__, "%2d:%2d:%2d", &clock_hour, &clock_min, &clock_sec);
}

void DrawClockAnimation( uint8_t layer )
{
    const int cx = CLOCK_CENTER_X;
    const int cy = CLOCK_CENTER_Y;
    const int r = CLOCK_RADIUS;
    static uint8_t TickPeriod=0;

    // 500ms once call
    ++TickPeriod;
    if( TickPeriod%=2 ) { return; }

    // Set active graphic Layer
    GPL_LayerSet(layer);
    // Clear the graphic layer
    GPL_LayerClean(layer);
    // Set Default Pen Size
    GPL_SetPenSize(1);

    // Draw clock face
    GPL_DrawCircle( cx, cy, r, PIXEL_SET );

    // Hour marks (12 positions)
    for (int h = 0; h < 12; h++)
    {
        float angle = h * 30.0f * M_PI / 180.0f;
        int x1 = cx + (int)(cos(angle) * (r - 4));
        int y1 = cy - (int)(sin(angle) * (r - 4));
        int x2 = cx + (int)(cos(angle) * r);
        int y2 = cy - (int)(sin(angle) * r);
        GPL_DrawLine( x1, y1, x2, y2, PIXEL_SET );
    }

    // Angles
    float sec_angle  = (90.0f - clock_sec * 6.0f) * M_PI / 180.0f;
    float min_angle  = (90.0f - clock_min * 6.0f - clock_sec * 0.1f) * M_PI / 180.0f;
    float hour_angle = (90.0f - (clock_hour % 12) * 30.0f - clock_min * 0.5f) * M_PI / 180.0f;

    // Hand Hour
    GPL_SetPenSize(3);
    int xh = cx + (int)(cos(hour_angle) * HOUR_HAND_LEN);
    int yh = cy - (int)(sin(hour_angle) * HOUR_HAND_LEN);
    GPL_DrawLine( cx, cy, xh, yh, PIXEL_SET );
    // Hand Minute
    GPL_SetPenSize( 2 );
    int xm = cx + (int)(cos(min_angle) * MIN_HAND_LEN);
    int ym = cy - (int)(sin(min_angle) * MIN_HAND_LEN);
    GPL_DrawLine( cx, cy, xm, ym, PIXEL_SET );
    // Hand Second
    GPL_SetPenSize(1);
    int xs = cx + (int)(cos(sec_angle) * SEC_HAND_LEN);
    int ys = cy - (int)(sin(sec_angle) * SEC_HAND_LEN);
    GPL_DrawLine( cx, cy, xs, ys, PIXEL_SET );

    // Digital clock
    GPL_Printf( DIGITAL_CLOCK_X, DIGITAL_CLOCK_Y, BG_SOLID, TEXT_NORMAL, "%02d:%02d:%02d", clock_hour, clock_min, clock_sec );

    // Advance time
    clock_sec++;
    if (clock_sec >= 60)
    {
        clock_sec = 0;
        clock_min++;
        if (clock_min >= 60)
        {
            clock_min = 0;
            clock_hour = (clock_hour + 1) % 24;
        }
    }

    // Microchip Text
    GPL_Printf( 67, 50, BG_SOLID, TEXT_NORMAL, "Microchip");
}

void DrawGraphicElements( uint8_t layer )
{
    // Set active graphic Layer
    GPL_LayerSet(layer);
    // Clear the graphic layer
    GPL_LayerClean(layer);
    // Set Default Pen Size
    GPL_SetPenSize(1);
    GPL_DrawRect   (   0,  0,  16, 16, PIXEL_SET );
    GPL_DrawRect   (   5,  5,  32, 10, PIXEL_SET );
    GPL_FillRect   (  10, 10,  12, 12, PIXEL_SET );
    GPL_FillRect   (  32,  0,  22, 12, PIXEL_SET );
    GPL_DrawLine   (  64,  0,  80, 30, PIXEL_SET );
    GPL_DrawLine   (  64, 30,  80,  0, PIXEL_SET );
    GPL_DrawLine   (  64, 15,  80, 15, PIXEL_SET );
    GPL_DrawArc    ( 108, 16,  16, 16, 340, 230, PIXEL_SET );
    GPL_DrawLine   ( 108, 16, 124, 20, PIXEL_SET );
    GPL_DrawLine   ( 108, 16, 100, 28, PIXEL_SET );
    GPL_DrawArc    ( 108, 16,  10, 10, 270, 180, PIXEL_SET );
    GPL_DrawLine   ( 108, 16,  98, 16, PIXEL_SET );
    GPL_DrawLine   ( 108, 16, 108, 26, PIXEL_SET );
    GPL_DrawCircle (  16, 42,  15, PIXEL_SET );
    GPL_DrawCross  (  16, 42,  10, PIXEL_SET );
    GPL_DrawEllipse(  48, 40,   6, 18, PIXEL_SET );
    GPL_DrawEllipse(  48, 40,  10,  6, PIXEL_SET );
    GPL_Printf     (  67, 38, BG_SOLID, TEXT_NORMAL, "Microchip");
    GPL_Printf     (  67, 50, BG_SOLID, TEXT_HIGHLIGHT, "GPL Lib");
}

void DrawDoraemonFace( uint8_t layer )
{
    // Set active graphic Layer
    GPL_LayerSet(layer);
    // Clear the graphic layer
    GPL_LayerClean(layer);
    // Set Default Pen Size
    GPL_SetPenSize(1);

    // Head (large outer circle)
    GPL_DrawCircle( 64, 30, 30, PIXEL_SET );

    // Face contour (inner circle)
    GPL_DrawArc( 64, 36, 24, 24, 120, 60, PIXEL_SET );

    // Left eye
    GPL_DrawEllipse( 58, 13, 5, 7, PIXEL_SET );
    // Right eye
    GPL_DrawEllipse( 70, 13, 5, 7, PIXEL_SET );

    // Pupils
    GPL_DrawCircle( 59, 13, 3, PIXEL_SET );
    GPL_DrawCircle( 69, 13, 3, PIXEL_SET );
    GPL_DrawCircle( 59, 13, 2, PIXEL_SET );
    GPL_DrawCircle( 69, 13, 2, PIXEL_SET );

    // Nose (small circle)
    GPL_DrawCircle( 64, 26, 3, PIXEL_SET );

    // Vertical line under nose
    GPL_DrawLine( 64, 29, 64, 38, PIXEL_SET );

    // Mouth (smile simulated using lines)
    GPL_DrawLine( 50, 38, 78, 38, PIXEL_SET );
    GPL_DrawArc( 64, 38, 14, 11, 180,  0, PIXEL_SET );
    GPL_DrawArc( 64, 49,  7,  6, 10, 170, PIXEL_SET );

    // Whiskers (three lines on each side)
    GPL_DrawLine( 32, 22, 50, 26, PIXEL_SET );
    GPL_DrawLine( 32, 31, 50, 30, PIXEL_SET );
    GPL_DrawLine( 32, 38, 50, 34, PIXEL_SET );
    GPL_DrawLine( 78, 26, 96, 22, PIXEL_SET );
    GPL_DrawLine( 78, 30, 96, 31, PIXEL_SET );
    GPL_DrawLine( 78, 34, 96, 38, PIXEL_SET );

    // Bell (Rectangle + circle + line)
    GPL_DrawRect( 46, 56, 38, 6, PIXEL_SET );
    GPL_DrawCircle( 64, 58, 4, PIXEL_SET );
    GPL_DrawLine( 60, 58, 68, 58, PIXEL_SET );
    GPL_DrawPoint( 64, 60, PIXEL_SET );

    // Arm (circle + line)
    GPL_DrawLine( 46, 53,  26, 44, PIXEL_SET );
    GPL_DrawLine( 40, 64,  20, 52, PIXEL_SET );
    GPL_DrawCircle( 18, 46, 7, PIXEL_SET );
    GPL_DrawLine( 82, 53, 102, 44, PIXEL_SET );
    GPL_DrawLine( 88, 64, 108, 52, PIXEL_SET );
    GPL_DrawCircle( 110, 46, 7, PIXEL_SET );
}

void DrawMicrochipLogo( uint8_t layer )
{
    // Set active graphic Layer
    GPL_LayerSet(layer);
    // Clear the graphic layer
    GPL_LayerClean(layer);
    // Set Default Pen Size
    GPL_SetPenSize(1);

    GPL_DrawArc( 62, 22, 22, 22, 55, 192, PIXEL_SET );

    GPL_DrawLine( 40, 28, 49, 19, PIXEL_SET );
    GPL_DrawLine( 49, 19, 54, 27, PIXEL_SET );
    GPL_DrawLine( 54, 27, 46, 35, PIXEL_SET );

    GPL_DrawArc( 62, 22, 22, 22, 219, 320, PIXEL_SET );

    GPL_DrawLine( 51, 15, 63, 35, PIXEL_SET );
    GPL_DrawLine( 63, 35, 70, 28, PIXEL_SET );
    GPL_DrawLine( 70, 28, 57,  8, PIXEL_SET );
    GPL_DrawLine( 57,  8, 51, 15, PIXEL_SET );

    GPL_DrawLine( 79, 36, 65, 15, PIXEL_SET );
    GPL_DrawLine( 65, 15, 71,  8, PIXEL_SET );
    GPL_DrawLine( 71,  8, 84, 27, PIXEL_SET );

    GPL_DrawLine( 75,  5, 86, 21, PIXEL_SET );

    GPL_DrawLine( 86, 21, 84, 27, PIXEL_SET );

    GPL_DrawCircle( 80, 3, 2, PIXEL_SET );

    GPL_Printf( 10, 48, BG_SOLID, TEXT_NORMAL, "Microchip" );

    GPL_Scale( 10, 48, 68, 8, 1.8f );
}

void DrawSplashAnimation(int x0, int y0, int x1, int y1, uint8_t step, const unsigned char *bitmap, uint8_t layer )
{
    // Set active graphic Layer
    GPL_LayerSet(layer);
    // Clear the graphic layer
    GPL_LayerClean(layer);
    // Set Default Pen Size
    GPL_SetPenSize(1);

    float current_x = x0;
    float current_y = y0;

    int dx = x1 - x0;
    int dy = y1 - y0;

    // Use the greater distance for frame count
    int distance = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);
    int steps = distance / step;
    if (steps == 0) steps = 1;

    float step_x = (float)dx / steps;
    float step_y = (float)dy / steps;

    for (int i = 0; i <= steps; i++)
    {
        // Clear the graphic layer
        GPL_LayerClean(layer);

        // Draw logo at current position (even if partially off-screen)
        GPL_DrawBitmap((int)(current_x + 0.5f), (int)(current_y + 0.5f),
                       LCM_WIDTH, LCM_HEIGHT, BG_SOLID, bitmap);

        // Refresh screen
        GPL_ScreenUpdate();

        // Update position
        current_x += step_x;
        current_y += step_y;
    }
}
/* *****************************************************************************
 End of File
 */

