/* ************************************************************************** */
/** GraphicAPP.h - High-Level OLED Graphic Application API Header

  @Company
    Microchip Technology Inc.

  @File Name
    GraphicAPP.h

  @Summary
    Header file defining high-level graphical application functions for OLED displays.

  @Description
    This header declares reusable application-level drawing functions built on top
    of the low-level GraphicLib. These APIs offer ready-to-use graphical features such
    as animated clocks, logos, demo graphics, and bitmap splash screens for embedded
    systems using monochrome OLED displays.

    Application Features:
    - Analog + digital clock with animation
    - Animated logo splash screen
    - Microchip logo and Doraemon face rendering
    - General-purpose graphic demo drawings

    Provided API Functions:
    - `DrawClockAnimationInit`
    - `DrawClockAnimation`
    - `DrawGraphicElements`
    - `DrawDoraemonFace`
    - `DrawMicrochipLogo`
    - `DrawSplashAnimation`
 */
/* ************************************************************************** */

#ifndef _GRAPHICAPP_H    /* Guard against multiple inclusion */
#define _GRAPHICAPP_H

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

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
void DrawClockAnimationInit( void );
void DrawClockAnimation( uint8_t layer );
void DrawGraphicElements( uint8_t layer );
void DrawDoraemonFace( uint8_t layer );
void DrawMicrochipLogo( uint8_t layer );
void DrawSplashAnimation( int x0, int y0, int x1, int y1, uint8_t step, const unsigned char *bitmap, uint8_t layer );

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _GRAPHICAPP_H */

/* *****************************************************************************
 End of File
 */

