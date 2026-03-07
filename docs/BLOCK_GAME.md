# Block Blaster Game (Tetris-like)

## Overview

Block Blaster is a classic falling blocks puzzle game inspired by Tetris. Rotate and position falling tetromino shapes to create complete horizontal lines, which then disappear and award points. The game continues until blocks stack to the top of the playfield.

## Game Features

### Core Mechanics
- **7 Tetromino Shapes**: I, O, T, S, Z, J, L pieces
- **Rotation System**: 90-degree clockwise rotation (4 orientations)
- **Line Clearing**: Complete horizontal lines disappear
- **Score System**: 100 points per cleared line
- **Progressive Difficulty**: Blocks fall at consistent speed
- **High Score Tracking**: Saves your best score

### Visual Features
- **10×20 Grid**: Standard Tetris playfield dimensions
- **Color-Coded Pieces**: Each shape has a unique color
- **Grid Border**: White border around playfield
- **Real-Time Score**: Updates only when changed (no flicker)
- **Smooth Rendering**: Optimized to prevent UI glitches

### Audio & Visual Feedback
- **Movement Beeps**: Short beep on left/right movement
- **Rotation Beep**: Confirmation beep on successful rotation
- **Line Clear**: 100ms beep + LED flash on line clear
- **Game Over Sound**: 3 long beeps (1.5s each)
- **RGB LED Indicators**:
  - Green: Playing
  - Yellow Flash: Line cleared
  - Red: Game Over

## Controls

| Input | Action |
|-------|--------|
| SW4 (PC5) | Move block left |
| SW2 (PC6) | Move block right |
| SW1 (PB8) | Rotate block clockwise |
| Touch (PC8) | Return to menu (after game over) |

## Gameplay Instructions

### Starting the Game
1. From the main menu, use SW1/SW3 to navigate to "BLOCKS"
2. Press the Touch sensor to start
3. A random tetromino will appear at the top

### Playing
1. **Move Left/Right**: Use SW4/SW2 to position the block
2. **Rotate**: Press SW1 to rotate the block 90° clockwise
3. **Auto-Drop**: Blocks automatically fall every 15 frames (~240ms)
4. **Clear Lines**: Fill a complete horizontal row to clear it
5. **Stack Carefully**: Prevent blocks from reaching the top

### Scoring
- **Line Clear**: 100 points per line
- **Multiple Lines**: Can clear multiple lines simultaneously
- **High Score**: Best score is saved during session

### Game Over
1. New piece spawns with collision = Game Over
2. Screen flashes 3 times
3. Final score and high score displayed
4. 3 long beeps indicate game over
5. Touch sensor to return to menu

## Tetromino Shapes

### I-Piece (Cyan - 0x07FF)
```
    ████████████████
```
- 4 blocks in a line
- Best for clearing 4 lines at once
- Rotates between horizontal and vertical

### O-Piece (Yellow - 0xFFE0)
```
    ████████
    ████████
```
- 2×2 square
- Does not rotate (same in all orientations)
- Useful for filling gaps

### T-Piece (Magenta - 0xF81F)
```
      ████
    ████████████
```
- T-shaped piece
- 4 rotation states
- Versatile for various situations

### S-Piece (Green - 0x07E0)
```
      ████████
    ████████
```
- S-shaped piece
- 2 unique rotation states
- Creates offset patterns

### Z-Piece (Red - 0xF800)
```
    ████████
      ████████
```
- Z-shaped piece
- 2 unique rotation states
- Mirror of S-piece

### J-Piece (Blue - 0x001F)
```
    ████
    ████████████
```
- J-shaped piece
- 4 rotation states
- Good for corners

### L-Piece (Orange - 0xFD20)
```
          ████
    ████████████
```
- L-shaped piece
- 4 rotation states
- Mirror of J-piece

## Technical Details

### Game Constants

```c
// Grid
GRID_WIDTH: 10 blocks
GRID_HEIGHT: 20 blocks
BLOCK_SIZE: 6 pixels
GRID_X: 24 pixels (left offset)
GRID_Y: 20 pixels (top offset)

// Timing
DROP_DELAY: 15 frames (~240ms at 62.5 FPS)
TICK_RATE: 16ms (62.5 FPS)

// Scoring
LINE_CLEAR_POINTS: 100 per line
```

### Rotation System

**Rotation Matrix** (90° Clockwise):
```c
// For rotation state 0-3:
State 0: Original shape
State 1: rotated[r][c] = original[3-c][r]
State 2: rotated[r][c] = original[3-r][3-c]
State 3: rotated[r][c] = original[c][3-r]
```

**Collision Detection**:
- Checks all 4×4 grid positions
- Validates against playfield boundaries
- Checks against locked blocks
- Reverts rotation if collision detected

### Rendering Optimization

**Problem Solved**: Original implementation redrew entire screen every frame, causing flicker.

**Solution**:
1. **Selective Redrawing**: Only clear/draw blocks that moved
2. **Position Tracking**: Store old position to clear previous frame
3. **Grid Persistence**: Only redraw grid after line clears
4. **Score Optimization**: Update score display only when changed
5. **Border Once**: Draw grid border only during initialization

**Performance Gain**: ~90% reduction in display operations

