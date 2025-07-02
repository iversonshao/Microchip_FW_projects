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
// TODO 16.01
#include "MotionApp.h"

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

// TODO 16.02
#define ICM42670P_ADDRESS   0x68
uint8_t ICM42670P_WriteData[2];
uint8_t ICM42670P_ReadData[1];
volatile uint8_t I2C3_IsDataReady = 0;
uint8_t AxisOutReg_Accel[6] = { 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };
uint8_t AxisOutReg_Gyro[6]  = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16 };
uint8_t AxisOutByte_Accel[6];
uint8_t AxisOutByte_Gyro[6];
short ax, ay, az, gx, gy, gz;

void I2C3_TransferCallback( uintptr_t contextHandle )
{
    if( ICM42670P_ReadData[0] & 0x1 )
    {
        I2C3_IsDataReady = 1;
    }
}

// TODO 16.03
void MotionSensorInit( void )
{
    ICM42670P_WriteData[0] = 0x02;
    ICM42670P_WriteData[1] = 0x10;
    SERCOM3_I2C_Write( ICM42670P_ADDRESS, ICM42670P_WriteData, 2 );
    myprintf( "@1,8;ICM42670P Software Reset Completed" );
    ICM42670P_WriteData[0] = 0x75;
    SERCOM3_I2C_WriteRead( ICM42670P_ADDRESS, ICM42670P_WriteData, 1, ICM42670P_ReadData, 1 );
    while( SERCOM3_I2C_IsBusy() );
    myprintf( "@1,9;ICM42670P WHO AM I : 0x%X", ICM42670P_ReadData[0]);
    ICM42670P_WriteData[0] = 0x20;
    ICM42670P_WriteData[1] = 0x0C;
    SERCOM3_I2C_Write( ICM42670P_ADDRESS, ICM42670P_WriteData, 2 );
    ICM42670P_WriteData[0] = 0x21;
    ICM42670P_WriteData[1] = 0x0C;
    SERCOM3_I2C_Write( ICM42670P_ADDRESS, ICM42670P_WriteData, 2 );
    ICM42670P_WriteData[0] = 0x1F;
    ICM42670P_WriteData[1] = 0x0F;
    SERCOM3_I2C_Write( ICM42670P_ADDRESS, ICM42670P_WriteData, 2 );
    myprintf( "@1,10;ICM42670P Initial Completed" );
    SERCOM3_I2C_CallbackRegister( I2C3_TransferCallback, 0 );
}

// TODO 16.04
void MotionSensorCheckDataReady( void )
{
    ICM42670P_WriteData[0] = 0x39;
    SERCOM3_I2C_WriteRead( ICM42670P_ADDRESS, ICM42670P_WriteData, 1, ICM42670P_ReadData, 1 );
}

// TODO 16.05
void MotionSensorRead( void )
{
    for( int i=0 ; i<6 ; i++ )
    {
       SERCOM3_I2C_WriteRead( ICM42670P_ADDRESS, &AxisOutReg_Accel[i], 1, &AxisOutByte_Accel[i], 1 );
       while( SERCOM3_I2C_IsBusy() );
       SERCOM3_I2C_WriteRead( ICM42670P_ADDRESS, &AxisOutReg_Gyro[i],  1, &AxisOutByte_Gyro[i],  1 );
       while( SERCOM3_I2C_IsBusy() );
    }
    ax = (short)(AxisOutByte_Accel[0]<<8|AxisOutByte_Accel[1]);
    ay = (short)(AxisOutByte_Accel[2]<<8|AxisOutByte_Accel[3]);
    az = (short)(AxisOutByte_Accel[4]<<8|AxisOutByte_Accel[5]);
    gx = (short)(AxisOutByte_Gyro[0]<<8|AxisOutByte_Gyro[1]);
    gy = (short)(AxisOutByte_Gyro[2]<<8|AxisOutByte_Gyro[3]);
    gz = (short)(AxisOutByte_Gyro[4]<<8|AxisOutByte_Gyro[5]);
}

// TODO 16.06
void MotionSensorGUI( void )
{
    static uint8_t ToggleGUI = 0;
    bool Reset =false;

    if( !BT1_Get() )
    {
        while( !BT1_Get() ){} // Button debounce
        ToggleGUI=(ToggleGUI+1)%4;
    }

    if( !BT2_Get() )
    {
        while( !BT2_Get() ) {} // Button debounce
        if( ToggleGUI==2 )  ax=ay=TC2_Timer16bitCounterGet()%2048;
        else                Reset=true;
    }

    switch( ToggleGUI )
    {
    case 0: DrawLevelBubble ( ax, ay, Reset, LAYER_GRAPHIC ); break;
    case 1: DrawBalanceBall ( ax, ay, Reset, LAYER_GRAPHIC ); break;
    case 2: DrawRotationCube( ax, ay, Reset, LAYER_GRAPHIC ); break;
    case 3: ViewRotationCube( ax, ay, az, gx, gy, gz, Reset, LAYER_GRAPHIC ); break;
    }

    GPL_ScreenUpdate();
}

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

// TODO 16.07
    MotionSensorInit();
    
    // OLED HW and Graphic Layers Initial and Clean all Frame Buffer
    GPL_ScreenInit();

    // Show Only Graphic Layers
    GPL_LayerShow( LAYER_GRAPHIC, LAYER_SHOW );
    GPL_LayerShow( LAYER_TEXT,    LAYER_HIDE );

    // Splash Logo In
    DrawSplashAnimation( 128, 0, 0, 0, 4, Microchip_Logo, LAYER_GRAPHIC );

    // Delay after animation ends
    DelayMS(DELAY_TIMER_OLED_DELAY, 1000);

    // Splash Logo Out
    DrawSplashAnimation( 0, 0, -128, 0, 8, Microchip_Logo, LAYER_GRAPHIC );

    // Show Both Text and Graphic Layers
    GPL_LayerShow( LAYER_GRAPHIC, LAYER_SHOW );
    GPL_LayerShow( LAYER_TEXT,    LAYER_SHOW );

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
// TODO 16.08
            MotionSensorCheckDataReady();
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
        }
        
        if( ADC1_IsCompleted )
        {
            ADC1_IsCompleted = 0;
            myprintf( "@1,3;Temperature Value : %4d", ADC1_Result[0] );
            myprintf( "@1,4;Lightness Value : %4d", ADC1_Result[1] );
        }
        
// TODO 16.09
        if( I2C3_IsDataReady )
        {
            MotionSensorRead();

            myprintf( "@1,6;Accel = (%+5d, %+5d, %+5d)", ax, ay, az );
            myprintf( "@1,7;Gyro  = (%+5d, %+5d, %+5d)", gx, gy, gz );

            MotionSensorGUI();

            I2C3_IsDataReady = 0;
        }

    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

