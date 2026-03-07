# STM32 Chrome Dino Game

A Chrome Dino runner game implementation for STM32F401 microcontroller with ST7735 TFT display.

## Features

- **Optimized Performance**: Running at 84 MHz with 10.5 MHz SPI for smooth gameplay
- **Touch Control**: Touch sensor on PC8 for jumping
- **Audio Feedback**: Buzzer with gameplay beeps and game over sounds
- **RGB LED Indicators**: 
  - Green: Playing
  - Blue: Jumping
  - Red: Game Over
- **Ambient Light Sensor**: Automatically switches between dark and light display modes
- **Score Tracking**: High score persistence across games

## Hardware Requirements

- STM32F401RET6 microcontroller
- ST7735 128x160 TFT display
- Touch sensor
- Buzzer
- RGB LED
- Ambient light sensor

## Pin Configuration

### Display (ST7735)
- PA5: SPI SCK
- PA7: SPI MOSI
- PC7: RST
- PB6: CS
- PA9: DC

### Input/Output
- PC8: Touch sensor (input)
- PB1: Ambient light sensor (input)
- PA4: Buzzer (output)

### RGB LED
- PC2: Green
- PC3: Red
- PC0: Blue

### Additional LEDs
- PC1: LED4
- PB0: LED3
- PB10: LED2
- PA1: LED1

## Performance Optimizations

1. **84 MHz System Clock**: 5.25x faster than default 16 MHz
2. **10.5 MHz SPI Speed**: 10.5x faster than original 1 MHz
3. **Batched Display Transfers**: 256-byte chunks instead of per-pixel
4. **Optimized Sprite Rendering**: One HAL call per scanline

## Building and Flashing

1. Open the project in STM32CubeIDE
2. Build the project (Ctrl+B)
3. Connect your STM32F401 board via ST-Link
4. Flash and run (Ctrl+F11)

## Gameplay

- Touch PC8 to make the dino jump
- Avoid the cacti obstacles
- Score increases as you progress
- Game speed increases over time
- Display colors adapt to ambient lighting

## Game Modes

### Dark Mode (Low Light)
- Black background
- White characters and obstacles

### Light Mode (Bright Light)
- White background
- Black characters and obstacles

## Audio Cues

### During Gameplay
- Double beep pattern with 0.7s gap

### Game Over
- 3 long beeps (1.5s each) with 0.25s gaps

## License

This project is open source and available for educational purposes.

## Author

Dhanush G Kulal
