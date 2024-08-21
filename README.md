# Keyboard-DIY
## Make a keyboard yourself at home using ARM-Cortex-M4 micro-controller

![example](https://github.com/KareimGazer/Keyboard-DIY/blob/main/assets/terminal.PNG?raw=true)

# Hardware Config
## Tools used

## Schematic

# Software config

# Arch
## Features used
- **Light Weight:** The microcontroller can perform othe functions and not only focusing on reading keyboad.
- **Buffered typing:** user input is stored and sent so info is not lost.
- **keys rollover SOLVED:** no info is lost when users type fast and multiple keys are pressed.
- **switch bouncing SOLVED:** every key stroke is calculated not more not less.

## Features Implementation
- **Buffered typing:** UART0 Interrupt with Software and Hardware FIFOs.
- **Light Weight & keys rollover SOLVED:** SysTick Interrupt.
- **switch bouncing SOLVED:** Edge triggered Interrupt at Port B.

## Idea

# Important
