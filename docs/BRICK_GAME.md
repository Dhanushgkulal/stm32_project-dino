# Brick Breaker Game

## Overview

Brick Breaker is a classic arcade game where you control a paddle to bounce a ball and destroy all bricks. The game features physics-based ball movement, collision detection, and progressive difficulty. Clear all 35 bricks to win!

## Game Features

### Core Mechanics
- **35 Bricks**: 5 rows × 7 columns grid layout
- **Physics-Based Ball**: Realistic bouncing with velocity vectors
- **Paddle Control**: Smooth left/right movement
- **Score System**: 10 points per brick destroyed
- **Win Condition**: Clear all bricks (350 points max)
- **High Score Tracking**: Saves your best score

### Visual Features
- **Fixed Dark Mode**: Consistent color scheme
- **Color-Coded Elements**:
  - Yellow bricks (0xFFE0)
  - Blue paddle (0x001F)
  - Red ball (0xF800)
  - Black background (0x0000)
- **Real-Time Score**: Updates as bricks are destroyed
- **Game Over Screen**: Displays final and high scores

### Audio & Visual Feedback
- **Wall Bounce**: 20ms beep
- **Paddle Hit**: 30ms beep
- **Brick Break**: 50ms beep + LED flash
- **Game Over Sound**: 3 long beeps (1.5s each)
- **RGB LED Indicators**:
  - Green: Playing
  - Blue Flash: Brick destroyed
  - Red: Game Over

## Controls

| Input | Action |
|-------|--------|
| SW4 (PC5) | Move paddle left |
| SW2 (PC6) | Move paddle right |
| Touch (PC8) | Return to menu (after game over) |

## Gameplay Instructions

### Starting the Game
1. From the main menu, use SW1/SW3 to navigate to "BRICK"
2. Press the Touch sensor to start
3. Ball starts moving automatically from center

### Playing
1. **Move Paddle**: Use SW4 (left) and SW2 (right) to position paddle
2. **Bounce Ball**: Keep ball in play by hitting it with paddle
3. **Break Bricks**: Ball destroys bricks on contact
4. **Score Points**: Each brick is worth 10 points
5. **Clear All**: Destroy all 35 bricks to win

### Scoring
- **Per Brick**: 10 points
- **Total Possible**: 350 points (35 bricks)
- **High Score**: Best score saved during session

### Game Over Conditions
1. **Ball Falls**: Ball goes below paddle = Game Over
2. **All Bricks Cleared**: Win condition = Game Over (victory)

### Game Over Sequence
1. Screen flashes 3 times
2. Score display shows current and high score
3. 3 long beeps (1.5s each) with 0.25s gaps
4. LED turns red
5. Touch sensor to return to menu

## Technical Details

### Game Constants

```c
// Paddle
PADDLE_WIDTH: 20 pixels
PADDLE_HEIGHT: 4 pixels
PADDLE_Y: 150 pixels (TFT_HEIGHT - 10)
PADDLE_SPEED: 3 pixels per frame

// Ball
BALL_SIZE: 3×3 pixels
INITIAL_VX: 2 pixels per frame
INITIAL_VY: -3 pixels per frame

// Bricks
BRICK_WIDTH: 16 pixels
BRICK_HEIGHT: 6 pixels
BRICK_ROWS: 5
BRICK_COLS: 7
BRICK_START_Y: 20 pixels
BRICK_SPACING: 2 pixels (horizontal and vertical)

// Scoring
POINTS_PER_BRICK: 10
MAX_SCORE: 350

// Timing
TICK_RATE: 16ms (62.5 FPS)
```

### Physics System

**Ball Velocity**:
- X velocity (vx): Horizontal movement (-2 to +2 typical)
- Y velocity (vy): Vertical movement (-3 to +3 typical)
- Velocity reverses on collision

**Collision Detection**:
- AABB (Axis-Aligned Bounding Box) algorithm
- Checks ball against: walls, paddle, bricks
- Ball position adjusted to prevent sticking

### Brick Layout

```
Grid Layout (128×160 display):
┌────────────────────────────┐
│  Score: XXXX               │ ← HUD (0-20px)
│  ████ ████ ████ ████ ████  │ ← Row 1 (20-26px)
│  ████ ████ ████ ████ ████  │ ← Row 2 (28-34px)
│  ████ ████ ████ ████ ████  │ ← Row 3 (36-42px)
│  ████ ████ ████ ████ ████  │ ← Row 4 (44-50px)
│  ████ ████ ████ ████ ████  │ ← Row 5 (52-58px)
│                            │
│         ●                  │ ← Ball (moves)
│                            │
│        ═══════             │ ← Paddle (150px)
└────────────────────────────┘
```

**Brick Positions**:
```c
for(row = 0; row < 5; row++) {
  for(col = 0; col < 7; col++) {
    x = col * (BRICK_W + 2) + 2;
    y = BRICK_START_Y + row * (BRICK_H + 2);
  }
}
```

### Collision Detection Algorithm

**Wall Collision**:
```c
if(ball_x <= 0 || ball_x >= TFT_WIDTH - BALL_SIZE) {
  ball_vx = -ball_vx;  // Reverse horizontal
}
if(ball_y <= 0) {
  ball_vy = -ball_vy;  // Reverse vertical
}
```

**Paddle Collision**:
```c
if(ball_y + BALL_SIZE >= PADDLE_Y && 
   ball_y <= PADDLE_Y + PADDLE_H &&
   ball_x + BALL_SIZE >= paddle_x && 
   ball_x <= paddle_x + PADDLE_W) {
  ball_vy = -ball_vy;
  ball_y = PADDLE_Y - BALL_SIZE;  // Prevent sticking
}
```

