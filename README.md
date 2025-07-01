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

---

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

---

# Lab08 - ADC Polling Software Trigger

## What We Do
Learn basic ADC (Analog-to-Digital Converter) operation using polling method to read potentiometer values.

## Lab Requirements
- Configure ADC0 for single-ended mode with 12-bit resolution
- Read VR1 potentiometer value (ADC0 AIN[7]) every 500ms
- Use software trigger and polling method
- Display ADC result via UART terminal

## Hardware Mapping
| Component | Pin | ADC Channel | Purpose |
|-----------|-----|-------------|---------|
| Potentiometer (VR1) | PA07 | ADC0 CH.7 | Variable voltage input |

## Key Configuration
- **ADC Mode**: Single-ended
- **Resolution**: 12-bit (0-4095)
- **Reference**: AVDD (3.3V)
- **Trigger**: Software
- **Prescaler**: Peripheral clock divided by 8
- **Sample Length**: 4 cycles

## Key Changes
- Add ADC0 module in MCC Harmony Device Resources
- Configure positive input as ADC AIN7 Pin (PA07) for potentiometer
- Enable ADC0 and use polling method to read conversion results
- Use software trigger with 12-bit resolution

## Project Name
Lab08_VR1_Single_Polling_ADC

## Result
Terminal displays VR1 potentiometer value (0-4095) updating every 500ms while LEDs continue operation.

---

# Lab09 - Dual ADC Core Polling Software Trigger

## What We Do
Learn to use dual ADC cores simultaneously to read multiple sensors using polling method.

## Lab Requirements
- Use ADC0 for potentiometer and ADC1 for temperature/light sensors
- Read three analog inputs: VR1, temperature sensor, and light sensor
- Configure channel switching for ADC1 to read multiple inputs
- Display all sensor values via UART terminal

## Hardware Mapping
| Component | Pin | ADC Channel | Purpose |
|-----------|-----|-------------|---------|
| Potentiometer (VR1) | PA07 | ADC0 CH.7 | Variable voltage input |
| Temperature Sensor (MCP9700A) | PC00 | ADC1 CH.10 | Temperature measurement |
| Light Sensor (TEMT6000X01) | PC01 | ADC1 CH.11 | Light intensity measurement |

## Key Features
- **Dual Core Operation**: ADC0 and ADC1 work independently
- **Channel Switching**: ADC1 alternates between channels 10 and 11
- **Multi-sensor Reading**: Three different analog inputs
- **Separate Sample & Hold**: Each ADC has dedicated circuits

## Key Changes
- Add ADC1 module alongside existing ADC0 configuration
- Configure ADC1 for temperature sensor (AIN10) and light sensor (AIN11)
- Implement channel switching using `ADC1_ChannelSelect()` function
- Read multiple sensors sequentially in same timer period

## Project Name
Lab09_TempSens_LightSens_DualCore_Multi_Polling_ADC

## Result
Terminal displays three sensor values:
- VR1 potentiometer value (every 500ms)
- Temperature sensor value (every 500ms) 
- Light sensor value (every 500ms)

---

# Lab10 - Dual ADC Core Interrupt Software Trigger

## What We Do
Convert from polling method to interrupt-driven ADC operation for better efficiency and non-blocking sensor reading.

## Lab Requirements
- Convert ADC0 and ADC1 from polling to interrupt mode
- Use callback functions for ADC conversion complete events
- Implement automatic channel switching in ADC1 callback
- Maintain same sensor reading functionality with improved efficiency

## Key Features
- **Interrupt-driven**: No CPU blocking during ADC conversion
- **Callback Functions**: Handle conversion complete events
- **Automatic Channel Switching**: ADC1 alternates channels in callback
- **Flag-based Communication**: Between ISR and main loop
- **Non-blocking Operation**: CPU can perform other tasks during conversion

## Key Changes
- Enable "Result Ready Interrupt" for both ADC0 and ADC1 in MCC configuration
- Replace polling method with callback functions for ADC completion events
- Use `volatile` flags to communicate between ISR and main loop
- Implement automatic channel switching in ADC1 callback function

