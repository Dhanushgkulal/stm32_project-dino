# Brick Breaker Game

A classic brick breaker game implementation for STM32F401 with ST7735 TFT display.

## Game Overview

Break all the bricks by bouncing the ball with your paddle. Don't let the ball fall!

## Features

### Gameplay
- **5 rows x 7 columns** of bricks (35 total bricks)
- **Physics-based ball movement** with realistic bouncing
- **Paddle control** using push buttons
- **Score tracking** with high score persistence
- **Progressive difficulty** - ball speed increases as you play

### Visual Feedback
- **Colorful display**:
  - Yellow bricks
  - Blue paddle
  - Red ball
  - Black background
- **Real-time score display** at the top of screen
- **Game over screen** showing current score and high score

### Audio Feedback
- **Wall bounce**: Short beep
- **Paddle hit**: Medium beep
- **Brick break**: Long beep with LED flash
- **Game over**: 3 long beeps (1.5s each) with 0.25s gaps

### LED Indicators
- **Green**: Playing
- **Blue**: Brick destroyed (flash)
- **Red**: Game over

## Controls

### During Gameplay
- **SW4 (PC5)**: Move paddle LEFT
- **SW2 (PC6)**: Move paddle RIGHT
- **Touch Sensor (PC8)**: Return to menu (after game over)

### Game Over
- Touch the sensor to return to the main menu

## Pin Configuration

### Input Controls
- **PC5**: SW4 - Paddle Left
- **PC6**: SW2 - Paddle Right
- **PC8**: Touch sensor

### Output
- **PA4**: Buzzer
- **PC0**: Blue LED
- **PC2**: Green LED
- **PC3**: Red LED

### Display (ST7735)
- **PA5**: SPI SCK
- **PA7**: SPI MOSI
- **PC7**: RST
- **PB6**: CS
- **PA9**: DC

## Game Constants

```c
#define PADDLE_W      20    // Paddle width
#define PADDLE_H      4     // Paddle height
#define PADDLE_Y      (TFT_HEIGHT - 10)  // Paddle Y position

#define BALL_SIZE     3     // Ball size (3x3 pixels)

#define BRICK_W       16    // Brick width
#define BRICK_H       6     // Brick height
#define BRICK_ROWS    5     // Number of brick rows
#define BRICK_COLS    7     // Number of brick columns
#define BRICK_START_Y 20    // Starting Y position for bricks
```

## Scoring System

- **10 points** per brick destroyed
- **Maximum score**: 350 points (35 bricks × 10 points)
- High score is saved across game sessions

## Collision Detection

### Ball Collisions
1. **Wall collision**: Ball bounces off left, right, and top walls
2. **Paddle collision**: Ball bounces when hitting the paddle
3. **Brick collision**: Brick is destroyed, ball bounces, score increases
4. **Bottom collision**: Game over if ball falls below paddle

### Collision Physics
- Ball velocity reverses on wall/paddle/brick hit
- Collision detection uses bounding box algorithm
- Ball position is adjusted to prevent getting stuck

## Game States

### Playing
- Paddle moves left/right with buttons
- Ball bounces around the screen
- Bricks disappear when hit
- Score increases

### Game Over Conditions
1. **Ball falls**: Ball goes below the paddle
2. **All bricks cleared**: Win condition (all 35 bricks destroyed)

### Game Over Screen
- Displays current score
- Displays high score
- Plays game over sound sequence
- LED turns red
- Touch sensor to return to menu

## Color Scheme

The brick game uses a fixed dark color scheme (no ambient light adaptation):

```c
Background:  Black (0x0000)
Paddle:      Blue (0x001F)
Ball:        Red (0xF800)
Bricks:      Yellow (0xFFE0)
Score:       Gray (0x8410)
```

## Performance Optimizations

1. **Efficient rendering**: Only redraws changed elements
2. **Batched SPI transfers**: Uses 256-byte buffer for display updates
3. **84 MHz clock**: Fast MCU performance
4. **10.5 MHz SPI**: High-speed display communication

## Tips for Playing

1. **Keep the ball in play**: Position paddle under the ball
2. **Aim for corners**: Hit bricks from angles to clear rows faster
3. **Watch the speed**: Ball moves faster as game progresses
4. **Use full paddle**: Ball can bounce from any part of the paddle

## Code Structure

### Initialization
```c
static void Brick_Init(void)
```
- Sets up paddle position
- Initializes ball position and velocity
- Creates brick array
- Resets score
- Initializes audio/LED

### Game Loop
```c
static void Brick_Tick(void)
```
- Reads paddle controls
- Updates ball physics
- Checks collisions
- Updates display
- Handles scoring

### Game Over
```c
static void Brick_GameOver(void)
```
- Displays final score
- Plays game over sequence
- Waits for touch to return to menu

## Future Enhancements

Possible improvements for future versions:

1. **Power-ups**: Extra balls, wider paddle, slower ball
2. **Multiple levels**: Different brick patterns
3. **Brick types**: Different colors with different point values
4. **Lives system**: Multiple chances before game over
5. **Ball speed control**: Gradual speed increase
6. **Particle effects**: Visual effects when bricks break
7. **Sound effects**: Different sounds for different events

## Troubleshooting

### Paddle not moving
- Check SW2 (PC6) and SW4 (PC5) connections
- Verify GPIO initialization in `MX_GPIO_Init()`

### Ball not bouncing correctly
- Check collision detection logic
- Verify ball velocity calculations

### Bricks not disappearing
- Check brick array initialization
- Verify collision detection with bricks

### No sound
- Check buzzer connection on PA4
- Verify `BUZZER_ON()` and `BUZZER_OFF()` macros

## Technical Details

### Memory Usage
- Brick array: 35 bytes (5 rows × 7 cols × 1 byte)
- Game variables: ~50 bytes
- Display buffer: 256 bytes (shared with other games)

### Timing
- Game tick rate: 16ms (62.5 FPS)
- Smooth animation and responsive controls

### Display Updates
- Paddle: Redrawn on position change
- Ball: Redrawn every frame
- Bricks: Drawn once, erased when destroyed
- Score: Updated when bricks are destroyed

## Credits

Developed for STM32F401RET6 microcontroller as part of a dual-game console project.

## License

This project is open source and available for educational purposes.
