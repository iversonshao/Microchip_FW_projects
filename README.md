# Lab00 - ARM Cortex M4F PIC32CX1025SG41128 Training

## What We Do
Set up MPLAB X IDE and create a basic project for PIC32CX1025SG41128.

## Steps
1. Create new project in MPLAB X IDE
2. Select PIC32CX1025SG41128 device
3. Configure XC32 compiler
4. Use MCC Harmony to generate code
5. Build and program to board

## Project Name
Lab00_NewProject_MCC

## Result
Working development environment ready for peripheral labs.

---

# Lab01 - GPIO Input/Output Control

## What We Do
Learn GPIO (PORT) configuration using MCC Harmony and control LEDs with button input.

## Lab Requirements
- Try to control LED1(PA12) and LED2(PA13) continues toggle (period around 500ms ~ 1S)
- Try to add BT1(PD00) and BT2(PD01), and get BT1 status then use to control LED3(PA15)
- Pressed -> LED3 ON
- Released -> LED3 OFF

## Objectives
- Configure GPIO pins for input/output using MCC Pin Configuration
- Control LED outputs with Toggle, Set, Clear functions
- Read button input status
- Implement state machine for better responsiveness

## Hardware Connections
| Component | Pin | Direction |
|-----------|-----|-----------|
| BT1 | PD00 | Input |
| BT2 | PD01 | Input |
| LED1 | PA12 | Output |
| LED2 | PA13 | Output |
| LED3 | PA15 | Output |

## Steps
1. Open MCC Harmony Pin Configuration
2. Enable GPIO for required pins (PD00, PD01, PA12, PA13, PA15)
3. Set pin directions and custom names
4. Generate code
5. Implement LED toggle and button control logic
6. Build and program to board

## Key Features
- **LED1 & LED2**: Continuous toggle every ~500ms-1s
- **LED3**: Controlled by BT1 button press
- **State Machine**: Non-blocking delay implementation for better button response

## Project Name
Lab01_Input_Output_PORT

## Code Tasks
- TODO 1.01: Declare counter variable
- TODO 1.02: Implement LED toggle with state machine
- TODO 1.03: Button control for LED3

## Result
LED1 and LED2 flash continuously while LED3 responds immediately to button press.

---

# Lab02 - Timer Polling TC

## What We Do
Learn to use Timer/Counter (TC) module with polling method to control LED timing instead of software delays.

## Lab Requirements
- Setup TC0 to Timer and Polling method (MPWM 16-bit mode), Timer period to 500ms, LED1 will flash at 500ms ON/OFF
- Setup TC1 to Timer and Polling method (MPWM 16-bit mode), Timer period to 40ms, LED2 will flash at 40ms ON/OFF
- Replace software delay with hardware timer for precise timing control

## Objectives
- Configure TC modules using MCC Harmony
- Understand timer prescaler and period calculations
- Implement polling method for timer overflow detection
- Replace blocking software delays with non-blocking hardware timers

## Hardware Connections
| Component | Pin | Timer | Period |
|-----------|-----|-------|--------|
| LED1 | PA12 | TC0 | 500ms |
| LED2 | PA13 | TC1 | 40ms |
| LED3 | PA15 | - | Button controlled |

## Timer Configuration
### TC0 Settings
- **Counter Mode**: Counter in 16-bit mode
- **Operating Mode**: Timer (MPWM)
- **Prescaler**: GCLK_TC/1024
- **Timer Period**: 500ms (29,296 counts)

### TC1 Settings
- **Counter Mode**: Counter in 16-bit mode  
- **Operating Mode**: Timer (MPWM)
- **Prescaler**: GCLK_TC/256
- **Timer Period**: 40ms (9,375 counts)

## Steps
1. Find TC0 and TC1 in MCC Device Resources
2. Add both TC modules to Project Resources
3. Configure TC0 for 500ms period with 1024 prescaler
4. Configure TC1 for 40ms period with 256 prescaler
5. Disable Timer Period Interrupt for both timers
6. Generate code
7. Update main.c with timer functions
8. Build and program to board

## Key Features
- **Hardware Timers**: Precise timing without blocking CPU
- **Polling Method**: Check timer overflow flags in main loop
- **Independent Timing**: Multiple timers running simultaneously
- **MPWM Mode**: Match Pulse-Width Modulation for accurate periods

## Project Name
Lab02_Timer_Polling_TC

## Code Tasks
- TODO 2.01: Comment out software delay from Lab01
- TODO 2.02: Start TC0 and TC1 timers
- TODO 2.03: Implement timer polling for LED control

## Key Functions
- `TC0_TimerStart()` / `TC1_TimerStart()`: Enable timer counters
- `TC0_TimerPeriodHasExpired()` / `TC1_TimerPeriodHasExpired()`: Check timer overflow flags
- Timer calculation: `Timer Count = (Period × Clock_After_Prescaler)`

## Result
- LED1 regularly turns on and off every 1s (500ms ON/OFF)
- LED2 regularly turns on and off every 80ms (40ms ON/OFF)
- LED3 still controlled by BT1 button with immediate response