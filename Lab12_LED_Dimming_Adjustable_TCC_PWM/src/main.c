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
volatile uint8_t ADC0_IsCompleted = 0;
volatile uint8_t ADC1_IsCompleted = 0;
volatile int8_t ADC1_ChannelIdx = 0;

void ADC0_Complete( ADC_STATUS status, uintptr_t context )
{	
    if( status & ADC_STATUS_RESRDY )
    {		
        ADC0_Result = ADC0_ConversionResultGet();
        ADC0_IsCompleted = 1;
    }
}

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
    
    ADC0_CallbackRegister( ADC0_Complete, (uintptr_t )NULL );
    ADC1_CallbackRegister( ADC1_Complete, (uintptr_t )NULL );
    ADC0_Enable();
    ADC1_Enable();

    TCC1_PWMStart();

    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        
        if (TC0_HasExpired)
        {
            TC0_HasExpired = 0;
            LED1_Toggle();

            myprintf( "@1,1;Hello World!!" );
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
        
        if( ADC0_IsCompleted )
        {
            ADC0_IsCompleted = 0;
            myprintf( "@1,2;VR1 Value : %4d", ADC0_Result );
// TODO 12.01
            TCC1_PWM24bitDutySet(TCC1_CHANNEL3, (((uint32_t) ADC0_Result * TCC1_PWM24bitPeriodGet() + 1) / 4095));
// TODO 12.02
            myprintf("@1,10;Lab12 Mathematical comparison:");
            myprintf("@1,11;DIV before MUL  : %6d", (ADC0_Result / 4095) * (TCC1_PWM24bitPeriodGet() + 1));
            myprintf("@1,12;16-bits Integer : %6d", (uint16_t) (ADC0_Result * (TCC1_PWM24bitPeriodGet() + 1)) / 4095);
            myprintf("@1,13;32-bits Integer : %6d", (uint32_t) (ADC0_Result * (TCC1_PWM24bitPeriodGet() + 1)) / 4095);
        }

        
        if( ADC1_IsCompleted )
        {
            ADC1_IsCompleted = 0;
            myprintf( "@1,3;Temperature Value : %4d", ADC1_Result[0] );
            myprintf( "@1,4;Lightness Value : %4d", ADC1_Result[1] );
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

