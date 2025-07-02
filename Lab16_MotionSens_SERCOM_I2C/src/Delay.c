/* ************************************************************************** */
/** Delay.c - Multi-channel Non-blocking Software Delay using Hardware Timer

  @Company
    Microchip Technology Inc.

  @File Name
    Delay.c

  @Summary
    Software delay module utilizing a hardware timer for multi-channel millisecond delays.

  @Description
    This module implements a software delay system based on a hardware timer interrupt
    (Timer/Counter TC2). It maintains internal counters for each delay channel and supports
    both blocking and non-blocking delays, suitable for time-based event scheduling in embedded systems.

    Key Functionalities:
    - Timer initialization and callback setup
    - Multiple indexed delay counters (channels)
    - Non-blocking delay: start and poll without halting CPU
    - Blocking delay: CPU waits until completion

    Implemented Functions:
    - `DelayInitial()`           : Configure the timer and prepare delay tracking
    - `DelayIsComplete(idx)`     : Return true if the delay for the given channel is done
    - `DelayMS_NonBlock(idx, ms)`: Begin a non-blocking delay for a specific index
    - `DelayMS(idx, ms)`         : Wait until a delay completes (blocking)

 */
/* ************************************************************************** */

#include "definitions.h"  // SYS function prototypes
#include "Delay.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
// Timer PLIB function Name (SAM/PIC32C only) in 1ms period
#define TC_PLIB_CALLBACK(...) TC2_TimerCallbackRegister(__VA_ARGS__)
#define TC_PLIB_START(...)    TC2_TimerStart(__VA_ARGS__)

static bool IsDelayInited = false;
volatile uint32_t IntervalTimerCount[MAX_DELAY_TIMER];
volatile uint32_t IntervalTimerTarget[MAX_DELAY_TIMER];

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */
static void Delay_EventHandler(TC_TIMER_STATUS status, uintptr_t context)
{
    int idx=0;

    do {
        if (IntervalTimerCount[idx] != 0)
        {
            IntervalTimerCount[idx]++;
        }
    } while( ++idx < MAX_DELAY_TIMER );
}

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */
void DelayInitial( void )
{
    TC_PLIB_CALLBACK(Delay_EventHandler, 0);
    TC_PLIB_START();

    IsDelayInited = true;
}

bool DelayIsComplete(uint8_t idx)
{
    if (IntervalTimerCount[idx] != 0 &&
        IntervalTimerCount[idx] > IntervalTimerTarget[idx])
    {
        // Disable current Interval Timer
        IntervalTimerCount[idx] = 0;
        return true;
    }

    return false;
}

void DelayMS_NonBlock(uint8_t idx, uint32_t ms)
{
    // Make sure the Delay already Initialized
    if( !IsDelayInited ) DelayInitial();

    // TC in 1ms period
    IntervalTimerTarget[idx] = ms;
    IntervalTimerCount[idx]  = 1; // Start Delay Counter
}

void DelayMS(uint8_t idx, uint32_t ms)
{
    DelayMS_NonBlock( idx, ms );
    while( !DelayIsComplete(idx) ) {}
}

/* *****************************************************************************
 End of File
 */

