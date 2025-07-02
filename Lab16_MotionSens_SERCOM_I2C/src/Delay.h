/* ************************************************************************** */
/** Delay.h - Multi-channel Non-blocking Software Delay using Hardware Timer

  @Company
    Microchip Technology Inc.

  @File Name
    Delay.h

  @Summary
    Interface for software-based delay functionality using hardware timer interrupts.

  @Description
    This header provides function prototypes for a delay module that supports multiple
    non-blocking delay channels. A hardware Timer/Counter (e.g., TC2) is used to generate
    1ms ticks, enabling accurate timing control across multiple operations.

    Features:
    - Initialization of the timer interrupt callback
    - Multiple independent software delay channels
    - Blocking and non-blocking delay support

    Function List:
    - `DelayInitial()`           : Initialize the timer callback and start the timer
    - `DelayIsComplete(idx)`     : Check if the delay for the given channel index is complete
    - `DelayMS_NonBlock(idx, ms)`: Start a non-blocking delay for the given channel
    - `DelayMS(idx, ms)`         : Blocking delay that waits until time elapses

    Note:
    - The macro `MAX_DELAY_TIMER` defines the number of supported delay channels
    - Assumes TC2 is configured to trigger at 1ms intervals

 */
/* ************************************************************************** */

#ifndef _DELAY_H    /* Guard against multiple inclusion */
#define _DELAY_H


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
enum {

    DELAY_TIMER_OLED_DELAY,
    MAX_DELAY_TIMER
};

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
void DelayInitial( void );
bool DelayIsComplete(uint8_t idx);
void DelayMS_NonBlock(uint8_t idx, uint32_t ms);
void DelayMS(uint8_t idx, uint32_t ms);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _DELAY_H */

/* *****************************************************************************
 End of File
 */

