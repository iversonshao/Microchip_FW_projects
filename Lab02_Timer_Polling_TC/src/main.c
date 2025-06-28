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

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
// TODO 2.02
    TC0_TimerStart();
    TC1_TimerStart();

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
// TODO 2.01
        // if ( ++i >= 2000000 )
        // {
        //    i = 0;
        //    LED1_Toggle();
        //    LED2_Toggle();
        // }
        
// TODO 2.03
        if ( TC0_TimerPeriodHasExpired() )
        {
            LED1_Toggle();
        }

        if ( TC1_TimerPeriodHasExpired() )
        {
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

