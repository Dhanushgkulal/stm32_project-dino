# STM32 Game Console

A triple-game console for STM32F401 microcontroller featuring Chrome Dino Runner, Brick Breaker, and Block Blaster (Tetris-like) games with ST7735 TFT display.

## Games

### 1. Chrome Dino Runner
Classic endless runner game with jumping dinosaur avoiding obstacles.
- Touch sensor to jump
- Ambient light adaptive display (dark/light modes)
- Progressive difficulty
- High score tracking

📖 **[Detailed Dino Game Documentation](docs/DINO_GAME.md)**

### 2. Brick Breaker
Classic arcade brick breaker game with paddle and ball.
- Button controls for paddle movement
- 35 bricks to destroy
- Physics-based ball bouncing
- Score tracking

📖 **[Detailed Brick Game Documentation](docs/BRICK_GAME.md)**

### 3. Block Blaster (Tetris-like)
Falling blocks puzzle game with 7 different tetromino shapes.
- Button controls for movement and rotation
- Line clearing mechanics
- Progressive difficulty
- Score tracking (100 points per line)

📖 **[Detailed Block Game Documentation](docs/BLOCK_GAME.md)**

## Features

### Menu System
- **Game Selection**: Choose between Dino Runner, Brick Breaker, and Block Blaster
- **Visual Interface**: Clear game names with green selection highlight
- **Button Navigation**: SW1/SW3 to navigate, Touch to select

### Performance
- **Optimized Speed**: 84 MHz system clock with 10.5 MHz SPI
- **Smooth Graphics**: Batched display transfers (256-byte chunks)
- **Responsive Controls**: 16ms game tick rate (62.5 FPS)

### Audio Feedback
- **Gameplay Beeps**: Double beep pattern during play
- **Game Over**: 3 long beeps (1.5s each) with 0.25s gaps
- **Menu Sounds**: Confirmation beeps for selections

### Visual Feedback
- **RGB LED Indicators**: 
  - Yellow: Menu
  - Green: Playing
  - Blue: Jumping/Brick hit
  - Red: Game Over
- **Ambient Light Sensor**: Auto display mode (Dino game only)
- **Score Display**: Real-time score with high score tracking

## Hardware Requirements

- STM32F401RET6 microcontroller
- ST7735 128x160 TFT display
- Touch sensor
- 4 Push buttons
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

### Input Controls
- PC8: Touch sensor
- PB1: Ambient light sensor
- PB8: SW1 (Menu Up)
- PC6: SW2 (Paddle Right / Menu)
- PB9: SW3 (Menu Down)
- PC5: SW4 (Paddle Left / Menu)

### Output
- PA4: Buzzer

### RGB LED
- PC2: Green
- PC3: Red
- PC0: Blue

### Additional LEDs
- PC1: LED4
- PB0: LED3
- PB10: LED2
- PA1: LED1

## Controls

### Menu
- **SW1 (PB8)**: Navigate up
- **SW3 (PB9)**: Navigate down
- **Touch (PC8)**: Select game

### Dino Runner Game
- **Touch (PC8)**: Jump
- **Ambient Light**: Auto-adjusts display brightness

### Brick Breaker Game
- **SW4 (PC5)**: Move paddle left
- **SW2 (PC6)**: Move paddle right
- **Touch (PC8)**: Return to menu (after game over)

### Block Blaster Game
- **SW4 (PC5)**: Move block left
- **SW2 (PC6)**: Move block right
- **SW1 (PB8)**: Rotate block
- **Touch (PC8)**: Return to menu (after game over)

## Game Modes

### Menu
- Fixed dark mode with white text
- Green highlight for selected game
- Yellow LED indicator
- 3 games to choose from

### Dino Runner (Dark/Light Mode)
**Dark Mode (Low Light)**
- Black background
- White characters and obstacles

**Light Mode (Bright Light)**
- White background
- Black characters and obstacles

### Brick Breaker
- Fixed dark mode
- Yellow bricks, blue paddle, red ball
- 5 rows × 7 columns (35 bricks total)

### Block Blaster
- Fixed dark mode
- 7 different colored tetromino shapes
- 10×20 grid playfield
- Line clearing when row is complete

## Performance Optimizations

