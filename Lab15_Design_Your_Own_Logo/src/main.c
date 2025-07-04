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
#include "Delay.h"
#include "GraphicLib.h"
#include "GraphicApp.h"
#include "Microchip_Logo.h"

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
uint8_t Duty = 50;
int8_t DutyDistance = 2;

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


// TODO 15.01 - Advanced - Store into multiple image arrays
void MicrochipLogoWave( uint32_t ms )
{
    const unsigned char* WaveImageSequences[11] = {
    Microchip_Logo_wave1,  Microchip_Logo_wave2, Microchip_Logo_wave3,
    Microchip_Logo_wave4,  Microchip_Logo_wave5, Microchip_Logo_wave6,
    Microchip_Logo_wave7,  Microchip_Logo_wave8, Microchip_Logo_wave9, 
    Microchip_Logo};

    GPL_LayerSet(LAYER_GRAPHIC);
    for(int i=0 ; i<10 ; i++ )
    {
        GPL_LayerClean(LAYER_GRAPHIC);
        GPL_DrawBitmap(0, 0, LCM_WIDTH, LCM_HEIGHT, BG_SOLID, WaveImageSequences[i]);
        GPL_ScreenUpdate();
        DelayMS(DELAY_TIMER_OLED_DELAY, ms);
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

    // OLED HW and Graphic Layers Initial and Clean all Frame Buffer
    GPL_ScreenInit();

    // Show Only Graphic Layers
    GPL_LayerShow( LAYER_GRAPHIC, LAYER_SHOW );
    GPL_LayerShow( LAYER_TEXT,    LAYER_HIDE );

    // Splash Logo In
    DrawSplashAnimation( 128, 0, 0, 0, 4, Microchip_Logo, LAYER_GRAPHIC );

    // Delay after animation ends
    DelayMS(DELAY_TIMER_OLED_DELAY, 1000);
    
// TODO 15.02 - Advanced - Store into multiple image arrays
    // Microchip Logo Wave animation
    MicrochipLogoWave(50);
    
    // Delay after animation ends
    DelayMS(DELAY_TIMER_OLED_DELAY, 500);
    
    // Splash Logo Out
    DrawSplashAnimation( 0, 0, -128, 0, 8, Microchip_Logo, LAYER_GRAPHIC );

    // Show Both Text and Graphic Layers
    GPL_LayerShow( LAYER_GRAPHIC, LAYER_SHOW );
    GPL_LayerShow( LAYER_TEXT,    LAYER_SHOW );

    // Draw on Text Layer
    GPL_LayerSet( LAYER_TEXT );

    // Clean Text Layer Buffer
    GPL_LayerClean( LAYER_TEXT );
    GPL_Printf( 0,  0, BG_SOLID, TEXT_NORMAL, "SysClock  : %dMHz", CPU_CLOCK_FREQUENCY/1000000 );
    GPL_Printf( 0,  8, BG_SOLID, TEXT_NORMAL, "TC0 period:  %ldms", (TC0_Timer16bitPeriodGet()+1)*1000 / TC0_TimerFrequencyGet());
    GPL_Printf( 0, 16, BG_SOLID, TEXT_NORMAL, "TC1 period:  %ldms", (TC1_Timer16bitPeriodGet()+1)*1000 / TC1_TimerFrequencyGet());
    GPL_Printf( 0, 56, BG_SOLID, TEXT_NORMAL, "Received Data :");
    GPL_ScreenUpdate();

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

            Duty += DutyDistance;
            if (Duty >= 100 || Duty <= 0)
                DutyDistance = -DutyDistance;

            TCC1_PWM24bitDutySet(TCC1_CHANNEL3, (((uint32_t) Duty * TCC1_PWM24bitPeriodGet() + 1) / 100));
            GPL_Printf( 0, 24, BG_SOLID, TEXT_NORMAL, "PWM Duty  : %3d%%", Duty);
            GPL_ScreenUpdate();
        }
        
        if (USART1_IsReceived)
        {
            USART1_IsReceived = 0;

            myprintf( "@1,5;Received Data : %1c", USART1_ReceiveData[0] );
            SERCOM1_USART_Read( USART1_ReceiveData, 1 );
            GPL_Printf( 0, 56, BG_SOLID, TEXT_NORMAL, "Received Data : %c", USART1_ReceiveData[0]);
            GPL_ScreenUpdate();        
        }
        
        if( ADC0_IsCompleted )
        {
            ADC0_IsCompleted = 0;
            myprintf( "@1,2;VR1 Value : %4d", ADC0_Result );
            GPL_Printf( 0, 32, BG_SOLID, TEXT_NORMAL, "VR1   : %4d", ADC0_Result);
        }
        
        if( ADC1_IsCompleted )
        {
            ADC1_IsCompleted = 0;
            myprintf( "@1,3;Temperature Value : %4d", ADC1_Result[0] );
            myprintf( "@1,4;Lightness Value : %4d", ADC1_Result[1] );
            GPL_Printf( 0, 40, BG_SOLID, TEXT_NORMAL, "Temp  : %4d", ADC1_Result[0]);
            GPL_Printf( 0, 48, BG_SOLID, TEXT_NORMAL, "Light : %4d", ADC1_Result[1]);
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

