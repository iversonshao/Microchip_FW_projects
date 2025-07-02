/* ************************************************************************** */
/** LCM.h - SSD1306 OLED Display Driver Header

  @Company
    Microchip Technology Inc.

  @File Name
    LCM.h

  @Summary
    Header for low-level SSD1306 OLED driver functions.

  @Description
    This header file defines the interface for controlling an SSD1306-based OLED display.
    It includes framebuffer operations, pixel manipulation, region drawing, bitmap rendering,
    and display updates. Designed for use with an external graphics library that manages drawing
    layers and display logic.

    Main Features:
    - Framebuffer assignment and access
    - Pixel-level set/get functions
    - Region and bitmap rendering
    - Display initialization and update (flush)

    Function List:
    - `LCM_SetLayerBuf`   : Set the framebuffer to be used for drawing
    - `LCM_GetLayerBuf`   : Get the pointer to the current drawing buffer
    - `LCM_GetFrameBuf`   : Get the pointer to the internal framebuffer
    - `LCM_Init`          : Initialize the SSD1306 OLED module
    - `LCM_Clean`         : Clear the framebuffer content
    - `LCM_Update`        : Transfer framebuffer to display
    - `LCM_Pixel`         : Set or clear a specific pixel
    - `LCM_PixelGet`      : Get the current value of a specific pixel
    - `LCM_Region`        : Fill a rectangular region with a pixel value
    - `LCM_Bitmap`        : Draw a bitmap to the display at a given location

 */
/* ************************************************************************** */

#ifndef _LCM_H    /* Guard against multiple inclusion */
#define _LCM_H

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Constants                                                         */
/* ************************************************************************** */
/* ************************************************************************** */

#define LCM_WIDTH   128
#define LCM_HEIGHT  64
#define LCM_FRAME_SIZE  ((LCM_WIDTH*LCM_HEIGHT)/8) // (128x64)/8) = 1024, 1bit/pixel

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************
void LCM_SetLayerBuf( uint8_t *pLayer );
uint8_t* LCM_GetLayerBuf( void );
uint8_t* LCM_GetFrameBuf( void );
uint8_t LCM_Init( void );
void LCM_Clean( void );
void LCM_Update( void );
void LCM_Pixel( uint16_t x, uint16_t y, uint8_t pixel );
uint8_t LCM_PixelGet( uint16_t x, uint16_t y );
void LCM_Region( uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t pixel );
void LCM_Bitmap( uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t background, const unsigned char *bitmap );

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _LCM_H */

/* *****************************************************************************
 End of File
 */