**Brick Collision**:
```c
for each brick:
  if(brick.active && ball overlaps brick) {
    brick.active = false;
    score += 10;
    ball_vy = -ball_vy;
    break;  // One brick per frame
  }
```

### Color Palette

```c
Background:  0x0000  // Black
Paddle:      0x001F  // Blue
Ball:        0xF800  // Red
Bricks:      0xFFE0  // Yellow
Score:       0x8410  // Gray
Flash:       0xF800  // Red (game over)
```

## Code Structure

### Main Functions

```c
Brick_Init()      // Initialize game state
Brick_Tick()      // Main game loop (16ms)
Brick_GameOver()  // Handle game over sequence
```

### Key Variables

```c
paddle_x          // Paddle X position
ball_x, ball_y    // Ball position
ball_vx, ball_vy  // Ball velocity
bricks[5][7]      // Brick array (active/inactive)
brick_score       // Current score
brick_hi_score    // Best score
bricks_remaining  // Count of active bricks
```

### Game Flow

```
1. Brick_Init()
   ├─ Set paddle to center
   ├─ Set ball above paddle
   ├─ Initialize ball velocity
   ├─ Create all bricks (active)
   ├─ Reset score
   └─ Set LED green

2. Brick_Tick() [Every 16ms]
   ├─ Read paddle controls (SW4/SW2)
   ├─ Update paddle position
   ├─ Update ball position
   ├─ Check wall collisions
   ├─ Check paddle collision
   ├─ Check brick collisions
   ├─ Update score display
   ├─ Check game over conditions
   └─ Redraw changed elements

3. Brick_GameOver()
   ├─ Flash screen 3 times
   ├─ Display scores
   ├─ Play 3 long beeps
   ├─ Set LED red
   └─ Wait for touch to return
```

## Strategy Tips

### Beginner Tips
1. **Center Position**: Keep paddle near center for better coverage
2. **Watch Angles**: Ball bounces at predictable angles
3. **Corner Bricks**: Clear corner bricks first for easier access
4. **Paddle Edges**: Use paddle edges to angle shots
5. **Stay Calm**: Don't panic when ball moves fast

### Advanced Techniques
1. **Angle Control**: Hit ball with paddle edges for sharper angles
2. **Row Clearing**: Clear bottom rows first to prevent ball traps
3. **Speed Management**: Faster paddle movement for quick adjustments
4. **Bounce Prediction**: Anticipate ball trajectory
5. **Edge Shots**: Use wall bounces to reach difficult bricks

### Common Mistakes
1. **Overreacting**: Moving paddle too much
2. **Edge Camping**: Staying at screen edges
3. **Ignoring Corners**: Leaving corner bricks for last
4. **Panic Movement**: Random paddle movement
5. **Not Centering**: Poor paddle positioning

## Troubleshooting

### Paddle Not Moving
- Check SW2 (PC6) and SW4 (PC5) connections
- Verify button GPIO initialization
- Test buttons with multimeter

### Ball Not Bouncing Correctly
- Check collision detection logic
- Verify velocity calculations
- Ensure ball position updates properly

### Bricks Not Disappearing
- Check brick array initialization
- Verify collision detection with bricks
- Ensure brick.active flag is set/cleared

### Ball Stuck in Wall/Paddle
- Position adjustment after collision may be needed
- Check for proper velocity reversal
- Verify collision detection order

### No Sound
- Check buzzer connection on PA4
- Verify BUZZER_ON/OFF macros
- Test buzzer with simple toggle

### Score Not Updating
- Check brick collision detection
- Verify score increment logic
- Ensure display update is called

## Performance Metrics

### Memory Usage
- Brick array: 35 bytes (5×7 grid)
- Game variables: ~50 bytes
- Display buffer: 256 bytes (shared)
- Total: ~100 bytes RAM

### Display Operations
- Paddle: Redrawn on movement (~3 FPS)
- Ball: Redrawn every frame (62.5 FPS)
- Bricks: Drawn once, erased on destroy
- Score: Updated on brick break only

### Timing
- Game loop: 16ms (62.5 FPS)
- Paddle response: <16ms
- Ball update: Every frame
- Collision check: <1ms

## Future Enhancements

Possible improvements for Brick Breaker:

1. **Power-Ups**:
   - Multi-ball
   - Wider paddle
   - Slower ball
   - Laser paddle
   - Sticky paddle

2. **Brick Types**:
   - Multi-hit bricks (2-3 hits to break)
   - Indestructible bricks
   - Explosive bricks
   - Point multiplier bricks

3. **Levels**:
   - Different brick patterns
   - Progressive difficulty
   - Boss levels

4. **Lives System**:
   - 3 lives per game
   - Extra life at certain scores

5. **Visual Effects**:
   - Particle effects on brick break
   - Trail effect for ball
   - Paddle glow

6. **Audio**:
   - Different tones for different bricks
   - Background music
   - Combo sound effects

7. **Gameplay**:
   - Ball speed increases over time
   - Paddle size decreases with score
   - Moving bricks

## Known Limitations

1. **Fixed Difficulty**: No progressive speed increase
2. **Single Ball**: No multi-ball feature
3. **No Lives**: One chance per game
4. **Simple Physics**: Basic velocity model
5. **RAM High Score**: Lost on power cycle

## Credits

- Inspired by: Breakout (Atari, 1976) and Arkanoid (Taito, 1986)
- Platform: STM32F401RET6
- Display: ST7735 128×160 TFT
- Framework: STM32 HAL

---

**[← Back to Main README](../README.md)** | **[View Dino Game →](DINO_GAME.md)** | **[View Block Game →](BLOCK_GAME.md)**
