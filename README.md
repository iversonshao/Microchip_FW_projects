# Lab00 - ARM Cortex M4F PIC32CX1025SG41128 Training

## What We Do
Set up MPLAB X IDE and create a basic project for PIC32CX1025SG41128.

## Project Name
Lab00_NewProject_MCC

## Result
Working development environment ready for peripheral labs.

---

# Lab01 - GPIO Input/Output Control

## What We Do
Learn GPIO configuration using MCC Harmony and control LEDs with button input.

## Lab Requirements
- LED1(PA12) and LED2(PA13) continuous toggle (~500ms-1s period)
- BT1(PD00) controls LED3(PA15): Pressed = ON, Released = OFF

## Project Name
Lab01_Input_Output_PORT

## Result
LED1/LED2 flash continuously while LED3 responds immediately to button press.

---

# Lab02 - Timer Polling TC

## What We Do
Replace software delays with hardware Timer/Counter (TC) modules using polling method.

## Lab Requirements
- TC0: 500ms period controlling LED1
- TC1: 40ms period controlling LED2
- Use polling method to check timer overflow flags

## Project Name
Lab02_Timer_Polling_TC

## Result
- LED1 toggles every 1s (500ms ON/OFF)
- LED2 toggles every 80ms (40ms ON/OFF)

---

# Lab03 - Timer Interrupt Callback TC

## What We Do
Convert from polling method to interrupt callback method for timer control.

## Lab Requirements
- Configure TC0 and TC1 to use interrupt callback method instead of polling
- LED1/2 toggle in interrupt callback when timer period expires
- Keep same timer periods: TC0 (500ms), TC1 (40ms)

## Key Changes
- Replace `TCn_TimerPeriodHasExpired()` polling with interrupt callbacks
- Use `volatile` flags to communicate between ISR and main loop
- Register callback functions for timer overflow events

## Project Name
Lab03_Timer_Interrupt_TC

## Result
Same LED behavior as Lab02 but using interrupt-driven approach for better efficiency.

---

# Lab04 - System Clock DFLL Close Loop

## What We Do
Improve system clock accuracy by switching from DFLL48M Open Loop to Close Loop with external 32K crystal reference.

## Lab Requirements
- Enable external XOSC32K clock source
- Configure DFLL48M Close Loop with 32K crystal as reference
- Achieve higher frequency accuracy (48MHz from 46,875 × 1KHz)

## Clock Routing Steps
1. Enable external XOSC32K (32.768kHz crystal)
2. Configure GCLK3 with XOSC32K ÷ 32 = 1KHz
3. Use GCLK3 as DFLL48M reference clock
4. Enable DFLL48M Close Loop with multiply factor 46,875

## Project Name
Lab04_SystemClock_DFLL_Closeloop

## Result
More accurate and stable 48MHz system clock with reduced temperature drift.

# Lab05 - COM Port TX by Polling - SERCOM-USART

## What We Do
Learn UART communication by implementing SERCOM-USART transmit functionality in blocking (polling) mode.

## Lab Requirements
- Configure SERCOM1 as USART with internal clock
- Use blocking mode for transmission
- Send "Hello World!" string every 500ms via TC0 timer
- Connect to PC via USB CDC bridge (PKoB4) for terminal display

## Key Components
- **SERCOM1**: Configured as USART (115200-n-8-1)
- **Pin Mapping**: PC27 (TX), PC28 (RX)
- **Operating Mode**: Blocking/Polling
- **Communication**: UART to USB CDC bridge

## Configuration Details
- Baud Rate: 115200 bps
- Data Bits: 8
- Parity: None
- Stop Bits: 1
- Flow Control: None

## Project Name
Lab05_COM_TX_Polling_SERCOM_USART

## Result
Terminal displays "Hello World!" message every 500ms while LEDs continue blinking.

---

# Lab06 - COM Port RX by Interrupt - SERCOM-USART

## What We Do
Implement UART receive functionality using interrupt-driven (non-blocking) mode for efficient character echo.

## Lab Requirements
- Switch SERCOM1-USART to non-blocking mode (interrupt)
- Receive single character from terminal
- Echo back with format: "Received Data : [character]"
- Use callback functions for TX/RX interrupt handling

## Key Features
- **Interrupt-driven**: Better efficiency than polling for receive operations
- **Character Echo**: Real-time response to terminal input
- **Callback Functions**: `USART1_Transmit()` and `USART1_Receive()`
- **Software Flags**: Communication between ISR and main loop

## Implementation Details
- Register TX/RX callback functions
- Use volatile flags for interrupt status
- Non-blocking UART operations
- Continuous character reception

## Project Name
Lab06_COM_RX_Interrupt_SERCOM_USART

## Result
Terminal echoes typed characters in format "Received Data : [char]" while maintaining LED operation.

---

# Lab07 - Custom printf with VT100 Commands - myprintf

## What We Do
Implement custom printf function with VT100 terminal control commands for advanced terminal formatting.

## Lab Requirements
- Create custom `myprintf()` function with printf-like formatting
- Implement VT100 command parser for terminal control
- Support cursor positioning, screen clearing, and text formatting
- Replace standard UART functions with enhanced terminal interface

## Key Features
### VT100 Commands Supported
- `#RST;` - Reset console formatting
- `#CLS;` - Clear screen and home cursor
- `#HID;` - Hide cursor
- `@x,y;` - Position cursor at column x, row y
- `#BOL;` - Bold text
- `#UND;` - Underline text
- `#FGC:RRGGBB;` - Set foreground color (24-bit RGB)
- `#BGC:RRGGBB;` - Set background color (24-bit RGB)

### Advanced Features
- Fixed position text output
- Color text and backgrounds
- Screen management
- Compatible with VT100/ANSI terminals

## Implementation
- Add `myprintf.c` and `myprintf.h` to project
- Custom command parser for simplified VT100 control
- Integration with existing SERCOM-USART infrastructure

## Project Name
Lab07_myprintf_SERCOM_USART

## Result
Enhanced terminal interface with:
- "Hello World!!" displayed at fixed position (1,1)
- Character echo at fixed position (1,5)
- Professional terminal formatting and control
- Support for colorful text and graphics (rainbow text, Microchip logo examples)

## Bonus Examples
- Rainbow colored text demonstration
- Microchip logo rendering using text characters
- Advanced VT100 formatting showcase