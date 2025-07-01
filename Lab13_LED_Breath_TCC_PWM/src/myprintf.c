/* ************************************************************************** */
/** @file myprintf.c

  @brief
    Custom printf implementation using UART with VT100 terminal control support.

  @details
    This module provides a `myprintf()` function similar to the standard `printf`,
    but with extended support for VT100 terminal control sequences. It allows text
    formatting (e.g., bold, underline), screen manipulation (e.g., clear screen),
    and cursor positioning using user-friendly tags embedded in the format string.

    It parses special control sequences such as:
    - `@x,y;`       : Move cursor to x (column), y (row)
    - `#FGC:RRGGBB;`: Set foreground color using 24-bit RGB hex
    - `#BGC:RRGGBB;`: Set background color using 24-bit RGB hex
    - `#RST;`, `#CLS`, etc.: Various VT100 commands (reset, clear screen, etc.)
    - `\\@`, `\\#`  : Escaped `@` and `#` symbols

    Internally, the UART HAL functions must be updated to match the specific platform's UART driver.

  @note
    - The UART write functions are defined using `SERCOM1_USART` APIs (customize as needed).
    - Blocking UART transmission is used (`while(UART_IsWriteBusy());`).
    - This module is compatible with APP055 EVB and similar platforms using SERCOM UART.

  @author
    Microchip Technology Inc.

  @date
    2025

 */
/* ************************************************************************** */


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "definitions.h"                // SYS function prototypes
#include "myprintf.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

// APP055 EVB use the SERCOM1_USART Ring Buffer Mode for the PKOB UART communication.
#if 0
// SERCOM_USART : Ring Buffer Mode
#define UART_Write(...)        SERCOM1_USART_Write(__VA_ARGS__)
#define UART_IsWriteBusy(...)  SERCOM1_USART_WriteCountGet(__VA_ARGS__)
#else
// SERCOM_USART : Non Blocking Mode
#define UART_Write(...)        SERCOM1_USART_Write(__VA_ARGS__)
#define UART_IsWriteBusy(...)  !SERCOM1_USART_TransmitComplete(__VA_ARGS__)
#endif

#define UART_PRINT_BUFFER_SIZE   2048
static char UartPrintBuffer[UART_PRINT_BUFFER_SIZE];

// Define VT100 command prefixes for easier reference and future updates
#define CURSOR_PREFIX           '@'
#define ESCAPE_CURSOR_PREFIX    "\@"
#define ESCAPE_COMMAND_PREFIX   "\#"
#define SET_FOREGROUND_COLOR    "#FGC:"
#define SET_BACKGROUND_COLOR    "#BGC:"
#define RESET_FORMATTING        "#RST;"
#define CLEAR_SCREEN            "#CLS;"
#define CLEAR_TO_END_OF_LINE    "#C2E;"
#define CLEAR_ROW               "#CLR;"
#define HIDDEN_CURSOR           "#HID;"
#define TEXT_BOLD               "#BOL;"
#define TEXT_UNDERLINE          "#UND;"
#define TEXT_INVERSE            "#INV;"
#define TEXT_DIM                "#DIM;"

/* ************************************************************************** */
/* ************************************************************************** */
// Section: HAL Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */
static void hal_printf( const char *format, ... )
{
    size_t len = 0;
    va_list args = { 0 };

    va_start(args, format);
    len = vsnprintf(UartPrintBuffer, UART_PRINT_BUFFER_SIZE, format, args);
    va_end(args);

    if( (len > 0) && (len < UART_PRINT_BUFFER_SIZE) )
    {
        UartPrintBuffer[len] = '\0';
        UART_Write(( uint8_t* )UartPrintBuffer, len);
        while( UART_IsWriteBusy() );
    }
}

static void hal_putchar( char c )
{
    UART_Write(&c, 1);
    while( UART_IsWriteBusy() );
}

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

// Try to parse cursor position in format @x,y;
// If valid, set x/y and return pointer after command, otherwise return NULL
static const char* try_parse_cursor( const char* str, int* x, int* y, int* parsed_len )
{
    const char* ptr = str;
    if( *ptr != CURSOR_PREFIX ) return NULL;

    ptr++;
    const char* start = ptr;
    while( isdigit(*ptr) || *ptr == ',' ) ptr++;

    if( *ptr != ';' ) return NULL;

    char temp[32];
    int len = ptr - start;
    if( len >= ( int )sizeof (temp) ) return NULL;

    strncpy(temp, start, len);
    temp[len] = '\0';

    char* extra = strchr(temp, ',');
    if( !extra ) return NULL;
    if( strchr(extra + 1, ',') ) return NULL;

    if( sscanf(temp, "%d,%d", x, y) != 2 ) return NULL;

    if( parsed_len ) *parsed_len = ( int )(ptr - str + 1);
    return ptr + 1;
}

