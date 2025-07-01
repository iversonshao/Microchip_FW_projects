/* ************************************************************************** */
/** GraphicLib.c - Monochrome OLED Graphics Library Source File

  @Company
    Microchip Technology Inc.

  @File Name
    GraphicLib.c

  @Summary
    Implementation of 2D graphics API functions for monochrome OLED displays.

  @Description
    This source file contains the implementation of a graphics library tailored for
    monochrome OLED displays. It supports multiple layers, pixel-based drawing, shapes,
    image rendering, formatted text output, and scaling. Each drawing operation is designed
    to work with the configured drawing layer and pen size, offering flexibility and
    efficiency for embedded GUIs on OLED panels.

    Key Features Implemented:
    - Initialization and management of drawing layers
    - Screen refresh and clearing
    - Drawing primitives: point, line, rectangle, filled rectangle, cross, circle, ellipse, arc
    - Image rendering with transparency or background control
    - Text output with formatting and optional highlight/background
    - Adjustable pen (brush) size
    - Scalable region rendering

    Corresponding API Functions:
    - `GPL_ScreenInit`
    - `GPL_LayerSet`
    - `GPL_LayerShow`
    - `GPL_LayerClean`
    - `GPL_ScreenClean`
    - `GPL_ScreenUpdate`
    - `GPL_SetPenSize`
    - `GPL_GetPenSize`
    - `GPL_DrawPoint`
    - `GPL_DrawLine`
    - `GPL_DrawRect`
    - `GPL_FillRect`
    - `GPL_DrawCross`
    - `GPL_DrawEllipse`
    - `GPL_FillEllipse`
    - `GPL_DrawCircle`
    - `GPL_FillCircle`
    - `GPL_DrawArc`
    - `GPL_DrawBitmap`
    - `GPL_Scale`
    - `GPL_Printf`

 */
/* ************************************************************************** */


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "definitions.h"
#include "GraphicLib.h"
#include "Alphabet_Fonts.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
#define  GPL_LAYERS LAYER_MAX
struct {
    uint8_t       Buf[LCM_FRAME_SIZE]; // (128x64)/8, 1bit/pixel, Layer Buffer
    GPL_LAYERSHOW Show;
} GPL_Layer[GPL_LAYERS];

uint8_t  GPL_Invalidate = false;
uint16_t GPL_PenSize  = 1; // Good for Odd number 1,3,5,7,9
char     GPL_Alphabet[95] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz~!@#$%^&()[]{}_\\|;:,.`'\"<>+-*/=? ";
#define GPL_PRINT_BUFFER_SIZE   50
static char GPLPrintBuffer[GPL_PRINT_BUFFER_SIZE];

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */
void GPL_SetPenSize( uint16_t psize )
{
    GPL_PenSize = psize;
}

uint16_t GPL_GetPenSize( void )
{
    return GPL_PenSize;
}

uint8_t GPL_ScreenInit( void )
{
    // OLED HW Initial
    LCM_Init();
    // Clean OLED Frame Buffer
    LCM_Clean();
    // Update LCM
    LCM_Update();

    for( int Layer=0 ; Layer < GPL_LAYERS ; Layer++ )
    {
        // Show all Layer
        GPL_Layer[Layer].Show = LAYER_SHOW;
        // Clean Layer Frame Buffer
        memset(GPL_Layer[Layer].Buf, 0, sizeof(GPL_Layer[Layer].Buf));
    }
    // Set Default Layer to Graphic
    GPL_LayerSet(LAYER_GRAPHIC);

    return true;
}

void GPL_LayerSet( uint8_t index )
{
    if( index < GPL_LAYERS )
    {
        // Assign Layer Frame Buffer
        LCM_SetLayerBuf( GPL_Layer[index].Buf );
    }
}

void GPL_LayerShow( uint8_t index, GPL_LAYERSHOW show )
{
    if( index < GPL_LAYERS )
    {
        // Assign Layer Frame Buffer
        GPL_Layer[index].Show = show;
    }
}