1. **84 MHz System Clock**: 5.25x faster than default 16 MHz
2. **10.5 MHz SPI Speed**: 10.5x faster than original 1 MHz
3. **Batched Display Transfers**: 256-byte chunks instead of per-pixel
4. **Optimized Sprite Rendering**: One HAL call per scanline
5. **Efficient Game Loop**: 16ms tick rate for smooth 62.5 FPS

## Building and Flashing

### Quick Start

1. Open the project in STM32CubeIDE
2. Build the project (Ctrl+B)
3. Connect your STM32F401 board via ST-Link
4. Flash and run (Ctrl+F11)

📖 **[Complete Build and Flash Guide](docs/BUILD.md)**

The build guide includes:
- Detailed hardware setup and wiring diagrams
- Software installation instructions
- Step-by-step build process
- Multiple flashing methods
- Troubleshooting common issues
- Advanced configuration options

## Gameplay

### Dino Runner
- Touch PC8 to make the dino jump
- Avoid the cacti obstacles
- Score increases as you progress
- Game speed increases over time
- Display colors adapt to ambient lighting

### Brick Breaker
- Use SW4/SW2 to move paddle left/right
- Break all bricks to win
- Don't let the ball fall
- Each brick is worth 10 points
- Maximum score: 350 points

### Block Blaster
- Use SW4/SW2 to move blocks left/right
- Use SW1 to rotate blocks
- Complete horizontal lines to clear them
- Each cleared line is worth 100 points
- Game over when blocks reach the top

## Audio Cues

### During Gameplay
- **Dino**: Double beep pattern with 0.7s gap
- **Brick**: Beeps on wall/paddle/brick hits

### Game Over
- 3 long beeps (1.5s each) with 0.25s gaps

## Project Structure

```
stm32_project-dino/
├── Core/
│   ├── Inc/           # Header files
│   └── Src/
│       └── main.c     # Main game code
├── Drivers/           # STM32 HAL drivers
├── docs/
│   └── BRICK_GAME.md  # Brick Breaker documentation
└── README.md          # This file
```

## Documentation

Detailed game guides with complete technical documentation:

- **[Chrome Dino Runner Guide](docs/DINO_GAME.md)** - Complete guide for the endless runner game
- **[Brick Breaker Guide](docs/BRICK_GAME.md)** - Detailed documentation for the brick breaker game
- **[Block Blaster Guide](docs/BLOCK_GAME.md)** - Complete guide for the Tetris-like puzzle game

Each guide includes:
- Gameplay instructions and controls
- Technical specifications and constants
- Code structure and algorithms
- Strategy tips and troubleshooting
- Future enhancement ideas

## Technical Details

### Memory Usage
- Dino game variables: ~200 bytes
- Brick game variables: ~100 bytes
- Block game variables: ~250 bytes
- Display buffer: 256 bytes (shared)
- Sprite data: ~400 bytes (ROM)

### Display Resolution
- 128×160 pixels (ST7735)
- 16-bit color (RGB565)

### Game Constants
- Dino tick rate: 16ms
- Brick tick rate: 16ms
- Block tick rate: 16ms
- Menu update: 50ms

## Troubleshooting

### Display Issues
- Check SPI connections (PA5, PA7)
- Verify control pins (PC7, PB6, PA9)
- Ensure 84 MHz clock configuration

### Control Issues
- Verify button connections (PB8, PC6, PB9, PC5)
- Check touch sensor on PC8
- Test ambient light sensor on PB1

### Audio Issues
- Check buzzer connection on PA4
- Verify GPIO output configuration

### LED Issues
- Check RGB LED connections (PC0, PC2, PC3)
- Verify GPIO initialization

## Future Enhancements

Possible improvements:

1. **More Games**: Snake, Pong, Space Invaders
2. **High Score Persistence**: EEPROM/Flash storage
3. **Sound Effects**: Different tones for events
4. **Multiplayer**: Two-player modes
5. **Difficulty Levels**: Easy, Medium, Hard
6. **Achievements**: Unlock system
7. **Custom Themes**: User-selectable color schemes
8. **Block Game Enhancements**: Hold piece, ghost piece, hard drop

## License

This project is open source and available for educational purposes.

## Author

Dhanush G Kulal

## Acknowledgments

- STM32 HAL Library
- ST7735 Display Driver
- Chrome Dino Game (inspiration)