### Line Clearing Algorithm

```c
1. Scan from bottom to top
2. For each row:
   - Check if all 10 blocks are filled
   - If full:
     a. Shift all rows above down by 1
     b. Clear top row
     c. Add 100 points
     d. Play sound + LED flash
     e. Re-check same row (in case of cascade)
3. Continue until top row reached
```

### Collision Detection

**Boundary Checks**:
- Left edge: x < 0
- Right edge: x >= GRID_WIDTH
- Bottom edge: y >= GRID_HEIGHT

**Block Checks**:
- For each filled cell in tetromino
- Check corresponding grid position
- Collision if grid cell is occupied

## Color Palette

```c
Red:     0xF800  // Z-piece
Green:   0x07E0  // S-piece
Blue:    0x001F  // J-piece
Yellow:  0xFFE0  // O-piece
Magenta: 0xF81F  // T-piece
Cyan:    0x07FF  // I-piece
Orange:  0xFD20  // L-piece
```

## Code Structure

### Main Functions

```c
Block_Init()           // Initialize game state
Block_Tick()           // Main game loop (16ms)
Block_GameOver()       // Handle game over sequence
Block_NewPiece()       // Spawn new random piece
Block_RotatePiece()    // Handle rotation logic
Block_CheckCollision() // Collision detection
Block_LockPiece()      // Lock piece to grid
Block_ClearLines()     // Line clearing algorithm
```

### Key Variables

```c
grid[20][10]      // Playfield state
curr_x, curr_y    // Current piece position
curr_shape        // Current piece type (0-6)
curr_rot          // Current rotation (0-3)
curr_block[4][4]  // Current piece data
drop_timer        // Auto-drop counter
drop_delay        // Frames before drop
block_score       // Current score
block_hi_score    // Best score
```

## Strategy Tips

### Beginner Tips
1. **Keep It Flat**: Avoid creating tall stacks
2. **Save I-Pieces**: Use for clearing multiple lines
3. **Rotate Early**: Plan rotation before piece drops
4. **Use Walls**: Rotate against walls for positioning
5. **Clear Often**: Don't wait for perfect 4-line clear

### Advanced Techniques
1. **T-Spin Setup**: Create T-shaped holes for T-pieces
2. **Well Building**: Create a 1-block wide well for I-pieces
3. **Stacking Patterns**: Alternate S and Z pieces efficiently
4. **Rotation Timing**: Rotate while moving for faster placement
5. **Preview Planning**: Think ahead for next piece placement

### Common Mistakes
1. **Overstacking**: Creating uneven heights
2. **Hole Creation**: Leaving gaps under blocks
3. **Panic Placement**: Rushing without planning
4. **Ignoring Sides**: Not using full width of playfield
5. **No Rotation**: Forgetting to rotate pieces

## Troubleshooting

### Blocks Won't Rotate
- Check SW1 button connection (PB8)
- Verify rotation isn't blocked by walls/blocks
- Ensure piece isn't O-piece (doesn't rotate)

### Blocks Fall Too Fast/Slow
- DROP_DELAY set to 15 frames (~240ms)
- Check TICK_MS is 16ms
- Verify 84 MHz system clock

### UI Flickering
- Should be fixed with optimized rendering
- Check that full screen redraw only happens after line clear
- Verify selective redrawing is working

### Lines Not Clearing
- Check Block_ClearLines() function
- Verify all 10 blocks in row are filled
- Ensure grid array is properly updated

### Collision Detection Issues
- Verify 4×4 tetromino data is correct
- Check boundary conditions (0, GRID_W, GRID_H)
- Ensure curr_block array is updated on rotation

## Performance Metrics

### Before Optimization
- Full screen redraw: Every frame (16ms)
- Display operations: ~200 per frame
- Visible flicker: Yes
- Frame drops: Occasional

### After Optimization
- Selective redraw: Only changed blocks
- Display operations: ~20 per frame (90% reduction)
- Visible flicker: None
- Frame drops: None

## Future Enhancements

Possible improvements for Block Blaster:

1. **Hold Piece**: Store one piece for later use
2. **Next Piece Preview**: Show upcoming piece
3. **Ghost Piece**: Show where piece will land
4. **Hard Drop**: Instant drop to bottom
5. **Soft Drop**: Faster drop when holding button
6. **Level System**: Increase speed with score
7. **Combo System**: Bonus for consecutive line clears
8. **Wall Kicks**: Allow rotation near walls
9. **Score Multipliers**: Bonus for T-spins, combos
10. **Persistent High Score**: Save to EEPROM

## Known Limitations

1. **No Hold Function**: Cannot store pieces
2. **No Preview**: Next piece is unknown
3. **Fixed Speed**: No progressive difficulty
4. **Simple Rotation**: No wall kick system
5. **RAM High Score**: Lost on power cycle

## Credits

- Inspired by: Tetris (Alexey Pajitnov, 1984)
- Platform: STM32F401RET6
- Display: ST7735 128×160 TFT
- Framework: STM32 HAL

---

**[← Back to Main README](../README.md)** | **[View Dino Game →](DINO_GAME.md)** | **[View Brick Game →](BRICK_GAME.md)**
