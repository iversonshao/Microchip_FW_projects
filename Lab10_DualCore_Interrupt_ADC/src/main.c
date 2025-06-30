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
#include <string.h>
#include <stdio.h>
#include "definitions.h"                // SYS function prototypes
#include "myprintf.h"

uint32_t i = 0;
volatile uint8_t TC0_HasExpired = 0;
volatile uint8_t TC1_HasExpired = 0;
uint8_t USART1_TransmitData[100];
uint8_t USART1_ReceiveData[1];
volatile uint8_t USART1_IsReceived = 0;
volatile uint8_t USART1_IsTransmitted = 1;
uint16_t ADC0_Result;
uint16_t ADC1_Result[2];
// TODO 10.02
volatile uint8_t ADC0_IsCompleted = 0;
volatile uint8_t ADC1_IsCompleted = 0;
volatile int8_t ADC1_ChannelIdx = 0;

// TODO 10.03
void ADC0_Complete( ADC_STATUS status, uintptr_t context )
{	
    if( status & ADC_STATUS_RESRDY )
    {		
        ADC0_Result = ADC0_ConversionResultGet();
        ADC0_IsCompleted = 1;
    }
}

// TODO 10.04
void ADC1_Complete( ADC_STATUS status, uintptr_t context )
{	
    if( status & ADC_STATUS_RESRDY )
    {		
        ADC1_Result[ADC1_ChannelIdx] = ADC1_ConversionResultGet();
        ADC1_ChannelIdx = ( ADC1_ChannelIdx + 1 ) % 2;
        if( ADC1_ChannelIdx == 0 )
        {
            ADC1_ChannelSelect( ADC_POSINPUT_AIN10, ADC_INPUTCTRL_MUXNEG_AVSS );
            ADC1_IsCompleted = 1;
        }else
        {
            ADC1_ChannelSelect( ADC_POSINPUT_AIN11, ADC_INPUTCTRL_MUXNEG_AVSS );
        }
    }
}

void USART1_Transmit(uintptr_t context)
{
    USART1_IsTransmitted = 1;
}

void USART1_Receive(uintptr_t context)
{
    USART1_IsReceived = 1;
}

void TC0_TimerExpired(TC_TIMER_STATUS status, uintptr_t context)
{
    if (status & TC_INTFLAG_OVF_Msk)
    {
        TC0_HasExpired = 1;
    }
}

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
    
    TC0_TimerCallbackRegister(TC0_TimerExpired, (uintptr_t) NULL);
    TC0_TimerStart();
    
    TC1_TimerCallbackRegister(TC1_TimerExpired, (uintptr_t) NULL);
    TC1_TimerStart();
    
    SERCOM1_USART_ReadCallbackRegister(USART1_Receive, (uintptr_t) NULL);
    SERCOM1_USART_WriteCallbackRegister(USART1_Transmit, (uintptr_t) NULL);
    SERCOM1_USART_Read(USART1_ReceiveData, 1);
    
    myprintf( "#RST;#CLS;#HID;" );    // Reset Console, Clear screen and Hide cursor
    
// TODO 10.05
    ADC0_CallbackRegister( ADC0_Complete, (uintptr_t )NULL );
    ADC1_CallbackRegister( ADC1_Complete, (uintptr_t )NULL );
    ADC0_Enable();
    ADC1_Enable();

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
        if (TC0_HasExpired)
        {
            TC0_HasExpired = 0;
            LED1_Toggle();

            myprintf( "@1,1;Hello World!!" );
            
// TODO 10.01
            // ADC0_ConversionStart();
            // while( !ADC0_ConversionStatusGet() ){}
            // ADC0_Result = ADC0_ConversionResultGet();
            // myprintf( "@1,2;VR1 Value : %4d", ADC0_Result );
            //
            // ADC1_ChannelSelect( ADC_POSINPUT_AIN10, ADC_INPUTCTRL_MUXNEG_AVSS );
            // ADC1_ConversionStart();
            // while( !ADC1_ConversionStatusGet() ){}
            // ADC1_Result[0] = ADC1_ConversionResultGet();
            // myprintf( "@1,3;Temperature Value : %4d", ADC1_Result[0] );
            //
            // ADC1_ChannelSelect( ADC_POSINPUT_AIN11, ADC_INPUTCTRL_MUXNEG_AVSS );
            // ADC1_ConversionStart();
            // while( !ADC1_ConversionStatusGet() ){}
            // ADC1_Result[1] = ADC1_ConversionResultGet();
            // myprintf( "@1,4;Lightness Value : %4d", ADC1_Result[1] );
            
// TODO 10.06
            ADC0_ConversionStart();
            ADC1_ConversionStart();
        }

        if (TC1_HasExpired)
        {
            TC1_HasExpired = 0;
            LED2_Toggle();
        }
        
        if (USART1_IsReceived)
        {
            USART1_IsReceived = 0;

            myprintf( "@1,5;Received Data : %1c", USART1_ReceiveData[0] );
            SERCOM1_USART_Read( USART1_ReceiveData, 1 );
        }
        
// TODO 10.07
        if( ADC0_IsCompleted )
        {
            ADC0_IsCompleted = 0;
            myprintf( "@1,2;VR1 Value : %4d", ADC0_Result );
        }
        
// TODO 10.08
        if( ADC1_IsCompleted )
        {
            ADC1_IsCompleted = 0;
            myprintf( "@1,3;Temperature Value : %4d", ADC1_Result[0] );
            myprintf( "@1,4;Lightness Value : %4d", ADC1_Result[1] );
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

