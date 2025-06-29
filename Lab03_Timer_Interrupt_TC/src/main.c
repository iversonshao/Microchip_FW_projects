/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes

uint32_t i = 0;
// TODO 3.02
volatile uint8_t TC0_HasExpired = 0;
volatile uint8_t TC1_HasExpired = 0;
// TODO 3.03
void TC0_TimerExpired(TC_TIMER_STATUS status, uintptr_t context)
{
    if (status & TC_INTFLAG_OVF_Msk)
    {
        TC0_HasExpired = 1;
    }
}

// TODO 3.04
void TC1_TimerExpired(TC_TIMER_STATUS status, uintptr_t context)
{
    if (status & TC_INTFLAG_OVF_Msk)
    {
        TC1_HasExpired = 1;
    }
}
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );

// TODO 3.05
    TC0_TimerCallbackRegister(TC0_TimerExpired, (uintptr_t) NULL);
    TC0_TimerStart();

// TODO 3.06
    TC1_TimerCallbackRegister(TC1_TimerExpired, (uintptr_t) NULL);
    TC1_TimerStart();
    
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
// TODO 3.01
        // if ( TC0_TimerPeriodHasExpired() )
        // {
        //     LED1_Toggle();
        // }
        //
        // if ( TC1_TimerPeriodHasExpired() )
        // {
        //     LED2_Toggle();
        // }
// TODO 3.07
        if (TC0_HasExpired)
        {
            TC0_HasExpired = 0;
            LED1_Toggle();
        }

// TODO 3.08
        if (TC1_HasExpired)
        {
            TC1_HasExpired = 0;
            LED2_Toggle();
        }
        
        if ( BT1_Get() ) LED3_Clear();
        else             LED3_Set();
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

