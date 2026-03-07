# Chrome Dino Runner Game

## Overview

The Chrome Dino Runner is an endless runner game inspired by Google Chrome's offline dinosaur game. Control a running dinosaur that must jump over cacti obstacles while the game progressively increases in difficulty.

## Game Features

### Core Mechanics
- **Endless Runner**: The game continues until the dinosaur collides with an obstacle
- **Progressive Difficulty**: Game speed increases every 80 points
- **Score System**: Distance-based scoring (1 point per 6 pixels traveled)
- **High Score Tracking**: Automatically saves and displays your best score

### Visual Features
- **Adaptive Display**: Automatically adjusts colors based on ambient light
  - **Dark Mode** (Low Light): Black background, white characters
  - **Light Mode** (Bright Light): White background, black characters
- **Animated Dino**: Two-frame running animation when on ground
- **Dynamic Obstacles**: Random spawning of small and large cacti
- **Ground Line**: Visual ground indicator with scrolling effect
- **HUD Display**: Real-time score display at the top

### Audio & Visual Feedback
- **Gameplay Beeps**: Double beep pattern every 0.7 seconds during play
- **Game Over Sound**: 3 long beeps (1.5s each) with 0.25s gaps
- **RGB LED Indicators**:
  - Green: Playing/Running
  - Blue: Jumping
  - Red: Game Over

## Controls

| Input | Action |
|-------|--------|
| Touch Sensor (PC8) | Jump (only when on ground) |
| Ambient Light (PB1) | Auto-adjusts display colors |

## Gameplay Instructions

### Starting the Game
1. From the main menu, use SW1/SW3 to navigate to "DINO"
2. Press the Touch sensor to start
3. The dinosaur will start running automatically

### Playing
1. **Jump**: Tap the touch sensor to make the dino jump
2. **Avoid Obstacles**: Jump over cacti (small and large)
3. **Score Points**: Survive as long as possible to increase your score
4. **Speed Increases**: Every 80 points, the game gets faster

### Game Over
1. Collision with any cactus ends the game
2. Screen flashes 3 times
3. Final score and high score are displayed
4. 3 long beeps indicate game over
5. Touch sensor to return to menu

## Technical Details

### Game Constants

```c
// Display
HUD_HEIGHT: 14 pixels
GROUND_Y: 138 pixels (TFT_HEIGHT - 22)
TICK_RATE: 16ms (62.5 FPS)

// Dinosaur
DINO_X: 14 pixels (fixed position)
DINO_WIDTH: 16 pixels
DINO_HEIGHT: 18 pixels
JUMP_VELOCITY: -38 (fixed point)
GRAVITY: 3 (fixed point)

// Obstacles
SMALL_CACTUS: 8×16 pixels
LARGE_CACTUS: 12×24 pixels
SPAWN_GAP_MIN: 70 pixels
SPAWN_GAP_RANGE: 80 pixels
MAX_OBSTACLES: 3 simultaneous

// Speed
INITIAL_SPEED: 8 (fixed point)
MAX_SPEED: 28 (fixed point)
SPEED_INCREMENT: Every 80 points
```

### Physics System

The game uses a fixed-point physics system for smooth movement:

**Jump Physics**:
- Initial velocity: -38 (upward)
- Gravity: +3 per frame
- Ground detection: Y position >= GROUND_Y

**Collision Detection**:
- Hitbox reduction: 6 pixels on all sides for fair gameplay
- AABB (Axis-Aligned Bounding Box) collision
- Checks: Dino vs each active obstacle

### Obstacle System

**Spawning**:
- Random type selection (33% large, 67% small)
- Minimum gap: 70 pixels
- Random additional gap: 0-79 pixels
- Spawns at right edge of screen

**Movement**:
- Scrolls left at current game speed
- Removed when off-screen (x < -width)
- Up to 3 obstacles active simultaneously

### Color Schemes

**Dark Mode** (Default/Low Light):
```c
Background: 0x0000 (Black)
Dino: 0xFFFF (White)
Obstacles: 0xFFFF (White)
Ground: 0x8410 (Gray)
Score: 0x8410 (Gray)
```

**Light Mode** (Bright Light):
```c
Background: 0xFFFF (White)
Dino: 0x0000 (Black)
Obstacles: 0x0000 (Black)
Ground: 0x7BEF (Light Gray)
Score: 0x7BEF (Light Gray)
```

## Performance Optimizations

1. **Batched Display Transfers**: 256-byte chunks instead of per-pixel
2. **Sprite Rendering**: One HAL call per scanline
3. **Selective Redrawing**: Only updates changed regions
4. **Fixed-Point Math**: Avoids floating-point operations
5. **Efficient Collision**: Early exit on first collision detected

## Scoring System

- **Base Score**: Distance traveled ÷ 6
- **Speed Bonus**: Faster speeds = more distance covered
- **High Score**: Automatically saved in RAM (lost on power cycle)

## Tips & Strategies

1. **Timing**: Learn the jump timing for different obstacle types
2. **Speed Management**: Game gets harder every 80 points
3. **Visual Cues**: Watch for obstacle spawning at screen edge
4. **Jump Height**: Full jump clears all obstacles
5. **Rhythm**: Develop a rhythm for consistent jumping

## Troubleshooting

### Dino Won't Jump
- Check touch sensor connection on PC8
- Verify dino is on ground (can't double jump)
- Ensure game is in PLAYING state

### Display Colors Wrong
- Check ambient light sensor on PB1
- Verify sensor is not covered/blocked
- Test in different lighting conditions

### Game Too Fast/Slow
- Speed increases automatically with score
- Check TICK_MS constant (should be 16ms)
- Verify 84 MHz system clock

### Collision Detection Issues
- Hitbox is intentionally smaller for fairness
- 6-pixel margin on all sides
- Check obstacle sprite alignment

## Code Structure

### Main Functions

```c
Dino_Init()      // Initialize game state
Dino_Tick()      // Main game loop (16ms)
Dino_GameOver()  // Handle game over sequence
Update_ColorScheme() // Ambient light handling
```

### Key Variables

```c
dino_y           // Dinosaur Y position
dino_vy_fp       // Vertical velocity (fixed point)
dino_on_ground   // Ground contact flag
speed_fp         // Current game speed
world_px         // Total distance traveled
dino_score       // Current score
dino_hi_score    // Best score
obs[MAX_OBS]     // Obstacle array
```

## Future Enhancements

Possible improvements for the Dino game:

1. **Power-ups**: Invincibility, slow-motion, double jump
2. **Flying Obstacles**: Pterodactyls that require ducking
3. **Day/Night Cycle**: Visual theme changes over time
4. **Combo System**: Bonus points for consecutive jumps
5. **Achievements**: Unlock milestones (100, 500, 1000 points)
6. **Difficulty Modes**: Easy, Normal, Hard presets
7. **Sound Effects**: Different beeps for jumps and collisions
8. **Persistent High Score**: Save to EEPROM/Flash

## Credits

- Inspired by: Google Chrome Offline Dinosaur Game
- Platform: STM32F401RET6
- Display: ST7735 128×160 TFT
- Framework: STM32 HAL

---

**[← Back to Main README](../README.md)** | **[View Brick Game →](BRICK_GAME.md)** | **[View Block Game →](BLOCK_GAME.md)**
