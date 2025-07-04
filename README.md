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
```

## Mathematical Comparison
The lab demonstrates different calculation methods:
- **Division before Multiplication**: Causes precision loss
- **16-bit Integer Overflow**: Results in incorrect values
- **32-bit Integer Calculation**: Provides accurate results

## Project Name
Lab12_LED_Dimming_Adjustable_TCC_PWM

## Result
LED3 brightness is controlled by the ADC value from VR1 potentiometer, with mathematical comparison results displayed on Tera Term terminal.

---

# Lab13 - LED Breath TCC PWM

## What We Do
Use TC1 timer to control PWM duty cycle for creating a breathing light effect on LED3.

## Lab Requirements
- Duty cycle changes in steps of 2% every 40ms intervals
- Create breathing pattern: Duty 0% → Duty 100% → Duty 0% → repeat
- Use TC1 timer interrupt to control the breathing algorithm
- Maintain other lab functionalities (ADC, UART, etc.)

## Key Algorithm
```c
// Breathing light variables
uint8_t Duty = 50;
int8_t DutyDistance = 2;

// In TC1 timer interrupt
Duty += DutyDistance;
if (Duty >= 100 || Duty <= 0)
    DutyDistance = -DutyDistance;

TCC1_PWM24bitDutySet(TCC1_CHANNEL3, ((uint32_t)Duty * (TCC1_PWM24bitPeriodGet() + 1)) / 100);
```

## Breathing Pattern Timing
- **Timer Period**: TC1 = 40ms
- **Duty Step**: ±2% per timer interrupt
- **Full Cycle Time**: ~4 seconds (0% → 100% → 0%)
- **Smooth Transition**: Creates natural breathing effect

## Key Changes
- Define breathing control variables (Duty, DutyDistance)
- Implement breathing algorithm in TC1 timer callback
- Comment out ADC-controlled dimming from Lab12
- Maintain all other peripheral functions

## Project Name
Lab13_LED_Breath_TCC_PWM

## Result
LED3 brightness is automatically controlled with a breathing pattern. The PWM duty cycle changes in steps of 2% at 40ms intervals, creating a smooth breathing light effect while maintaining all other system functions.

# Lab14 - OLED Graphics SERCOM-SPI

## What We Do
Learn SPI communication by implementing SERCOM-SPI interface to control an SSD1306 OLED display with graphics library and custom logo functionality.

## Lab Requirements
- Configure SERCOM5 as SPI Master with 1MHz clock speed
- Control SSD1306 128x64 monochrome OLED display via SPI
- Implement multi-layer graphics system (LAYER_GRAPHIC, LAYER_TEXT)
- Display splash logo animation and real-time peripheral data
- Use TC2 timer for software delay functions
- Integrate all previous lab functionalities with OLED display

## Hardware Mapping
| Component | Pin | Function | Purpose |
|-----------|-----|----------|---------|
| OLED MOSI | PB02 | SERCOM5_PAD0 | SPI Data Output |
| OLED SCK | PB03 | SERCOM5_PAD1 | SPI Clock |
| OLED CS | PB00 | GPIO Output | SPI Chip Select |
| OLED A0 | PB01 | GPIO Output | Data/Command Control |
| OLED Reset | PB31 | GPIO Output | Hardware Reset |

## Key Configuration
- **SERCOM5**: SPI Master mode, Transfer Mode 0
- **SPI Speed**: 1MHz (1,000,000 Hz)
- **Display**: SSD1306 128x64 OLED, 1-bit per pixel
- **Graphics Layers**: LAYER_GRAPHIC and LAYER_TEXT
- **Software Delay**: TC2-based timer with 1ms tick

## Key Features
- **Splash Animation**: Microchip logo sliding in/out animation
- **Multi-layer Display**: Separate graphics and text layers
- **Real-time Data**: System clock, timer periods, ADC values, UART data
- **Graphics Library**: Complete drawing functions (points, lines, shapes, text)
- **Printf-like Functions**: GPL_Printf with positioning and formatting

## Key Changes
- Add SERCOM5 SPI Master configuration in MCC Harmony
- Configure GPIO pins for OLED control signals (CS, A0, Reset)
- Add TC2 timer for software delay implementation
- Include graphics library files (LCM.c/.h, GraphicLib.c/.h, GraphicApp.c/.h)
- Implement splash animation and real-time data display
- Integrate OLED display with all existing peripheral functions

## Project Name
Lab14_OLED_Graphic_SERCOM_SPI

## Result
OLED display shows animated Microchip logo splash screen on startup, followed by real-time system information including 48MHz clock frequency, timer periods (TC0: 500ms, TC1: 40ms), PWM duty cycle with breathing pattern, ADC sensor values (VR1, temperature, light sensor), and UART received characters. All data updates in real-time while maintaining LED operations and existing functionality.

---

# Lab15 - Design Your Own Logo

## What We Do
Learn to create custom graphics using OLED Assistant software and integrate custom logos into the OLED display project for personalized splash screens.

## Lab Requirements
- Use OLED Assistant tool to design custom graphics on PC
- Create bitmap images with 128x64 monochrome format
- Export custom designs as C header files
- Replace default Microchip logo with custom design
- Support text input, icon placement, and image manipulation
- Implement custom logo animation sequences

## Key Features
### OLED Assistant Tool Functions
- **Drawing Tools**: Pen/eraser with adjustable sizes for pixel-level editing
- **Shape Tools**: Lines, rectangles, circles, polygons for geometric designs
- **Text Input**: Multiple system fonts with adjustable size control
- **Icon Library**: Built-in symbols and Unicode characters
- **Image Import**: Load external BMP/PNG files for editing
- **Image Manipulation**: Zoom, flip horizontal/vertical, invert colors
- **Region Operations**: Copy/paste selected areas, region-based editing
- **Export Function**: Generate C header files with bitmap arrays

### Advanced Features
- **Multi-frame Animation**: Create sequences for wave effects and transitions
- **Custom Font Support**: Access to Windows system fonts
- **Preview Mode**: Real-time OLED simulation with accurate pixel representation
- **Batch Export**: Multiple image arrays in single header file
- **Professional Tools**: Undo/redo, keyboard shortcuts, precise positioning

## Design Process
1. **Initialize**: Open OLED Assistant and set 128x64 canvas
2. **Create Graphics**: Use drawing tools or import existing images
3. **Add Text**: Select regions and input custom text with various fonts
4. **Apply Effects**: Use flip, invert, zoom operations for refinement
5. **Export**: Generate C header file with properly formatted bitmap data
6. **Integration**: Replace Microchip_Logo.h in project source folder
7. **Program**: Build and upload to see custom logo in action

## Key Operations
- **Left Click + Drag**: Draw/paint pixels in pen mode
- **Right Click + Drag**: Select rectangular regions for operations
- **Region Selection**: Copy specific areas to clipboard for reuse
- **Icon Mode**: Place symbols using text characters and fonts
- **Image Import**: Load and convert external images to monochrome
- **Export Settings**: Configure C array names and header file format

## Key Changes
- Use OLED Assistant software to create custom graphics
- Design personalized logo or import existing company/personal branding
- Export as C header file with same format as Microchip_Logo.h
- Replace original logo file in project source directory
- Maintain same animation functions for smooth logo transitions
- Optional: Create multiple frames for advanced animation effects

## Project Name
Lab15_Custom_Logo_Design

## Result
Custom designed logo displays on OLED with personalized startup animation, replacing the default Microchip logo. The system supports professional logo design capabilities, easy integration with existing OLED graphics system, and maintains all previous lab functionality while showcasing custom branding. Advanced users can create multi-frame animations for wave effects, company logos, educational graphics, or personal artistic expressions.

## Advanced Applications
- **Corporate Branding**: Company logos on embedded product displays
- **Educational Tools**: Custom diagrams and visual teaching aids
- **Personal Projects**: Artistic expressions and creative graphics
- **Product Identification**: Custom startup screens for different product variants
- **Animation Sequences**: Professional-looking logo transitions and effects

# Lab16 - Motion Sensor Application SERCOM-I2C

## What We Do
Learn I2C communication by implementing SERCOM-I2C interface to control a 6-axis motion sensor (accelerometer + gyroscope) and create interactive motion-based applications.

## Lab Requirements
- Configure SERCOM3 as I2C Master with 100kHz clock speed
- Interface with TDK ICM-42670-P 6-axis MEMS Motion Sensor (I2C address 0x68)
- Read accelerometer data (ax, ay, az) and gyroscope data (gx, gy, gz)
- Display sensor values via UART terminal using myprintf
- Implement four interactive motion applications on OLED display
- Use BT1 to switch between applications and BT2 for zero calibration

## Hardware Mapping
| Component | Pin | I2C Function | Purpose |
|-----------|-----|--------------|---------|
| Motion Sensor SDA | PA22 | SERCOM3_PAD0 | I2C Data Line |
| Motion Sensor SCL | PA23 | SERCOM3_PAD1 | I2C Clock Line |
| TDK ICM-42670-P | - | 0x68 | 6-axis Motion Sensor |

## Key Configuration
- **SERCOM3**: I2C Master mode
- **I2C Speed**: 100kHz (Standard mode)
- **Sensor**: TDK ICM-42670-P with 3-axis accelerometer + 3-axis gyroscope
- **Communication**: Non-blocking I2C with callback functions
- **Data Ready**: Interrupt-based sensor reading using status register polling

## Motion Applications
### Application 0: Level Bubble
- Horizontal bubble level simulation
- Uses accelerometer ax, ay for tilt detection
- Shows centering bubble with smooth physics

### Application 1: Balance Ball
- Vertically reactive ball with bounce physics
- Gravity simulation based on accelerometer input
- Real-time ball movement responding to board tilt

### Application 2: Rotation Cube (2D)
- 2D rotating cube with physics simulation
- Uses accelerometer for rotation control
- Demonstrates object rotation algorithms

### Application 3: Surround Dice (3D)
- Full 3D cube with six dice faces
- Combines accelerometer and gyroscope data
- Advanced 3D rendering with Z-buffer and hidden line removal
- Shows different dice face numbers (1-6)

## Key Features
- **I2C Protocol**: Master-slave communication with motion sensor
- **Sensor Initialization**: Software reset, WHO_AM_I verification, register configuration
- **Data Ready Detection**: Interrupt status register monitoring
- **Multi-axis Reading**: Simultaneous accelerometer and gyroscope data acquisition
- **Real-time Display**: Live sensor values on UART terminal
- **Interactive Graphics**: Four different motion-responsive applications
- **3D Mathematics**: Advanced 3D rendering algorithms with rotation matrices
- **Physics Simulation**: Gravity, bounce, and inertia effects

## Technical Implementation
### ICM-42670-P Initialization Sequence
1. Software reset (register 0x02 = 0x10)
2. WHO_AM_I verification (register 0x75 should return 0x67)
3. Configure gyroscope (register 0x20 = 0x0C)
4. Configure accelerometer (register 0x21 = 0x0C)
5. Enable low noise mode (register 0x1F = 0x0F)

### Data Reading Process
1. Check data ready status (register 0x39, bit 0)
2. Read accelerometer data (registers 0x0B-0x10)
3. Read gyroscope data (registers 0x11-0x16)
4. Convert 16-bit signed values to motion parameters

### Application Control
- **BT1**: Cycle through four applications (0-3)
- **BT2**: Zero calibration / reset application state
- **TC1 Timer**: 40ms periodic data ready requests
- **I2C Callback**: Automatic sensor data processing

## Project Name
Lab16_MotionSens_SERCOM_I2C

## Result
OLED display shows four interactive motion applications that respond in real-time to board movement. Terminal displays live accelerometer and gyroscope values along with sensor initialization status. The system demonstrates advanced I2C communication, 6DOF motion sensing, 3D graphics rendering, and real-time physics simulation, creating an engaging demonstration of motion-controlled embedded applications.

## Advanced Features
- **3D Graphics Engine**: Complete Z-buffer implementation with hidden surface removal
- **Motion Physics**: Realistic gravity, bouncing, and momentum simulation
- **Multi-layer Graphics**: Separate graphics and text rendering layers
- **AI-Assisted Development**: Applications developed with AI assistance for advanced algorithms
- **Professional Motion Sensing**: Industrial-grade 6-axis MEMS sensor integration
- **Real-time Performance**: Smooth 25Hz motion updates with responsive graphics