/* ************************************************************************** */
/** GraphicLib.h - Monochrome OLED Graphics Library Header

  @Company
    Microchip Technology Inc.

  @File Name
    GraphicLib.h

  @Summary
    Header file for the GraphicLib.c module providing 2D graphics APIs for monochrome OLED displays.

  @Description
    This file defines a graphics library designed for monochrome OLED screens. The library provides
    a comprehensive set of drawing primitives and layer management functions to render pixels, lines,
    rectangles, circles, ellipses, arcs, bitmaps, and formatted text on the display. It supports multiple
    layers, drawing pen size configuration, scaling, and screen update control.

    Main Features:
    - Multi-layer screen support (show/hide/clear individual layers)
    - Drawing primitives: point, line, rectangle, filled rectangle, circle, ellipse, arc, cross
    - Bitmap rendering with transparency and background handling
    - Formatted text rendering with highlighting/background support (via GPL_Printf)
    - Adjustable pen (brush) size
    - Region scaling support
    - Full screen clear and refresh operations

    Function List:
    - `GPL_ScreenInit`         : Initialize the screen and graphics system
    - `GPL_LayerSet`           : Set the active drawing layer
    - `GPL_LayerShow`          : Show or hide a specific layer
    - `GPL_LayerClean`         : Clear all pixels in a specific layer
    - `GPL_ScreenClean`        : Clear the entire screen
    - `GPL_ScreenUpdate`       : Refresh the OLED with current layer data
    - `GPL_SetPenSize`         : Set the pen (brush) pixel size
    - `GPL_GetPenSize`         : Get the current pen size
    - `GPL_DrawPoint`          : Draw a pixel at the given coordinate
    - `GPL_DrawLine`           : Draw a line between two points
    - `GPL_DrawRect`           : Draw an unfilled rectangle
    - `GPL_FillRect`           : Draw a filled rectangle with a specified color
    - `GPL_DrawCross`          : Draw a cross centered at a point
    - `GPL_DrawEllipse`        : Draw an ellipse centered at a point
    - `GPL_FillEllipse`        : Draw an filled ellipse centered at a point
    - `GPL_DrawCircle`         : Draw a circle centered at a point
    - `GPL_FillCircle`         : Draw a filled circle centered at a point
    - `GPL_DrawArc`            : Draw an arc (portion of an ellipse) with angle range
    - `GPL_DrawBitmap`         : Draw a bitmap image to the screen with optional background
    - `GPL_Scale`              : Scale a region with a given ratio
    - `GPL_Printf`             : Print formatted text at a specific position with optional background and highlight

 */
/* ************************************************************************** */

#ifndef _GRAPHICLIB_H    /* Guard against multiple inclusion */
#define _GRAPHICLIB_H

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "LCM.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// GPL Screen size and Center
#define SCREEN_WIDTH        LCM_WIDTH
#define SCREEN_HEIGHT       LCM_HEIGHT
#define SCREEN_CENTER_X    (LCM_WIDTH>>1)
#define SCREEN_CENTER_Y    (LCM_HEIGHT>>1)

// Font size, Font Bitmap height should multiple of 8 (8, 16, 24 ...)
#define FONT_WIDTH       7
#define FONT_HEIGHT      8

// Math
#define M_PI           3.14159265358979323846
#define DEG2RAD( deg ) ( deg * M_PI / 180 )
#define RAD2DEG( rad ) ( rad * 180 / M_PI )

typedef enum {
    PIXEL_CLEAN = 0,
    PIXEL_SET
} GPL_PIXEL;

typedef enum {
    BG_TRANSPARENT = 0,
    BG_SOLID
} GPL_BACKGROUND;

typedef enum {
    TEXT_NORMAL = 0,
    TEXT_HIGHLIGHT
} GPL_TEXT;

typedef enum {
    LAYER_HIDE,
    LAYER_SHOW,
} GPL_LAYERSHOW;

enum {
    LAYER_GRAPHIC,
    LAYER_TEXT,
    LAYER_MAX
};

// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************
uint8_t GPL_ScreenInit( void );
void GPL_LayerSet( uint8_t index );
void GPL_LayerShow( uint8_t index, GPL_LAYERSHOW show );
void GPL_LayerClean( uint8_t index );
void GPL_ScreenClean( void );
void GPL_ScreenUpdate( void );
void GPL_SetPenSize( uint16_t psize );
uint16_t GPL_GetPenSize( void );
void GPL_DrawPoint( uint16_t x, uint16_t y, GPL_PIXEL pixel );
void GPL_DrawLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, GPL_PIXEL pixel );
void GPL_DrawRect( uint16_t x, uint16_t y, uint16_t w, uint16_t h, GPL_PIXEL pixel );
void GPL_FillRect( uint16_t x, uint16_t y, uint16_t w, uint16_t h, GPL_PIXEL pixel );
void GPL_DrawCross( uint16_t x0, uint16_t y0, uint16_t r, GPL_PIXEL pixel );
void GPL_DrawEllipse(uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, GPL_PIXEL pixel );
void GPL_FillEllipse(uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, GPL_PIXEL pixel);
void GPL_DrawCircle( uint16_t x0, uint16_t y0, uint16_t r, GPL_PIXEL pixel );
void GPL_FillCircle(uint16_t x0, uint16_t y0, uint16_t r, GPL_PIXEL pixel);
void GPL_DrawArc(uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, int start_angle, int end_angle, GPL_PIXEL pixel);
void GPL_DrawBitmap( uint16_t x, uint16_t y, uint16_t w, uint16_t h, GPL_BACKGROUND background, const unsigned char *bitmap );
void GPL_Scale(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, float Ratio);
void GPL_Printf( uint16_t x, uint16_t y, GPL_BACKGROUND background, GPL_TEXT highlight, const char *format, ... );

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _GRAPHICLIB_H */

/* *****************************************************************************
 End of File
 */