## Advanced Features
- **Automatic Channel Switching**: ADC1 cycles between temperature and light sensors
- **Optimized Timing**: All sensors can be read every 500ms without blocking
- **Enhanced Efficiency**: CPU available for other tasks during conversion

## Project Name
Lab10_DualCore_Interrupt_ADC

## Result
Same sensor display as Lab09 but with:
- Non-blocking ADC operation
- Automatic channel cycling for ADC1
- Better system responsiveness
- All three sensor values updated every 500ms

## ADC Technical Summary

### ADC Specifications (PIC32CX-SG)
- **Resolution**: Up to 12-bit (4096 levels)
- **Sampling Rate**: Up to 1Msps
- **Dual Cores**: ADC0 and ADC1 with independent circuits
- **Input Channels**: Up to 23 analog inputs per core
- **Reference Options**: Internal bandgap, AVDD, external references
- **Conversion Modes**: Single-ended and differential
- **Trigger Sources**: Software, free-run, hardware event

### Performance Calculations
- **Maximum Throughput**: 1Msps = 16MHz / (4+12) TAD
- **Minimum Sample Time**: 4 TAD (for 12-bit, typical source impedance)
- **Conversion Time**: 12 TAD (for 12-bit resolution)
- **ADC Clock**: Maximum 16MHz input clock

---

# Lab11 - LED Dimming TCC PWM

## What We Do
Learn TCC PWM operation by implementing LED brightness control using Normal Pulse-Width Modulation (NPWM).

## Lab Requirements
- Configure TCC1 in PWM mode with 1kHz frequency and 20% duty cycle
- Use TCC1 to control LED3(PA15) brightness instead of GPIO
- PWM period: 59,999 counts (1ms), duty: 12,000 counts (200μs)
- Disable LED3 GPIO function and enable TCC1_WO3 output

## Hardware Mapping
| Component | Pin | TCC Channel | Purpose |
|-----------|-----|-------------|---------|
| LED3 | PA15 | TCC1 WO3 | PWM-controlled LED dimming |

## Key Configuration
- **TCC1 Mode**: PWM (NPWM - Normal Pulse-Width Modulation)
- **PWM Frequency**: 1kHz (60MHz/60,000 = 1kHz)
- **Duty Cycle**: 20% (12,000/60,000)
- **Period Value**: 59,999 (60,000 counts total)
- **Channel**: TCC1 Channel 3 connected to PA15

## Key Changes
- Add TCC1 module in MCC Harmony Device Resources
- Configure TCC1 for PWM mode with NPWM type
- Set period value to 59,999 and duty value to 12,000 for Channel 3
- Disable LED3 GPIO function and enable TCC1_WO3 pin mapping
- Start TCC1 PWM and comment out button-controlled LED3 function

## Project Name
Lab11_LED_Dimming_TCC_PWM

## Result
LED3 displays constant dimmed brightness (20% duty cycle) controlled by TCC1 PWM while LED1/LED2 continue timer-based blinking.

---

# Lab12 - LED Dimming Adjustable TCC PWM

## What We Do
Implement variable LED brightness control by using ADC potentiometer value to dynamically adjust TCC1 PWM duty cycle.

## Lab Requirements
- Use VR1 potentiometer (ADC0) to control TCC1 PWM duty cycle
- Map ADC range (0-4095) to PWM duty range (0-100%)
- Display mathematical comparison of different calculation methods
- Real-time adjustment of LED3 brightness based on VR1 position

## Hardware Mapping
| Component | Pin | Function | Purpose |
|-----------|-----|----------|---------|
| VR1 Potentiometer | PA07 | ADC0 CH.7 | PWM duty control input |
| LED3 | PA15 | TCC1 WO3 | Variable brightness output |

## Key Algorithm
```c
// Convert ADC value (0-4095) to PWM duty cycle
PWM_Duty = (ADC_Value * PWM_Period) / 4095
// Using 32-bit arithmetic to prevent overflow
TCC1_PWM24bitDutySet(TCC1_CHANNEL3, ((uint32_t)ADC0_Result * (TCC1_PWM24bitPeriodGet() + 1)) / 4095);