/* ************************************************************************** */
/** MotionAPP.h - High-Level OLED Graphic Application API Header

  @Company
    Microchip Technology Inc.

  @File Name
    MotionAPP.h

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
    - `DrawBalanceBall'
    - `DrawHorizontalBubble'
    - `DrawRotationCube'
    - `ViewRotationCube'
 */
/* ************************************************************************** */

#ifndef _MOTIONAPP_H    /* Guard against multiple inclusion */
#define _MOTIONAPP_H

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
void DrawBalanceBall(short ax, short ay, bool reset, uint8_t layer);
void DrawLevelBubble(short ax, short ay, bool reset, uint8_t layer);
void DrawRotationCube(short ax, short ay, bool reset, uint8_t layer);
void ViewRotationCube(short ax, short ay, short az, short gx, short gy, short gz, bool reset, uint8_t layer);
/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _MOTIONAPP_H */

/* *****************************************************************************
 End of File
 */

