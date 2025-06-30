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