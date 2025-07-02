/* ************************************************************************** */
/** LCM.c - SSD1306 OLED Display Driver Implementation

  @Company
    Microchip Technology Inc.

  @File Name
    LCM.c

  @Summary
    Source file for low-level SSD1306 OLED driver routines.

  @Description
    This file provides the implementation of a hardware abstraction layer (HAL) for the
    SSD1306 OLED display controller. It handles communication with the OLED, manages the
    framebuffer, and provides pixel and region-level drawing capabilities.

    Key Functionalities:
    - Framebuffer management and layer assignment
    - Coordinate-to-buffer mapping in SSD1306 page mode
    - Drawing primitives (pixel, region, bitmap)
    - OLED initialization and data transmission (e.g., via SPI/I2C)
    - Refreshing the screen with updated data

    Implemented Functions:
    - `LCM_SetLayerBuf`   : Assign a buffer as the drawing layer
    - `LCM_GetLayerBuf`   : Return pointer to the current drawing buffer
    - `LCM_GetFrameBuf`   : Return pointer to the display¡¦s internal framebuffer
    - `LCM_Init`          : Initialize OLED controller and communication interface
    - `LCM_Clean`         : Clear the current framebuffer
    - `LCM_Update`        : Refresh display with current framebuffer content
    - `LCM_Pixel`         : Set or clear a pixel at specific coordinates
    - `LCM_PixelGet`      : Retrieve the value of a pixel at specific coordinates
    - `LCM_Region`        : Fill a rectangular region in the framebuffer
    - `LCM_Bitmap`        : Render a bitmap with optional background setting

 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include <string.h>
#include "definitions.h"
#include "LCM.h"


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
// APP055 SERCOM5_SPI Pin Configuration
// SERCOM5 SPI : MOSI (PB02)
// SERCOM5 SPI : SCK  (PB03)
// OLED_nCS    : SS   (PB00), GPIO Output Latch high
// OLED_nRST   : Reset(PB31), GPIO Output Latch high
// OLED_A0     : A0   (PB01), GPIO Output Latch low (Command:0, Data:1)
#define LCM_HW_RESET_ENABLE       1  // Enable OLED Reset PIN on PB31
#define LCM_SPI_CS_GPIO           1  // Enable SPI GPIO Slave Select on PB00
#define LCM_SPI_Write(...)        SERCOM5_SPI_Write(__VA_ARGS__)
#define LCM_SPI_IsWriteBusy(...)  SERCOM5_SPI_IsTransmitterBusy(__VA_ARGS__)
#define LCM_RESET_Enable(...)     OLED_nRST_Clear(__VA_ARGS__)
#define LCM_RESET_Disable(...)    OLED_nRST_Set(__VA_ARGS__)
#define LCM_BUS_CMD(...)          OLED_A0_Clear(__VA_ARGS__)
#define LCM_BUS_DAT(...)          OLED_A0_Set(__VA_ARGS__)

#if LCM_SPI_CS_GPIO
#define LCM_SPI_CS_Enable(...)    OLED_nCS_Clear(__VA_ARGS__)
#define LCM_SPI_CS_Disable(...)   OLED_nCS_Set(__VA_ARGS__)
#endif

uint8_t LCM_FrameBuf[LCM_FRAME_SIZE]; // (128x64)/8, 1bit/pixel
uint8_t *LCM_pFrameBuf = NULL; // The Frame Buffer Pointer
const uint8_t LCM_InitCMD[]={
    0xAE,          // DISPLAY OFF
    0xD5,          // SET OSC FREQUENY
    0x80,          // divide ratio = 1 (bit 3-0), OSC (bit 7-4)
    0xA8,          // SET MUX RATIO
    0x3F,          // 64MUX
    0xD3,          // SET DISPLAY OFFSET
    0x00,          // offset = 0
    0x40,          // set display start line, start line = 0
    0x8D,          // ENABLE CHARGE PUMP REGULATOR
    0x14,          //
    0x20,          // SET MEMORY ADDRESSING MODE
    0x02,          // horizontal addressing mode
    0xA1,          // set segment re-map, column address 127 is mapped to SEG0
    0xC8,          // set COM/Output scan direction, remapped mode (COM[N-1] to COM0)
    0xDA,          // SET COM PINS HARDWARE CONFIGURATION
    0x12,          // alternative COM pin configuration
    0x81,          // SET CONTRAST CONTROL
    0xCF,          //
    0xD9,          // SET PRE CHARGE PERIOD
    0xF1,          //
    0xDB,          // SET V_COMH DESELECT LEVEL
    0x40,          //
    0xA4,          // DISABLE ENTIRE DISPLAY ON
    0xA6,          // TEXT_NORMAL MODE (A7 for inverse display)
    0xAF           // DISPLAY ON
};

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************
// Example of Pixel to Bit calculate
//     X0  X1  X2  X3  X4  X5  X6  X7  X8  X9  X10 X11 X12 X13 X14 X15
//     B00 B01 B02 B03 B04 B05 B06 B07 B08 B09 B10 B11 B12 B13 B14 B15
// Y00 b00                                                         b00
// Y01 b01                                                         b01
// Y02 b02                                                         b02
// Y03 b03                                                         b03
// Y04 b04                                                         b04
// Y05 b05                                                         b05
// Y06 b06              #                                          b06
// Y07 b07                                                         b07
//
//     B16 B17 B18 B19 B20 B21 B22 B23 B24 B25 B26 B27 B28 B29 B30 B31
// Y08 b00                                                         b00
// Y09 b01                                                         b01
// Y10 b02                                                         b02
// Y11 b03                                                         b03
// Y12 b04                                                         b04
// Y13 b05                                                         b05
// Y14 b06                                                  *      b06
// Y15 b07                                                         b07
//
// WxH = 16x16 = 256 pixels(bit) > 256/8 = 32 Bytes
//
// * = (X,Y)  = (13,14)
// PixelByte  = x+((y/8)*W) = 13+((14/8)*16) = B29
// PixelOfset = y%8         = 14%8           = b06
//
// # = (X,Y)  = (4,6)
// PixelByte  = x+((y/8)*W) =  4+(( 6/8)*16) = B04
// PixelOfset = y%8         =  6%8           = b06
//
static void _Pixel2Bit( uint16_t x, uint16_t y, uint8_t pixel )
{
    uint16_t Byte;
    uint8_t  Bit;

    if( x>=LCM_WIDTH || y>=LCM_HEIGHT ) return;

    Byte = x+((int)(y/8)*LCM_WIDTH);
    Bit  = y%8;
    LCM_pFrameBuf[Byte] = (LCM_pFrameBuf[Byte]&(~(1<<Bit)))|(pixel<<Bit);
}

static uint8_t _PixelGet( uint16_t x, uint16_t y )
{
    uint16_t Byte;
    uint8_t  Bit;

    if( x>=LCM_WIDTH || y>=LCM_HEIGHT ) return 0;

    Byte = x+((int)(y/8)*LCM_WIDTH);
    Bit  = y%8;
    return LCM_pFrameBuf[Byte]&(1<<Bit);
}

void LCM_SetLayerBuf( uint8_t *pLayer )
{
    // Assign LayerBuf for Drawing
    LCM_pFrameBuf = pLayer;
}

uint8_t* LCM_GetLayerBuf( void )
{
    // Return current LayerBuf
    return LCM_pFrameBuf;
}

uint8_t* LCM_GetFrameBuf( void )
{
    // Return LCM Frame Buffer Address for Merge
    return LCM_FrameBuf;
}

uint8_t LCM_Init( void )
{
#if LCM_HW_RESET_ENABLE
    // Reset Start
    LCM_RESET_Enable();
    // Software Delay 10ms
    for( int i=0 ; i<(CPU_CLOCK_FREQUENCY/100) ; i++ ) {}
    // Reset Release
    LCM_RESET_Disable();
    // Software Delay 100ms
    for( int i=0 ; i<(CPU_CLOCK_FREQUENCY/10) ; i++ ) {}
#endif

#if LCM_SPI_CS_GPIO
    // Chip Enable
    LCM_SPI_CS_Enable();
#endif

    // Command Transfer
    LCM_BUS_CMD();

    // Send LCM Init Command
    LCM_SPI_Write(( void* )LCM_InitCMD, sizeof (LCM_InitCMD));
    while( LCM_SPI_IsWriteBusy() ) {}

#if LCM_SPI_CS_GPIO
    // Chip Disable
    LCM_SPI_CS_Disable();
#endif

    // Assign Default FrameBuf
    LCM_pFrameBuf = LCM_FrameBuf;

    return true;
}

void LCM_Clean( void )
{
    // Erase Frame Buffer
    memset(( void* )LCM_pFrameBuf, 0, 1024);
}

void LCM_Update( void )
{
    uint8_t *pFrame = LCM_FrameBuf;
    uint8_t PixelAddr[3] = {0x00, 0x10, 0xB0};

#if LCM_SPI_CS_GPIO
    // Chip Enable
    LCM_SPI_CS_Enable();
#endif

    // Update full Frame
    while( PixelAddr[2] < (0xB0+(LCM_HEIGHT/8)) )
    {
        // Command Transfer (Pixel Address)
        LCM_BUS_CMD();
        LCM_SPI_Write(( void* )PixelAddr, 3);
        while( LCM_SPI_IsWriteBusy() ) {}

        // Data Transfer (Pixel Data)
        LCM_BUS_DAT();
        LCM_SPI_Write(( void* )pFrame, LCM_WIDTH);
        while( LCM_SPI_IsWriteBusy() ) {}

        // Index to next 8 row for Pixels
        pFrame += LCM_WIDTH;
        PixelAddr[2]++;
    }

#if LCM_SPI_CS_GPIO
    // Chip Disable
    LCM_SPI_CS_Disable();
#endif
}

void LCM_Pixel( uint16_t x, uint16_t y, uint8_t pixel )
{
    if( x>=LCM_WIDTH || y>=LCM_HEIGHT ) return;

    // Update Pixel to Frame Buffer
    _Pixel2Bit( x, y, pixel );
}

uint8_t LCM_PixelGet( uint16_t x, uint16_t y )
{
    if( x>=LCM_WIDTH || y>=LCM_HEIGHT ) return 0;

    // Get Pixel from Frame Buffer
    return _PixelGet( x, y );
}

void LCM_Region( uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t pixel )
{
    uint16_t Col, Row;

    if( x+w>LCM_WIDTH || y+h>LCM_HEIGHT ) return;

    // Update a Region to Frame Buffer
    for( Row = 0 ; Row < h ; Row++ )
    {
        for( Col = 0 ; Col < w ; Col++ )
        {
            _Pixel2Bit( x+Col, y+Row, pixel );
        }
    }
}

void LCM_Bitmap( uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t background, const unsigned char *bitmap )
{
    uint16_t Col, Row;
    uint16_t Byte;
    uint8_t Bit;
    uint8_t Pixel;

    // Update a Bitmap to Frame Buffer
    for( Row = 0 ; Row < h ; Row++ )
    {
        for( Col = 0 ; Col < w ; Col++ )
        {
            Byte = Col+((int)(Row/8)*w);
            Bit  = Row%8;
            Pixel  = (bitmap[Byte]>>Bit)&0x1;
            if( Pixel || (!Pixel && background) )
            {
                _Pixel2Bit( x+Col, y+Row, Pixel );
            }
        }
    }
}

/* *****************************************************************************
 End of File
 */