// Check if character is valid hexadecimal
static int is_hex_char( char c )
{
    return (c >= '0' && c <= '9') ||
        (c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F');
}

// Try to parse #FGC:RRGGBB; or #BGC:RRGGBB; color command
// Converts RGB hex to ANSI 24-bit VT100 escape codes
static const char* try_parse_color( const char* str, int* parsed_len )
{
    if( strncmp(str, SET_FOREGROUND_COLOR, 5) == 0 || strncmp(str, SET_BACKGROUND_COLOR, 5) == 0 )
    {
        int is_bg = (str[1] == 'B');
        const char* hex = str + 5;
        for( int i = 0; i < 6; ++i )
        {
            if( !is_hex_char(hex[i]) ) return NULL;
        }
        if( hex[6] != ';' ) return NULL;

        char buf[7];
        strncpy(buf, hex, 6);
        buf[6] = '\0';

        int r, g, b;
        if( sscanf(buf, "%02x%02x%02x", &r, &g, &b) != 3 ) return NULL;

        if( is_bg )
            hal_printf("\033[48;2;%d;%d;%dm", r, g, b); // Set background color
        else
            hal_printf("\033[38;2;%d;%d;%dm", r, g, b); // Set foreground color

        if( parsed_len ) *parsed_len = 12;
        return str + 12;
    }

    return NULL;
}

// Validate if command after '#' is 3 alphanumeric characters followed by ';'
static int is_valid_cmd( const char* cmd )
{
    for( int i = 0; i < 3; ++i )
    {
        if( !isalnum(cmd[i]) ) return 0;
    }
    return cmd[3] == ';' || cmd[3] == '\0';
}

// Try to parse control commands like #RST;, #CLS, etc.
// Emit corresponding VT100 escape codes
static const char* try_parse_control( const char* str, int* parsed_len )
{
    if( *str != '#' ) return NULL;

    if( strncmp(str, RESET_FORMATTING, 5) == 0 )
    {
        hal_printf("\033[0m");
        if( parsed_len ) *parsed_len = 5;
        return str + 5;
    }
    if( strncmp(str, CLEAR_SCREEN, 5) == 0 )
    {
        hal_printf("\033[2J\033[H");
        if( parsed_len ) *parsed_len = 5;
        return str + 5;
    }
    if( strncmp(str, CLEAR_TO_END_OF_LINE, 5) == 0 )
    {
        hal_printf("\033[K");
        if( parsed_len ) *parsed_len = 5;
        return str + 5;
    }
    if( strncmp(str, CLEAR_ROW, 5) == 0 )
    {
        hal_printf("\033[2K");
        if( parsed_len ) *parsed_len = 5;
        return str + 5;
    }
    if( strncmp(str, TEXT_BOLD, 5) == 0 )
    {
        hal_printf("\033[1m");
        if( parsed_len ) *parsed_len = 5;
        return str + 5;
    }
    if( strncmp(str, TEXT_UNDERLINE, 5) == 0 )
    {
        hal_printf("\033[4m");
        if( parsed_len ) *parsed_len = 5;
        return str + 5;
    }
    if( strncmp(str, TEXT_INVERSE, 5) == 0 )
    {
        hal_printf("\033[7m");
        if( parsed_len ) *parsed_len = 5;
        return str + 5;
    }
    if( strncmp(str, TEXT_DIM, 5) == 0 )
    {
        hal_printf("\033[2m");
        if( parsed_len ) *parsed_len = 5;
        return str + 5;
    }
    if( strncmp(str, HIDDEN_CURSOR, 5) == 0 )
    {
        hal_printf("\033[?25l");
        if( parsed_len ) *parsed_len = 5;
        return str + 5;
    }

    if( strlen(str) >= 5 && str[0] == '#' && is_valid_cmd(str + 1) )
    {
        if( parsed_len ) *parsed_len = 5;
        return str + 5;
    }

    return NULL;
}

/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */

// Extended printf function with VT100 control support
void myprintf( const char* format, ... )
{
    va_list args;
    va_start(args, format);

    char buffer[1024];
    vsnprintf(buffer, sizeof (buffer), format, args);
    va_end(args);

    const char* p = buffer;
    while( *p )
    {
        if( *p == '\\' )
        {
            // Escaped @ or # symbol
            if( *(p + 1) == '@' )
            {
                hal_putchar('@');
                p += 2;
                continue;
            }
            else if( *(p + 1) == '#' )
            {
                hal_putchar('#');
                p += 2;
                continue;
            }
        }

        // Parse cursor position @x,y;
        if( *p == CURSOR_PREFIX )
        {
            int x, y, len;
            const char* after_coord = try_parse_cursor(p, &x, &y, &len);
            if( after_coord )
            {
                hal_printf("\033[%d;%dH", y, x);
                p += len;
                continue;
            }
        }

        // Parse color command
        if( *p == '#' )
        {
            int len;
            const char* after_color = try_parse_color(p, &len);
            if( after_color )
            {
                p += len;
                continue;
            }

            const char* after_ctrl = try_parse_control(p, &len);
            if( after_ctrl )
            {
                p += len;
                continue;
            }
        }

        // Output normal character
        hal_putchar(*p++);
    }
}

/* *****************************************************************************
 End of File
 */

