/* ************************************************************************** */
/** @file myprintf.h

  @brief
    Header file for custom UART-based printf with VT100 control sequences.

  @details
    This header declares the `myprintf()` function, a replacement for standard
    `printf()` designed for embedded systems using UART. It supports VT100 terminal
    control commands for formatting output and controlling cursor behavior.

    To use this module, replace the UART transmit function macros as needed to
    match your hardware abstraction layer (HAL) or PLIB.

    Supported features in `myprintf()`:
    - Basic printf formatting (integers, strings, etc.)
    - VT100 cursor control using `@x,y;`
    - RGB foreground and background color control: `#FGC:RRGGBB;`, `#BGC:RRGGBB;`
    - Common terminal control codes: `#RST;`, `#CLS`, `#C2E;`, etc.
    - Text formatting: bold, underline, inverse, dim
    - Escape sequences for literal `@` and `#` using `\\@` and `\\#`

  @note
    - `myprintf()` is blocking and assumes UART is ready for transmission.
    - This function is designed for terminal environments that support ANSI/VT100.

  @author
    Microchip Technology Inc.

  @date
    2025

 */
/* ************************************************************************** */

#ifndef _MYPRINTF_H    /* Guard against multiple inclusion */
#define _MYPRINTF_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Constants                                                         */
/* ************************************************************************** */
/* ************************************************************************** */


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
/* ************************************************************************** */
void myprintf(const char *format, ...);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _MYPRINTF_H */

/* *****************************************************************************
 End of File
 */