void GPL_LayerClean( uint8_t index )
{
    if( index < GPL_LAYERS )
    {
        // Assign Layer Frame Buffer
        memset(GPL_Layer[index].Buf, 0, sizeof(GPL_Layer[index].Buf));
    }
}

void GPL_ScreenClean( void )
{
    LCM_Clean();

    // Request to Update Screen
    GPL_Invalidate = true;
}

void GPL_ScreenUpdate( void )
{
    uint8_t *pFrame = NULL;
    uint16_t  Layer, Byte;

    // If Update Screen is requested
    if( GPL_Invalidate )
    {
        pFrame = LCM_GetFrameBuf();
        memset( pFrame, 0, LCM_FRAME_SIZE );
        for( Layer=0 ; Layer < GPL_LAYERS ; Layer++ )
        {
            if( GPL_Layer[Layer].Show == LAYER_SHOW )
            {
                for( Byte=0 ; Byte<LCM_FRAME_SIZE ; Byte++ )
                {
                    pFrame[Byte]|=GPL_Layer[Layer].Buf[Byte];
                }
            }
        }
        LCM_Update();
        GPL_Invalidate = false;
    }
}

void GPL_DrawPoint( uint16_t x, uint16_t y, GPL_PIXEL pixel )
{
    if( x < (GPL_PenSize>>1) )  x=(GPL_PenSize>>1);
    if( y < (GPL_PenSize>>1) )  y=(GPL_PenSize>>1);
    LCM_Region( x-(GPL_PenSize>>1), y-(GPL_PenSize>>1), GPL_PenSize, GPL_PenSize, pixel );

    // Request to Update Screen
    GPL_Invalidate = true;
}

