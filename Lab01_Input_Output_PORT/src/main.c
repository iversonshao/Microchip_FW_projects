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

// TODO 1.01
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

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
// TODO 1.02
        // for ( i = 0 ; i < 2000000 ; i++ ){}
        // LED1_Toggle();
        // LED2_Toggle();

        if ( ++i >= 2000000 )
        {
            i = 0;
            // *(__IO uint32_t *) (0x4100801CU) = ((uint32_t)1U << 12U);
            (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 12U));
//            LED1_Toggle();
//            LED2_Toggle();
        }
// TODO 1.03
        if ( BT1_Get() ) LED3_Clear();
        else             LED3_Set();
        
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