void GPL_DrawLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, GPL_PIXEL pixel )
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;

    delta_x = x2 - x1;
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if( delta_x > 0 )       {   incx =  1;  }
    else if( delta_x == 0 ) {   incx =  0;  }
    else                    {   incx = -1;  delta_x = -delta_x; }

    if( delta_y > 0 )       {   incy =  1;  }
    else if( delta_y == 0 ) {   incy =  0;  }
    else                    {   incy = -1;  delta_y = -delta_y; }

    if( delta_x > delta_y ) {   distance = delta_x; }
    else                    {   distance = delta_y; }

    for( t = 0; t <= distance + 1; t++ )
    {
        GPL_DrawPoint(uRow, uCol, pixel);

        xerr += delta_x;
        yerr += delta_y;

        if( xerr > distance )
        {
            xerr -= distance;
            uRow += incx;
        }

        if( yerr > distance )
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

void GPL_DrawRect( uint16_t x, uint16_t y, uint16_t w, uint16_t h, GPL_PIXEL pixel )
{
    GPL_DrawLine(     x,     y, x+w-1,     y, pixel );
    GPL_DrawLine(     x, y+h-1, x+w-1, y+h-1, pixel );
    GPL_DrawLine(     x,     y,     x, y+h-1, pixel );
    GPL_DrawLine( x+w-1,     y, x+w-1, y+h-1, pixel );
}

void GPL_FillRect( uint16_t x, uint16_t y, uint16_t w, uint16_t h, GPL_PIXEL pixel )
{
    LCM_Region( x, y, w, h, pixel );

    // Request to Update Screen
    GPL_Invalidate = true;
}

void GPL_DrawCross( uint16_t x0, uint16_t y0, uint16_t r, GPL_PIXEL pixel )
{
    GPL_DrawLine( x0-r, y0, x0+r, y0, pixel );
    GPL_DrawLine( x0, y0-r, x0, y0+r, pixel );
}

void GPL_DrawEllipse( uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, GPL_PIXEL pixel )
{
    int32_t x = 0;
    int32_t y = ry;

    int32_t rx2 = rx * rx;
    int32_t ry2 = ry * ry;
    int32_t two_rx2 = 2 * rx2;
    int32_t two_ry2 = 2 * ry2;

    int32_t px = 0;
    int32_t py = two_rx2 * y;

    // Region 1
    int32_t p = (int32_t)(ry2 - (rx2 * ry) + (0.25 * rx2));
    while (px < py)
    {
        GPL_DrawPoint( x0 + x, y0 + y, pixel );
        GPL_DrawPoint( x0 - x, y0 + y, pixel );
        GPL_DrawPoint( x0 - x, y0 - y, pixel );
        GPL_DrawPoint( x0 + x, y0 - y, pixel );

        x++;
        px += two_ry2;

        if (p < 0)
        {
            p += ry2 + px;
        }
        else
        {
            y--;
            py -= two_rx2;
            p += ry2 + px - py;
        }
    }

    // Region 2
    p = (int32_t)(ry2 * (x + 0.5) * (x + 0.5) +
                  rx2 * (y - 1) * (y - 1) -
                  rx2 * ry2);

    while (y >= 0)
    {
        GPL_DrawPoint( x0 + x, y0 + y, pixel );
        GPL_DrawPoint( x0 - x, y0 + y, pixel );
        GPL_DrawPoint( x0 - x, y0 - y, pixel );
        GPL_DrawPoint( x0 + x, y0 - y, pixel );

        y--;
        py -= two_rx2;

        if (p > 0)
        {
            p += rx2 - py;
        }
        else
        {
            x++;
            px += two_ry2;
            p += rx2 - py + px;
        }
    }
}

void GPL_FillEllipse( uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, GPL_PIXEL pixel )
{
    int32_t x = 0;
    int32_t y = ry;

    int32_t rx2 = rx * rx;
    int32_t ry2 = ry * ry;
    int32_t two_rx2 = 2 * rx2;
    int32_t two_ry2 = 2 * ry2;

    int32_t px = 0;
    int32_t py = two_rx2 * y;

    // Region 1
    int32_t p = (int32_t)(ry2 - (rx2 * ry) + (0.25 * rx2));
    while (px < py)
    {
        // Draw horizontal lines between symmetric points to fill the ellipse
        GPL_DrawLine( x0 - x, y0 + y, x0 + x, y0 + y, pixel );  // Lower horizontal line
        GPL_DrawLine( x0 - x, y0 - y, x0 + x, y0 - y, pixel );  // Upper horizontal line

        x++;
        px += two_ry2;

        if (p < 0)
        {
            p += ry2 + px;
        }
        else
        {
            y--;
            py -= two_rx2;
            p += ry2 + px - py;
        }
    }

    // Region 2
    p = (int32_t)(ry2 * (x + 0.5) * (x + 0.5) +
                  rx2 * (y - 1) * (y - 1) -
                  rx2 * ry2);

    while (y >= 0)
    {
        // Draw horizontal lines between symmetric points to fill the ellipse
        GPL_DrawLine( x0 - x, y0 + y, x0 + x, y0 + y, pixel );  // Lower horizontal line
        GPL_DrawLine( x0 - x, y0 - y, x0 + x, y0 - y, pixel );  // Upper horizontal line

        y--;
        py -= two_rx2;

        if (p > 0)
        {
            p += rx2 - py;
        }
        else
        {
            x++;
            px += two_ry2;
            p += rx2 - py + px;
        }
    }
}

void GPL_DrawCircle( uint16_t x0, uint16_t y0, uint16_t r, GPL_PIXEL pixel )
{
    GPL_DrawEllipse( x0, y0, r, r, pixel );
}

void GPL_FillCircle( uint16_t x0, uint16_t y0, uint16_t r, GPL_PIXEL pixel )
{
    GPL_FillEllipse( x0, y0, r, r, pixel );
}

#define DEG(x, y) ((int)(atan2((y), (x)) * 180.0 / M_PI + 360.0)) % 360
void GPL_DrawArc( uint16_t x0, uint16_t y0, uint16_t rx, uint16_t ry, int start_angle, int end_angle, GPL_PIXEL pixel )
{
    int32_t x = 0;
    int32_t y = ry;

    int32_t rx2 = rx * rx;
    int32_t ry2 = ry * ry;
    int32_t two_rx2 = 2 * rx2;
    int32_t two_ry2 = 2 * ry2;

    int32_t px = 0;
    int32_t py = two_rx2 * y;

    // Normalize angles to [0, 360)
    start_angle = (start_angle + 360) % 360;
    end_angle = (end_angle + 360) % 360;

    // Region 1
    int32_t p = (int32_t)(ry2 - rx2 * ry + (0.25 * rx2));
    while (px < py)
    {
        int points[4][2] = {
            {x0 + x, y0 + y},
            {x0 - x, y0 + y},
            {x0 - x, y0 - y},
            {x0 + x, y0 - y}
        };

        for (int i = 0; i < 4; i++)
        {
            int dx = points[i][0] - x0;
            int dy = y0 - points[i][1];  // Flip Y for angle
            int angle = DEG(dx, dy);

            if ((start_angle <= end_angle && angle >= start_angle && angle <= end_angle) ||
                (start_angle > end_angle && (angle >= start_angle || angle <= end_angle)))
            {
                GPL_DrawPoint( points[i][0], points[i][1], pixel );
            }
        }

        x++;
        px += two_ry2;

        if (p < 0)
        {
            p += ry2 + px;
        }
        else
        {
            y--;
            py -= two_rx2;
            p += ry2 + px - py;
        }
    }

    // Region 2
    p = (int32_t)(ry2 * (x + 0.5) * (x + 0.5) +
                  rx2 * (y - 1) * (y - 1) -
                  rx2 * ry2);

    while (y >= 0)
    {
        int points[4][2] = {
            {x0 + x, y0 + y},
            {x0 - x, y0 + y},
            {x0 - x, y0 - y},
            {x0 + x, y0 - y}
        };

        for (int i = 0; i < 4; i++)
        {
            int dx = points[i][0] - x0;
            int dy = y0 - points[i][1];
            int angle = DEG(dx, dy);

            if ((start_angle <= end_angle && angle >= start_angle && angle <= end_angle) ||
                (start_angle > end_angle && (angle >= start_angle || angle <= end_angle)))
            {
                GPL_DrawPoint( points[i][0], points[i][1], pixel );
            }
        }

        y--;
        py -= two_rx2;

        if (p > 0)
        {
            p += rx2 - py;
        }
        else
        {
            x++;
            px += two_ry2;
            p += rx2 - py + px;
        }
    }
}

void GPL_DrawBitmap( uint16_t x, uint16_t y, uint16_t w, uint16_t h, GPL_BACKGROUND background, const unsigned char *bitmap )
{
    LCM_Bitmap( x, y, w, h, background, bitmap );

    // Request to Update Screen
    GPL_Invalidate = true;
}

static void _GPL_DrawFont( uint16_t x, uint16_t y, char chr, GPL_BACKGROUND background, GPL_TEXT highlight )
{
    uint16_t chIdx = 0;
    uint8_t FontChar[FONT_WIDTH*FONT_HEIGHT/8];

    // Search chr in GPL_Alphabet[]
    while( chr != GPL_Alphabet[chIdx] && chIdx<sizeof(GPL_Alphabet) )
    { chIdx++; }
    if( chIdx>=sizeof(GPL_Alphabet) )
    { return; }

    // Extract chr bitmap from Font Bitmap, Font Bitmap height should multiple of 8 (8, 16, 24 ...)
    // Example of Two Font Char and Font Width = 7, Height = 16
    //
    // Font Bitmap Array = Byte00, Byte01, Byte02, ... Byte15,| Byte16, Byte17, ... Byte 31
    //
    //                           [ Font Char 1 ]   [ Font Char 2 ]
    //                         |    Font W=7     |     Font W=7
    // Font H = 16 | Bit[0: 7] | Byte00 - Byte07 |  Byte08 - Byte15
    //             | Bit[8:15] | Byte16 - Byte23 |  Byte24 - Byte31

    // First Row of Font
    memcpy(FontChar,            Alphabet_Fonts+(chIdx*FONT_WIDTH),                        FONT_WIDTH);
    if( FONT_HEIGHT>8 )
    {
        // Second Row of Font
        memcpy(FontChar+FONT_WIDTH, Alphabet_Fonts+((chIdx+sizeof(GPL_Alphabet))*FONT_WIDTH), FONT_WIDTH);
    }
    // Highlight(Invert) font
    if( highlight )
    {
        for(int i=0 ; i<FONT_WIDTH*(FONT_HEIGHT+(FONT_HEIGHT%8))/8 ; i++ )
        {
            FontChar[i] = ~FontChar[i];
        }
    }

    GPL_DrawBitmap(x, y, FONT_WIDTH, FONT_HEIGHT, background, FontChar);
}

// Function prototype: scales up a block of pixels in-place or into a new position
static void _GPL_SetPixel( uint16_t x, uint16_t y, GPL_PIXEL pixel )
{
    LCM_Pixel( x, y, pixel );
}

static uint8_t _GPL_GetPixel( uint16_t x, uint16_t y )
{
    return LCM_PixelGet( x, y );
}

// Scale a rectangular region from SourceLayer to TargetLayer using float Ratio
// Scale a region (x0,y0)-(x1,y1) on SourceLayer by Ratio into TargetLayer.
// Scaled pixels are allowed to go beyond original bounds.
void GPL_Scale(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, float Ratio)
{
    if (Ratio <= 0.0f)
    {
        return; // Invalid ratio
    }

    uint8_t ClipboardBuf[LCM_FRAME_SIZE];
    uint8_t *CurrentLayerBuf = LCM_GetLayerBuf();
    uint16_t src_x, src_y;

    // Copy Current Layer Buffer to ClipboardBuf
    memcpy( ClipboardBuf, CurrentLayerBuf, LCM_FRAME_SIZE );

    // Clean oringinal region pixels
    GPL_FillRect( x0, y0, w, h, PIXEL_CLEAN );

    // Set LCM Frame Buffer to a ClipboardBuf (Copied from Current Layer Buffer)
    LCM_SetLayerBuf( ClipboardBuf );

    for( src_y = 0; src_y < h; src_y++ )
    {
        for( src_x = 0; src_x < w; src_x++ )
        {
            // Get Pixel from ClipboardBuf (Copied from Current Layer Buffer)
            GPL_PIXEL pixel = _GPL_GetPixel( x0 + src_x, y0 + src_y );

            if( pixel )
            {
                // Calculate scaled destination coordinates (float -> int)
                float dst_xf = x0 + src_x * Ratio;
                float dst_yf = y0 + src_y * Ratio;

                uint16_t dst_x_start = (uint16_t)floorf(dst_xf);
                uint16_t dst_y_start = (uint16_t)floorf(dst_yf);
                uint16_t dst_x_end   = (uint16_t)floorf(dst_xf + Ratio);
                uint16_t dst_y_end   = (uint16_t)floorf(dst_yf + Ratio);

                // Set LCM Frame Buffer to current Layer Buffer
                LCM_SetLayerBuf( CurrentLayerBuf );
                // Scale Up Pixels calcualte
                for (uint16_t y = dst_y_start; y < dst_y_end; y++)
                {
                    for (uint16_t x = dst_x_start; x < dst_x_end; x++)
                    {
                        _GPL_SetPixel( x, y, PIXEL_SET );  // May go beyond (x1,y1) - this is allowed
                    }
                }
                // Set LCM Frame Buffer to a ClipboardBuf (Copied from Current Layer Buffer)
                LCM_SetLayerBuf( ClipboardBuf );
            }
        }
    }

    // Set LCM Frame Buffer to current Layer Buffer
    LCM_SetLayerBuf( CurrentLayerBuf );
}

void GPL_Printf( uint16_t x, uint16_t y, GPL_BACKGROUND background, GPL_TEXT highlight, const char *format, ... )
{
    size_t len = 0;
    va_list args = { 0 };

    va_start(args, format);
    len = vsnprintf(GPLPrintBuffer, GPL_PRINT_BUFFER_SIZE, format, args);
    va_end(args);

    if( (len > 0) && (len < GPL_PRINT_BUFFER_SIZE) )
    {
        GPLPrintBuffer[len] = '\0';
        for( int chrIdx=0 ; chrIdx<strlen(GPLPrintBuffer); chrIdx++ )
        {
            _GPL_DrawFont( x+(chrIdx*FONT_WIDTH), y, GPLPrintBuffer[chrIdx], background, highlight );
        }
    }
}
/* *****************************************************************************
 End of File
 */

