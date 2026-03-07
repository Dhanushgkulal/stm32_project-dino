/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    main.c
  * @brief   Game Console: Dino Runner + Brick Breaker
  *          Display: ST7735 128x160
  *          Controls: 4 Buttons + Touch Sensor + Ambient Light
  *
  *  Features:
  *  - Menu system to select games
  *  - Chrome Dino runner game
  *  - Classic Brick Breaker game
  *  - Buzzer audio feedback
  *  - RGB LED indicators
  *  - Ambient light adaptive display
  *  - Optimized 84 MHz / 10.5 MHz SPI
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include <string.h>
#include <stdlib.h>

/* ===========================================================================
   TYPEDEFS
   =========================================================================== */
typedef enum { APP_MENU, APP_DINO, APP_BRICK, APP_BLOCKS } AppState;
typedef enum { GS_TITLE, GS_PLAYING, GS_GAMEOVER } GameState;

/* ===========================================================================
   PIN MACROS
   =========================================================================== */
#define TFT_RST_LOW()   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET)
#define TFT_RST_HIGH()  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET)
#define TFT_CS_LOW()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)
#define TFT_CS_HIGH()   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)
#define TFT_DC_CMD()    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET)
#define TFT_DC_DATA()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET)

/* Input sensors */
#define TOUCH_PRESSED   (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == GPIO_PIN_SET)
#define LIGHT_BRIGHT    (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_SET)
#define SW1_PRESSED     (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == GPIO_PIN_SET)
#define SW2_PRESSED     (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6) == GPIO_PIN_SET)
#define SW3_PRESSED     (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == GPIO_PIN_SET)
#define SW4_PRESSED     (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5) == GPIO_PIN_SET)

/* Buzzer */
#define BUZZER_ON()     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)
#define BUZZER_OFF()    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)

/* RGB LED */
#define LED_RED_ON()    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET)
#define LED_RED_OFF()   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET)
#define LED_GREEN_ON()  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET)
#define LED_GREEN_OFF() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET)
#define LED_BLUE_ON()   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET)
#define LED_BLUE_OFF()  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET)

/* Additional LEDs */
#define LED4_ON()       HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET)
#define LED4_OFF()      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET)
#define LED3_ON()       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET)
#define LED3_OFF()      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET)
#define LED2_ON()       HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET)
#define LED2_OFF()      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET)
#define LED1_ON()       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET)
#define LED1_OFF()      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET)

/* ===========================================================================
   ST7735 COMMANDS
   =========================================================================== */
#define ST7735_SWRESET  0x01
#define ST7735_SLPOUT   0x11
#define ST7735_NORON    0x13
#define ST7735_INVOFF   0x20
#define ST7735_DISPON   0x29
#define ST7735_CASET    0x2A
#define ST7735_RASET    0x2B
#define ST7735_RAMWR    0x2C
#define ST7735_MADCTL   0x36
#define ST7735_COLMOD   0x3A
#define ST7735_FRMCTR1  0xB1
#define ST7735_FRMCTR2  0xB2
#define ST7735_FRMCTR3  0xB3
#define ST7735_INVCTR   0xB4
#define ST7735_PWCTR1   0xC0
#define ST7735_PWCTR2   0xC1
#define ST7735_PWCTR3   0xC2
#define ST7735_PWCTR4   0xC3
#define ST7735_PWCTR5   0xC4
#define ST7735_VMCTR1   0xC5
#define ST7735_GMCTRP1  0xE0
#define ST7735_GMCTRN1  0xE1

/* ===========================================================================
   DISPLAY
   =========================================================================== */
#define TFT_WIDTH   128
#define TFT_HEIGHT  160

/* ===========================================================================
   COLORS
   =========================================================================== */
#define COLOR_BG_DARK         0x0000
#define COLOR_DINO_DARK       0xFFFF
#define COLOR_OBSTACLE_DARK   0xFFFF
#define COLOR_GROUND_DARK     0x8410
#define COLOR_SCORE_DARK      0x8410
#define COLOR_SKY_DARK        0xC618
#define COLOR_CLOUD_DARK      0xDEFB

#define COLOR_BG_LIGHT         0xFFFF
#define COLOR_DINO_LIGHT       0x0000
#define COLOR_OBSTACLE_LIGHT   0x0000
#define COLOR_GROUND_LIGHT     0x7BEF
#define COLOR_SCORE_LIGHT      0x7BEF
#define COLOR_SKY_LIGHT        0x39E7
#define COLOR_CLOUD_LIGHT      0x2104

static uint16_t COLOR_BG, COLOR_DINO, COLOR_OBSTACLE, COLOR_GROUND, COLOR_SCORE, COLOR_SKY, COLOR_CLOUD;
#define COLOR_GAMEOVER   0x2104
#define COLOR_FLASH      0xF800
#define COLOR_MENU_SEL   0x07E0
#define COLOR_MENU_TEXT  0xFFFF
#define COLOR_PADDLE     0x001F
#define COLOR_BALL       0xF800
#define COLOR_BRICK      0xFFE0

/* Block colors */
static const uint16_t BLOCK_COL[7] = {
  0xF800, // Red
  0x07E0, // Green
  0x001F, // Blue
  0xFFE0, // Yellow
  0xF81F, // Magenta
  0x07FF, // Cyan
  0xFD20  // Orange
};

/* ===========================================================================
   DINO GAME CONSTANTS
   =========================================================================== */
#define HUD_H         14
#define GROUND_Y      (TFT_HEIGHT - 22)
#define DINO_X        14
#define DINO_W        16
#define DINO_H        18
#define DINO_GROUND_Y (GROUND_Y - DINO_H)
#define FP_SHIFT      2
#define JUMP_VY_INIT  (-38)
#define GRAVITY        3
#define MAX_OBS            3
#define OBS_W_SMALL        8
#define OBS_H_SMALL        16
#define OBS_W_LARGE        12
#define OBS_H_LARGE        24
#define OBS_SPAWN_GAP_MIN  70
#define OBS_SPAWN_GAP_RNG  80
#define INIT_SPEED_FP      8
#define MAX_SPEED_FP       28
#define SPEED_INC_SCORE    80

/* ===========================================================================
   BRICK GAME CONSTANTS
   =========================================================================== */
#define PADDLE_W      20
#define PADDLE_H      4
#define PADDLE_Y      (TFT_HEIGHT - 10)
#define BALL_SIZE     3
#define BRICK_W       16
#define BRICK_H       6
#define BRICK_ROWS    5
#define BRICK_COLS    7
#define BRICK_START_Y 20

/* ===========================================================================
   BLOCK GAME CONSTANTS (Tetris-like)
   =========================================================================== */
#define BLOCK_SIZE    6
#define GRID_W        10
#define GRID_H        20
#define GRID_X        24
#define GRID_Y        20
#define BLOCK_COLORS  7

/* ===========================================================================
   TIMING
   =========================================================================== */
#define TICK_MS   16

/* ===========================================================================
   TRANSFER BUFFER
   =========================================================================== */
#define TBUF_SZ  256
static uint8_t TBUF[TBUF_SZ];

/* ===========================================================================
   SPRITES
   =========================================================================== */
static const uint16_t dinoA[18] = {
  0x01E0,0x03F0,0x03F8,0x0368,0x03F8,0x01F0,0x00F0,0x03C0,0x07E0,
  0x0FE0,0x1FE0,0x0FC0,0x07C0,0x0380,0x0180,0x0180,0x0300,0x0000
};
static const uint16_t dinoB[18] = {
  0x01E0,0x03F0,0x03F8,0x0368,0x03F8,0x01F0,0x00F0,0x03C0,0x07E0,
  0x0FE0,0x1FE0,0x0FC0,0x07C0,0x0380,0x0300,0x0300,0x0000,0x0000
};
static const uint16_t dinoDead[18] = {
  0x01E0,0x03F0,0x03D8,0x0308,0x03F8,0x01F0,0x00F0,0x03C0,0x07E0,
  0x0FE0,0x1FE0,0x0FC0,0x07C0,0x0380,0x0180,0x0180,0x0300,0x0000
};
static const uint8_t cactusS[16] = {
  0x18,0x18,0x98,0xF8,0x1E,0x18,0x18,0x18,
  0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x3C
};
static const uint16_t cactusL[24] = {
  0x0600,0x0600,0x4E00,0x7E00,0x07C0,0x0600,0x0600,0x0600,
  0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,
  0x0600,0x0600,0x0600,0x0600,0x0600,0x0600,0x0F00,0x0F00
};
static const uint32_t cloudSpr[7] = {
  0x000E00UL,0x003F80UL,0x1FFFC0UL,0x3FFFC0UL,
  0x1FFFC0UL,0x07FF80UL,0x000000UL
};
static const uint8_t digitFont[10][5] = {
  {7,5,5,5,7},{2,6,2,2,7},{7,1,7,4,7},{7,1,3,1,7},{5,5,7,1,1},
  {7,4,7,1,7},{7,4,7,5,7},{7,1,2,2,2},{7,5,7,5,7},{7,5,7,1,7}
};

/* ===========================================================================
   GAME STRUCTS
   =========================================================================== */
typedef struct { int16_t x,y; uint8_t w,h,large,active; } Obstacle;
typedef struct { int16_t x,y; uint8_t active; } Cloud;
typedef struct { uint8_t active; } Brick;

/* Tetromino shapes (4x4 grid) */
static const uint8_t SHAPES[7][4][4] = {
  // I
  {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
  // O
  {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
  // T
  {{0,0,0,0},{0,1,0,0},{1,1,1,0},{0,0,0,0}},
  // S
  {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},
  // Z
  {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},
  // J
  {{0,0,0,0},{1,0,0,0},{1,1,1,0},{0,0,0,0}},
  // L
  {{0,0,0,0},{0,0,1,0},{1,1,1,0},{0,0,0,0}}
};

/* ===========================================================================
   GLOBALS
   =========================================================================== */
SPI_HandleTypeDef hspi1;

static AppState   app_state = APP_MENU;
static uint8_t    menu_selection = 0;

/* Dino game */
static GameState  dino_state;
static int16_t    dino_y;
static int16_t    dino_vy_fp;
static uint8_t    dino_on_ground;
static uint8_t    dino_frame;
static uint8_t    dino_frame_cnt;
static Obstacle   obs[MAX_OBS];
static Cloud      clouds[2];
static int32_t    speed_fp;
static int32_t    scroll_accum;
static uint32_t   world_px;
static uint32_t   next_obs_world;
static uint32_t   dino_score;
static uint32_t   dino_hi_score;
static uint32_t   last_speed_score;
static uint32_t   ground_offset;

/* Brick game */
static GameState  brick_state;
static int16_t    paddle_x;
static int16_t    ball_x, ball_y;
static int8_t     ball_vx, ball_vy;
static Brick      bricks[BRICK_ROWS][BRICK_COLS];
static uint32_t   brick_score;
static uint32_t   brick_hi_score;
static uint8_t    bricks_remaining;

/* Block game (Tetris) */
static GameState  block_state;
static uint8_t    grid[GRID_H][GRID_W];
static int8_t     curr_x, curr_y;
static uint8_t    curr_shape, curr_rot;
static uint8_t    curr_block[4][4];
static uint32_t   block_score;
static uint32_t   block_hi_score;
static uint32_t   drop_timer;
static uint16_t   drop_delay;

/* Common */
static uint8_t    touch_prev, sw1_prev, sw2_prev, sw3_prev, sw4_prev;
static uint32_t   rng_seed = 0xDEADBEEFUL;
static uint32_t   beep_timer;
static uint8_t    beep_state, beep_count;
static uint8_t    light_mode = 0;

/* ===========================================================================
   PROTOTYPES
   =========================================================================== */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
void Error_Handler(void);

static void TFT_SendCmd(uint8_t cmd);
static void TFT_SendData(uint8_t data);
void        TFT_Init(void);
static void TFT_SetWindow(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1);
void        TFT_FillRect(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t color);
void        TFT_FillScreen(uint16_t color);
static void Spr16_Draw(int16_t x,int16_t y,const uint16_t *rows,uint8_t w,uint8_t h,uint16_t fg,uint16_t bg);
static void Spr8_Draw (int16_t x,int16_t y,const uint8_t  *rows,uint8_t w,uint8_t h,uint16_t fg,uint16_t bg);
static void TFT_DrawDigit (uint8_t x,uint8_t y,uint8_t d,uint16_t col);
static void TFT_DrawNumber(uint8_t x,uint8_t y,uint32_t n,uint8_t digits,uint16_t col);

static uint32_t RNG_Next(void);
static uint8_t  Touch_Once(void);
static uint8_t  Button_Once(uint8_t *prev, uint8_t current);
static void     LED_SetColor(uint8_t red, uint8_t green, uint8_t blue);
static void     LED_AllOff(void);
static void     Buzzer_Update(void);
static void     Update_ColorScheme(void);

static void Menu_Init(void);
static void Menu_Update(void);
static void Menu_Draw(void);

static void Dino_Init(void);
static void Dino_Tick(void);
static void Dino_GameOver(void);

static void Brick_Init(void);
static void Brick_Tick(void);
static void Brick_GameOver(void);

static void Block_Init(void);
static void Block_Tick(void);
static void Block_GameOver(void);
static uint8_t Block_CheckCollision(int8_t x, int8_t y, uint8_t rot);
static void Block_LockPiece(void);
static void Block_ClearLines(void);
static void Block_NewPiece(void);
static void Block_RotatePiece(void);

/* ===========================================================================
   TFT DRIVER
   =========================================================================== */
static void TFT_SendCmd(uint8_t cmd){
  TFT_DC_CMD(); TFT_CS_LOW();
  HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
  TFT_CS_HIGH();
}

static void TFT_SendData(uint8_t data){
  TFT_DC_DATA(); TFT_CS_LOW();
  HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
  TFT_CS_HIGH();
}

void TFT_Init(void){
  TFT_RST_HIGH(); HAL_Delay(10);
  TFT_RST_LOW();  HAL_Delay(10);
  TFT_RST_HIGH(); HAL_Delay(120);
  
  TFT_SendCmd(ST7735_SWRESET); HAL_Delay(150);
  TFT_SendCmd(ST7735_SLPOUT);  HAL_Delay(120);
  TFT_SendCmd(ST7735_FRMCTR1); TFT_SendData(0x01); TFT_SendData(0x2C); TFT_SendData(0x2D);
  TFT_SendCmd(ST7735_FRMCTR2); TFT_SendData(0x01); TFT_SendData(0x2C); TFT_SendData(0x2D);
  TFT_SendCmd(ST7735_FRMCTR3); TFT_SendData(0x01); TFT_SendData(0x2C); TFT_SendData(0x2D);
  TFT_SendData(0x01); TFT_SendData(0x2C); TFT_SendData(0x2D);
  TFT_SendCmd(ST7735_INVCTR);  TFT_SendData(0x07);
  TFT_SendCmd(ST7735_PWCTR1);  TFT_SendData(0xA2); TFT_SendData(0x02); TFT_SendData(0x84);
  TFT_SendCmd(ST7735_PWCTR2);  TFT_SendData(0xC5);
  TFT_SendCmd(ST7735_PWCTR3);  TFT_SendData(0x0A); TFT_SendData(0x00);
  TFT_SendCmd(ST7735_PWCTR4);  TFT_SendData(0x8A); TFT_SendData(0x2A);
  TFT_SendCmd(ST7735_PWCTR5);  TFT_SendData(0x8A); TFT_SendData(0xEE);
  TFT_SendCmd(ST7735_VMCTR1);  TFT_SendData(0x0E);
  TFT_SendCmd(ST7735_INVOFF);
  TFT_SendCmd(ST7735_MADCTL);  TFT_SendData(0xC8);
  TFT_SendCmd(ST7735_COLMOD);  TFT_SendData(0x05);
  TFT_SendCmd(ST7735_GMCTRP1);
  TFT_SendData(0x02); TFT_SendData(0x1C); TFT_SendData(0x07); TFT_SendData(0x12);
  TFT_SendData(0x37); TFT_SendData(0x32); TFT_SendData(0x29); TFT_SendData(0x2D);
  TFT_SendData(0x29); TFT_SendData(0x25); TFT_SendData(0x2B); TFT_SendData(0x39);
  TFT_SendData(0x00); TFT_SendData(0x01); TFT_SendData(0x03); TFT_SendData(0x10);
  TFT_SendCmd(ST7735_GMCTRN1);
  TFT_SendData(0x03); TFT_SendData(0x1D); TFT_SendData(0x07); TFT_SendData(0x06);
  TFT_SendData(0x2E); TFT_SendData(0x2C); TFT_SendData(0x29); TFT_SendData(0x2D);
  TFT_SendData(0x2E); TFT_SendData(0x2E); TFT_SendData(0x37); TFT_SendData(0x3F);
  TFT_SendData(0x00); TFT_SendData(0x00); TFT_SendData(0x02); TFT_SendData(0x10);
  TFT_SendCmd(ST7735_NORON);  HAL_Delay(10);
  TFT_SendCmd(ST7735_DISPON); HAL_Delay(10);
}

static void TFT_SetWindow(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1){
  TFT_SendCmd(ST7735_CASET);
  TFT_SendData(0); TFT_SendData(x0); TFT_SendData(0); TFT_SendData(x1);
  TFT_SendCmd(ST7735_RASET);
  TFT_SendData(0); TFT_SendData(y0); TFT_SendData(0); TFT_SendData(y1);
  TFT_SendCmd(ST7735_RAMWR);
}

void TFT_FillRect(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t color){
  if(w<=0||h<=0) return;
  if(x<0){w+=x;x=0;} if(y<0){h+=y;y=0;}
  if(x>=TFT_WIDTH||y>=TFT_HEIGHT) return;
  if(x+w>TFT_WIDTH)  w=TFT_WIDTH-x;
  if(y+h>TFT_HEIGHT) h=TFT_HEIGHT-y;
  if(w<=0||h<=0) return;
  
  TFT_SetWindow((uint8_t)x,(uint8_t)y,(uint8_t)(x+w-1),(uint8_t)(y+h-1));
  uint8_t hi=(uint8_t)(color>>8), lo=(uint8_t)(color);
  for(uint16_t i=0;i<TBUF_SZ;i+=2){ TBUF[i]=hi; TBUF[i+1]=lo; }
  uint32_t total=(uint32_t)w*(uint32_t)h;
  TFT_DC_DATA(); TFT_CS_LOW();
  while(total>0){
    uint16_t chunk=(total>(TBUF_SZ/2))?(TBUF_SZ/2):(uint16_t)total;
    HAL_SPI_Transmit(&hspi1, TBUF, chunk*2, HAL_MAX_DELAY);
    total-=chunk;
  }
  TFT_CS_HIGH();
}

void TFT_FillScreen(uint16_t color){
  TFT_FillRect(0,0,TFT_WIDTH,TFT_HEIGHT,color);
}

/* ===========================================================================
   SPRITE DRAWING (Compact)
   =========================================================================== */
static void Spr16_Draw(int16_t sx,int16_t sy,const uint16_t *rows,uint8_t w,uint8_t h,uint16_t fg,uint16_t bg){
  if(sx+(int16_t)w<=0||sx>=TFT_WIDTH||sy+(int16_t)h<=0||sy>=TFT_HEIGHT) return;
  int16_t x0=sx,y0=sy,x1=(int16_t)(sx+w-1),y1=(int16_t)(sy+h-1);
  if(x0<0) x0=0;
  if(y0<0) y0=0;
  if(x1>=TFT_WIDTH)  x1=TFT_WIDTH-1;
  if(y1>=TFT_HEIGHT) y1=TFT_HEIGHT-1;
  uint8_t fH=(uint8_t)(fg>>8),fL=(uint8_t)(fg),bH=(uint8_t)(bg>>8),bL=(uint8_t)(bg);
  TFT_SetWindow((uint8_t)x0,(uint8_t)y0,(uint8_t)x1,(uint8_t)y1);
  TFT_DC_DATA(); TFT_CS_LOW();
  for(int row=y0-sy; row<=(y1-sy); row++){
    uint16_t bits=rows[row], n=0;
    for(int col=x0-sx; col<=(x1-sx); col++){
      if(bits&(0x8000u>>col)){ TBUF[n++]=fH; TBUF[n++]=fL; }
      else                   { TBUF[n++]=bH; TBUF[n++]=bL; }
    }
    HAL_SPI_Transmit(&hspi1, TBUF, n, HAL_MAX_DELAY);
  }
  TFT_CS_HIGH();
}

static void Spr8_Draw(int16_t sx,int16_t sy,const uint8_t *rows,uint8_t w,uint8_t h,uint16_t fg,uint16_t bg){
  if(sx+(int16_t)w<=0||sx>=TFT_WIDTH||sy+(int16_t)h<=0||sy>=TFT_HEIGHT) return;
  int16_t x0=sx,y0=sy,x1=(int16_t)(sx+w-1),y1=(int16_t)(sy+h-1);
  if(x0<0) x0=0;
  if(y0<0) y0=0;
  if(x1>=TFT_WIDTH)  x1=TFT_WIDTH-1;
  if(y1>=TFT_HEIGHT) y1=TFT_HEIGHT-1;
  uint8_t fH=(uint8_t)(fg>>8),fL=(uint8_t)(fg),bH=(uint8_t)(bg>>8),bL=(uint8_t)(bg);
  TFT_SetWindow((uint8_t)x0,(uint8_t)y0,(uint8_t)x1,(uint8_t)y1);
  TFT_DC_DATA(); TFT_CS_LOW();
  for(int row=y0-sy; row<=(y1-sy); row++){
    uint8_t bits=rows[row]; uint16_t n=0;
    for(int col=x0-sx; col<=(x1-sx); col++){
      if(bits&(0x80u>>col)){ TBUF[n++]=fH; TBUF[n++]=fL; }
      else                  { TBUF[n++]=bH; TBUF[n++]=bL; }
    }
    HAL_SPI_Transmit(&hspi1, TBUF, n, HAL_MAX_DELAY);
  }
  TFT_CS_HIGH();
}

static void TFT_DrawDigit(uint8_t x,uint8_t y,uint8_t d,uint16_t col){
  for(int r=0;r<5;r++)
    for(int c=0;c<3;c++)
      TFT_FillRect(x+c*2,y+r*2,2,2,(digitFont[d][r]&(0x4>>c))?col:COLOR_BG);
}

static void TFT_DrawNumber(uint8_t x,uint8_t y,uint32_t n,uint8_t digits,uint16_t col){
  char buf[8]={0}; int8_t i=digits-1;
  for(;i>=0;i--){ buf[i]=(char)(n%10); n/=10; }
  for(int j=0;j<digits;j++){ TFT_DrawDigit(x,y,(uint8_t)buf[j],col); x+=8; }
}

/* ===========================================================================
   UTILS
   =========================================================================== */
static uint32_t RNG_Next(void){ rng_seed=rng_seed*1664525UL+1013904223UL; return rng_seed; }

static uint8_t Touch_Once(void){
  uint8_t cur=TOUCH_PRESSED?1u:0u;
  uint8_t edge=(uint8_t)(cur&~touch_prev);
  touch_prev=cur; return edge;
}

static uint8_t Button_Once(uint8_t *prev, uint8_t current){
  uint8_t edge=(uint8_t)(current&~(*prev));
  *prev=current; return edge;
}

static void LED_SetColor(uint8_t red, uint8_t green, uint8_t blue){
  if(red)   LED_RED_ON();   else LED_RED_OFF();
  if(green) LED_GREEN_ON(); else LED_GREEN_OFF();
  if(blue)  LED_BLUE_ON();  else LED_BLUE_OFF();
}

static void LED_AllOff(void){
  LED_RED_OFF(); LED_GREEN_OFF(); LED_BLUE_OFF();
  LED1_OFF(); LED2_OFF(); LED3_OFF(); LED4_OFF();
}

static void Buzzer_Update(void){
  uint32_t now = HAL_GetTick();
  if(now - beep_timer >= 100) {
    beep_timer = now;
    if(beep_state == 0) { BUZZER_ON(); beep_state = 1; }
    else if(beep_state == 1) { BUZZER_OFF(); beep_state = 2; }
    else if(beep_state == 2) { beep_state = 3; }
    else if(beep_state == 3) { BUZZER_ON(); beep_state = 4; }
    else if(beep_state == 4) { BUZZER_OFF(); beep_state = 5; beep_count = 0; }
    else if(beep_state == 5) { beep_count++; if(beep_count >= 7) beep_state = 0; }
  }
}

static void Update_ColorScheme(void){
  uint8_t new_mode = LIGHT_BRIGHT ? 1 : 0;
  if(new_mode != light_mode) {
    light_mode = new_mode;
    if(light_mode) {
      COLOR_BG = COLOR_BG_LIGHT; COLOR_DINO = COLOR_DINO_LIGHT;
      COLOR_OBSTACLE = COLOR_OBSTACLE_LIGHT; COLOR_GROUND = COLOR_GROUND_LIGHT;
      COLOR_SCORE = COLOR_SCORE_LIGHT; COLOR_SKY = COLOR_SKY_LIGHT;
      COLOR_CLOUD = COLOR_CLOUD_LIGHT;
    } else {
      COLOR_BG = COLOR_BG_DARK; COLOR_DINO = COLOR_DINO_DARK;
      COLOR_OBSTACLE = COLOR_OBSTACLE_DARK; COLOR_GROUND = COLOR_GROUND_DARK;
      COLOR_SCORE = COLOR_SCORE_DARK; COLOR_SKY = COLOR_SKY_DARK;
      COLOR_CLOUD = COLOR_CLOUD_DARK;
    }
    TFT_FillScreen(COLOR_BG);
  }
}

/* ===========================================================================
   MENU SYSTEM
   =========================================================================== */
static void Menu_Init(void){
  menu_selection = 0;
  LED_SetColor(1, 1, 0);
  // Set default dark mode for menu
  COLOR_BG = COLOR_BG_DARK;
  COLOR_DINO = COLOR_DINO_DARK;
  COLOR_OBSTACLE = COLOR_OBSTACLE_DARK;
  COLOR_GROUND = COLOR_GROUND_DARK;
  COLOR_SCORE = COLOR_SCORE_DARK;
  COLOR_SKY = COLOR_SKY_DARK;
  COLOR_CLOUD = COLOR_CLOUD_DARK;
  Menu_Draw();
}

static void Menu_Draw(void){
  TFT_FillScreen(COLOR_BG);
  uint16_t text_col = 0xFFFF;  // Always white text in menu
  uint16_t sel_col = 0x07E0;  // Green selection
  
  // Title "GAMES"
  TFT_FillRect(35,10,58,20,text_col);
  TFT_FillRect(37,12,54,16,COLOR_BG);
  TFT_FillRect(40,16,48,8,text_col);
  
  // Game 1: DINO
  uint16_t y1=45;
  if(menu_selection==0){
    TFT_FillRect(15,y1-5,98,22,sel_col);
    TFT_FillRect(17,y1-3,94,18,COLOR_BG);
  }
  // Draw "DINO" text
  TFT_FillRect(25,y1+2,8,10,text_col);
  TFT_FillRect(25,y1+2,12,3,text_col);
  TFT_FillRect(25,y1+9,12,3,text_col);
  TFT_FillRect(35,y1+2,8,10,text_col);
  TFT_FillRect(48,y1+2,3,10,text_col);
  TFT_FillRect(56,y1+2,3,10,text_col);
  TFT_FillRect(56,y1+2,10,3,text_col);
  TFT_FillRect(63,y1+2,3,10,text_col);
  TFT_FillRect(71,y1+2,10,10,text_col);
  TFT_FillRect(73,y1+4,6,6,COLOR_BG);
  
  // Game 2: BRICK
  uint16_t y2=80;
  if(menu_selection==1){
    TFT_FillRect(15,y2-5,98,22,sel_col);
    TFT_FillRect(17,y2-3,94,18,COLOR_BG);
  }
  // Draw "BRICK" text
  TFT_FillRect(18,y2+2,10,10,text_col);
  TFT_FillRect(20,y2+4,6,2,COLOR_BG);
  TFT_FillRect(20,y2+8,6,2,COLOR_BG);
  TFT_FillRect(32,y2+2,10,10,text_col);
  TFT_FillRect(34,y2+4,6,6,COLOR_BG);
  TFT_FillRect(39,y2+7,3,5,text_col);
  TFT_FillRect(46,y2+2,3,10,text_col);
  TFT_FillRect(53,y2+2,10,3,text_col);
  TFT_FillRect(53,y2+9,10,3,text_col);
  TFT_FillRect(53,y2+2,3,10,text_col);
  TFT_FillRect(67,y2+2,3,10,text_col);
  TFT_FillRect(67,y2+6,7,3,text_col);
  TFT_FillRect(71,y2+2,3,5,text_col);
  TFT_FillRect(71,y2+8,3,4,text_col);
  
  // Game 3: BLOCKS
  uint16_t y3=115;
  if(menu_selection==2){
    TFT_FillRect(15,y3-5,98,22,sel_col);
    TFT_FillRect(17,y3-3,94,18,COLOR_BG);
  }
  // Draw "BLOCKS" text
  TFT_FillRect(12,y3+2,10,10,text_col);
  TFT_FillRect(14,y3+4,6,2,COLOR_BG);
  TFT_FillRect(14,y3+8,6,2,COLOR_BG);
  TFT_FillRect(26,y3+2,3,10,text_col);
  TFT_FillRect(26,y3+9,8,3,text_col);
  TFT_FillRect(31,y3+2,3,10,text_col);
  TFT_FillRect(38,y3+2,10,10,text_col);
  TFT_FillRect(40,y3+4,6,6,COLOR_BG);
  TFT_FillRect(52,y3+2,3,10,text_col);
  TFT_FillRect(52,y3+6,7,3,text_col);
  TFT_FillRect(56,y3+2,3,5,text_col);
  TFT_FillRect(56,y3+8,3,4,text_col);
  TFT_FillRect(63,y3+2,10,3,text_col);
  TFT_FillRect(63,y3+9,10,3,text_col);
  TFT_FillRect(70,y3+2,3,10,text_col);
  
  // Instructions at bottom
  TFT_FillRect(20,145,88,2,text_col);
  TFT_FillRect(30,150,68,2,text_col);
}

static void Menu_Update(void){
  // No ambient light update in menu
  
  if(Button_Once(&sw1_prev, SW1_PRESSED)){
    if(menu_selection > 0) menu_selection--;
    else menu_selection = 2;
    Menu_Draw();
    BUZZER_ON(); HAL_Delay(50); BUZZER_OFF();
  }
  
  if(Button_Once(&sw3_prev, SW3_PRESSED)){
    if(menu_selection < 2) menu_selection++;
    else menu_selection = 0;
    Menu_Draw();
    BUZZER_ON(); HAL_Delay(50); BUZZER_OFF();
  }
  
  if(Touch_Once()){
    BUZZER_ON(); HAL_Delay(100); BUZZER_OFF();
    if(menu_selection == 0){
      app_state = APP_DINO;
      Dino_Init();
    } else if(menu_selection == 1){
      app_state = APP_BRICK;
      Brick_Init();
    } else {
      app_state = APP_BLOCKS;
      Block_Init();
    }
  }
}

/* ===========================================================================
   DINO GAME (Compact)
   =========================================================================== */
static void Dino_Init(void){
  rng_seed^=HAL_GetTick(); RNG_Next();
  dino_score=0; last_speed_score=0;
  speed_fp=INIT_SPEED_FP; scroll_accum=0;
  world_px=0; ground_offset=0;
  next_obs_world=OBS_SPAWN_GAP_MIN+(uint32_t)(RNG_Next()%OBS_SPAWN_GAP_RNG);
  dino_y=DINO_GROUND_Y; dino_vy_fp=0;
  dino_on_ground=1; dino_frame=0; dino_frame_cnt=0;
  for(int i=0;i<MAX_OBS;i++) obs[i].active=0;
  clouds[0]=(Cloud){50, HUD_H+6,  1};
  clouds[1]=(Cloud){100,HUD_H+16, 1};
  beep_timer = HAL_GetTick(); beep_state = 0; beep_count = 0;
  LED_SetColor(0, 1, 0);
  Update_ColorScheme();
  TFT_FillScreen(COLOR_BG);
  dino_state = GS_PLAYING;
}

static void Dino_Tick(void){
  Update_ColorScheme();
  
  if(Touch_Once()&&dino_on_ground){ 
    dino_vy_fp=JUMP_VY_INIT; dino_on_ground=0;
    LED_SetColor(0, 0, 1);
  }
  
  scroll_accum+=speed_fp;
  uint8_t dx=(uint8_t)(scroll_accum>>FP_SHIFT);
  scroll_accum&=(1<<FP_SHIFT)-1;
  world_px+=dx;
  dino_score=world_px/6u;
  
  if(dino_score>=last_speed_score+(uint32_t)SPEED_INC_SCORE){
    last_speed_score=dino_score;
    if(speed_fp<MAX_SPEED_FP) speed_fp++;
  }
  
  // HUD
  TFT_FillRect(0,0,TFT_WIDTH,HUD_H,COLOR_BG);
  TFT_DrawNumber(62,2,dino_score,5,COLOR_SCORE);
  TFT_FillRect(0,HUD_H-1,TFT_WIDTH,1,COLOR_GROUND);
  
  // Dino physics
  int16_t oy=dino_y;
  if(!dino_on_ground){
    dino_vy_fp+=GRAVITY;
    dino_y+=(int16_t)(dino_vy_fp>>FP_SHIFT);
    if(dino_y>=DINO_GROUND_Y){ 
      dino_y=DINO_GROUND_Y; dino_vy_fp=0; dino_on_ground=1;
      LED_SetColor(0, 1, 0);
    }
  }
  
  if(dino_y<oy) TFT_FillRect(DINO_X,oy,DINO_W,DINO_H,COLOR_BG);
  else if(dino_y>oy) TFT_FillRect(DINO_X,oy,DINO_W,(int16_t)(dino_y-oy),COLOR_BG);
  
  if(dino_on_ground&&++dino_frame_cnt>=6){ dino_frame_cnt=0; dino_frame^=1; }
  Spr16_Draw(DINO_X,dino_y,dino_on_ground?(dino_frame?dinoB:dinoA):dinoA,DINO_W,DINO_H,COLOR_DINO,COLOR_BG);
  
  ground_offset+=dx;
  TFT_FillRect(0,GROUND_Y,TFT_WIDTH,2,COLOR_GROUND);
  
  // Obstacles
  if(world_px>=next_obs_world){
    for(int i=0;i<MAX_OBS;i++) if(!obs[i].active){
      obs[i].active=1;
      obs[i].large=(uint8_t)((RNG_Next()%3u==0)?1:0);
      if(obs[i].large){ obs[i].w=OBS_W_LARGE; obs[i].h=OBS_H_LARGE; obs[i].y=(int16_t)(GROUND_Y-OBS_H_LARGE); }
      else { obs[i].w=OBS_W_SMALL; obs[i].h=OBS_H_SMALL; obs[i].y=(int16_t)(GROUND_Y-OBS_H_SMALL); }
      obs[i].x=TFT_WIDTH;
      next_obs_world=world_px+OBS_SPAWN_GAP_MIN+(uint32_t)(RNG_Next()%OBS_SPAWN_GAP_RNG);
      break;
    }
  }
  
  for(int i=0;i<MAX_OBS;i++){
    if(!obs[i].active) continue;
    TFT_FillRect(obs[i].x,obs[i].y,obs[i].w,obs[i].h,COLOR_BG);
    obs[i].x-=(int16_t)dx;
    if(obs[i].x+(int16_t)obs[i].w<0){ obs[i].active=0; continue; }
    if(obs[i].large) Spr16_Draw(obs[i].x,obs[i].y,cactusL,OBS_W_LARGE,OBS_H_LARGE,COLOR_OBSTACLE,COLOR_BG);
    else Spr8_Draw(obs[i].x,obs[i].y,cactusS,OBS_W_SMALL,OBS_H_SMALL,COLOR_OBSTACLE,COLOR_BG);
    
    // Collision
    int16_t ax=DINO_X+3,ay=dino_y+3,aw=DINO_W-6,ah=DINO_H-6;
    int16_t bx=obs[i].x+3,by=obs[i].y+3,bw=obs[i].w-6,bh=obs[i].h-6;
    if(!(ax+aw<=bx||bx+bw<=ax||ay+ah<=by||by+bh<=ay)){
      if(dino_score>dino_hi_score) dino_hi_score=dino_score;
      dino_state=GS_GAMEOVER;
      return;
    }
  }
  
  Buzzer_Update();
}

static void Dino_GameOver(void){
  BUZZER_OFF();
  LED_SetColor(1, 0, 0);
  Spr16_Draw(DINO_X,dino_y,dinoDead,DINO_W,DINO_H,COLOR_DINO,COLOR_BG);
  
  for(int f=0;f<3;f++){
    TFT_FillRect(0,HUD_H,TFT_WIDTH,TFT_HEIGHT-HUD_H,COLOR_FLASH); HAL_Delay(90);
    TFT_FillRect(0,HUD_H,TFT_WIDTH,TFT_HEIGHT-HUD_H,COLOR_BG); HAL_Delay(90);
  }
  
  TFT_FillRect(20,60,88,40,COLOR_GROUND);
  TFT_FillRect(22,62,84,36,COLOR_BG);
  TFT_DrawNumber(30,68,dino_score,5,COLOR_SCORE);
  TFT_DrawNumber(30,82,dino_hi_score,5,COLOR_SCORE);
  
  for(int b=0; b<3; b++){
    BUZZER_ON(); HAL_Delay(1500); BUZZER_OFF();
    if(b < 2) HAL_Delay(250);
  }
  
  touch_prev=0;
  while(!Touch_Once()) HAL_Delay(20);
  HAL_Delay(200);
  
  app_state = APP_MENU;
  Menu_Init();
}

/* ===========================================================================
   BRICK BREAKER GAME
   =========================================================================== */
static void Brick_Init(void){
  paddle_x = (TFT_WIDTH - PADDLE_W) / 2;
  ball_x = TFT_WIDTH / 2;
  ball_y = PADDLE_Y - 10;
  ball_vx = 2;
  ball_vy = -3;
  brick_score = 0;
  bricks_remaining = 0;
  
  for(int r=0; r<BRICK_ROWS; r++){
    for(int c=0; c<BRICK_COLS; c++){
      bricks[r][c].active = 1;
      bricks_remaining++;
    }
  }
  
  beep_timer = HAL_GetTick(); beep_state = 0; beep_count = 0;
  LED_SetColor(0, 1, 0);
  // Set default dark mode for brick game
  COLOR_BG = COLOR_BG_DARK;
  COLOR_SCORE = COLOR_SCORE_DARK;
  TFT_FillScreen(COLOR_BG);
  brick_state = GS_PLAYING;
}

static void Brick_Tick(void){
  // No ambient light update in brick game
  
  // Paddle control
  int16_t old_paddle_x = paddle_x;
  if(SW4_PRESSED && paddle_x > 0) paddle_x -= 3;
  if(SW2_PRESSED && paddle_x < TFT_WIDTH - PADDLE_W) paddle_x += 3;
  
  if(old_paddle_x != paddle_x){
    TFT_FillRect(old_paddle_x, PADDLE_Y, PADDLE_W, PADDLE_H, COLOR_BG);
  }
  TFT_FillRect(paddle_x, PADDLE_Y, PADDLE_W, PADDLE_H, COLOR_PADDLE);
  
  // Ball physics
  int16_t old_ball_x = ball_x;
  int16_t old_ball_y = ball_y;
  
  ball_x += ball_vx;
  ball_y += ball_vy;
  
  // Wall collision
  if(ball_x <= 0 || ball_x >= TFT_WIDTH - BALL_SIZE){
    ball_vx = -ball_vx;
    BUZZER_ON(); HAL_Delay(20); BUZZER_OFF();
  }
  if(ball_y <= 0){
    ball_vy = -ball_vy;
    BUZZER_ON(); HAL_Delay(20); BUZZER_OFF();
  }
  
  // Paddle collision
  if(ball_y + BALL_SIZE >= PADDLE_Y && ball_y <= PADDLE_Y + PADDLE_H){
    if(ball_x + BALL_SIZE >= paddle_x && ball_x <= paddle_x + PADDLE_W){
      ball_vy = -ball_vy;
      ball_y = PADDLE_Y - BALL_SIZE;
      BUZZER_ON(); HAL_Delay(30); BUZZER_OFF();
    }
  }
  
  // Brick collision
  for(int r=0; r<BRICK_ROWS; r++){
    for(int c=0; c<BRICK_COLS; c++){
      if(!bricks[r][c].active) continue;
      
      int16_t bx = c * (BRICK_W + 2) + 2;
      int16_t by = BRICK_START_Y + r * (BRICK_H + 2);
      
      if(ball_x + BALL_SIZE >= bx && ball_x <= bx + BRICK_W &&
         ball_y + BALL_SIZE >= by && ball_y <= by + BRICK_H){
        bricks[r][c].active = 0;
        bricks_remaining--;
        brick_score += 10;
        ball_vy = -ball_vy;
        TFT_FillRect(bx, by, BRICK_W, BRICK_H, COLOR_BG);
        BUZZER_ON(); HAL_Delay(50); BUZZER_OFF();
        LED_SetColor(0, 0, 1); HAL_Delay(50); LED_SetColor(0, 1, 0);
        break;
      }
    }
  }
  
  // Game over (ball fell)
  if(ball_y >= TFT_HEIGHT){
    if(brick_score > brick_hi_score) brick_hi_score = brick_score;
    brick_state = GS_GAMEOVER;
    return;
  }
  
  // Win condition
  if(bricks_remaining == 0){
    if(brick_score > brick_hi_score) brick_hi_score = brick_score;
    brick_state = GS_GAMEOVER;
    return;
  }
  
  // Clear old ball
  TFT_FillRect(old_ball_x, old_ball_y, BALL_SIZE, BALL_SIZE, COLOR_BG);
  
  // Draw ball
  TFT_FillRect(ball_x, ball_y, BALL_SIZE, BALL_SIZE, COLOR_BALL);
  
  // Draw bricks
  for(int r=0; r<BRICK_ROWS; r++){
    for(int c=0; c<BRICK_COLS; c++){
      if(bricks[r][c].active){
        int16_t bx = c * (BRICK_W + 2) + 2;
        int16_t by = BRICK_START_Y + r * (BRICK_H + 2);
        TFT_FillRect(bx, by, BRICK_W, BRICK_H, COLOR_BRICK);
      }
    }
  }
  
  // Score
  TFT_FillRect(0, 0, TFT_WIDTH, 15, COLOR_BG);
  TFT_DrawNumber(10, 2, brick_score, 4, COLOR_SCORE);
  
  Buzzer_Update();
}

static void Brick_GameOver(void){
  BUZZER_OFF();
  LED_SetColor(1, 0, 0);
  
  for(int f=0;f<3;f++){
    TFT_FillRect(0,20,TFT_WIDTH,TFT_HEIGHT-20,COLOR_FLASH); HAL_Delay(90);
    TFT_FillRect(0,20,TFT_WIDTH,TFT_HEIGHT-20,COLOR_BG); HAL_Delay(90);
  }
  
  TFT_FillRect(20,60,88,40,COLOR_GROUND);
  TFT_FillRect(22,62,84,36,COLOR_BG);
  TFT_DrawNumber(30,68,brick_score,4,COLOR_SCORE);
  TFT_DrawNumber(30,82,brick_hi_score,4,COLOR_SCORE);
  
  for(int b=0; b<3; b++){
    BUZZER_ON(); HAL_Delay(1500); BUZZER_OFF();
    if(b < 2) HAL_Delay(250);
  }
  
  touch_prev=0;
  while(!Touch_Once()) HAL_Delay(20);
  HAL_Delay(200);
  
  app_state = APP_MENU;
  Menu_Init();
}

/* ===========================================================================
   BLOCK GAME (Tetris-like)
   =========================================================================== */
static uint8_t Block_CheckCollision(int8_t x, int8_t y, uint8_t rot){
  for(int r=0; r<4; r++){
    for(int c=0; c<4; c++){
      if(curr_block[r][c]){
        int8_t gx = x + c;
        int8_t gy = y + r;
        if(gx < 0 || gx >= GRID_W || gy >= GRID_H) return 1;
        if(gy >= 0 && grid[gy][gx]) return 1;
      }
    }
  }
  return 0;
}

static void Block_LockPiece(void){
  for(int r=0; r<4; r++){
    for(int c=0; c<4; c++){
      if(curr_block[r][c]){
        int8_t gx = curr_x + c;
        int8_t gy = curr_y + r;
        if(gy >= 0 && gy < GRID_H && gx >= 0 && gx < GRID_W){
          grid[gy][gx] = curr_shape + 1;
        }
      }
    }
  }
}

static void Block_ClearLines(void){
  for(int r=GRID_H-1; r>=0; r--){
    uint8_t full = 1;
    for(int c=0; c<GRID_W; c++){
      if(!grid[r][c]){ full = 0; break; }
    }
    if(full){
      // Clear line
      for(int rr=r; rr>0; rr--){
        for(int c=0; c<GRID_W; c++){
          grid[rr][c] = grid[rr-1][c];
        }
      }
      for(int c=0; c<GRID_W; c++) grid[0][c] = 0;
      block_score += 100;
      r++; // Check same row again
      BUZZER_ON(); HAL_Delay(100); BUZZER_OFF();
      LED_SetColor(1, 1, 0); HAL_Delay(50); LED_SetColor(0, 1, 0);
    }
  }
}

static void Block_NewPiece(void){
  curr_shape = (uint8_t)(RNG_Next() % 7);
  curr_rot = 0;
  curr_x = GRID_W / 2 - 2;
  curr_y = -1;
  drop_timer = 0;
  
  // Copy shape to curr_block
  for(int r=0; r<4; r++){
    for(int c=0; c<4; c++){
      curr_block[r][c] = SHAPES[curr_shape][r][c];
    }
  }
  
  // Check collision
  uint8_t collision = 0;
  for(int r=0; r<4; r++){
    for(int c=0; c<4; c++){
      if(curr_block[r][c]){
        int8_t gx = curr_x + c;
        int8_t gy = curr_y + r;
        if(gx < 0 || gx >= GRID_W || gy >= GRID_H){
          collision = 1;
          break;
        }
        if(gy >= 0 && grid[gy][gx]){
          collision = 1;
          break;
        }
      }
    }
    if(collision) break;
  }
  
  if(collision){
    if(block_score > block_hi_score) block_hi_score = block_score;
    block_state = GS_GAMEOVER;
  }
}

static void Block_RotatePiece(void){
  // Clear old piece before rotation
  for(int r=0; r<4; r++){
    for(int c=0; c<4; c++){
      if(SHAPES[curr_shape][r][c]){
        int8_t gx = curr_x + c;
        int8_t gy = curr_y + r;
        if(gy >= 0 && gy < GRID_H && gx >= 0 && gx < GRID_W){
          if(!grid[gy][gx]){
            TFT_FillRect(GRID_X + gx*BLOCK_SIZE, GRID_Y + gy*BLOCK_SIZE, BLOCK_SIZE-1, BLOCK_SIZE-1, COLOR_BG);
          }
        }
      }
    }
  }
  
  // Create rotated shape (90 degrees clockwise)
  uint8_t rotated[4][4];
  for(int r=0; r<4; r++){
    for(int c=0; c<4; c++){
      rotated[r][c] = SHAPES[curr_shape][3-c][r];
    }
  }
  
  // Temporarily store current shape
  uint8_t temp[4][4];
  for(int r=0; r<4; r++){
    for(int c=0; c<4; c++){
      temp[r][c] = curr_block[r][c];
      curr_block[r][c] = rotated[r][c];
    }
  }
  
  // Check collision with rotated shape
  uint8_t collision = 0;
  for(int r=0; r<4; r++){
    for(int c=0; c<4; c++){
      if(curr_block[r][c]){
        int8_t gx = curr_x + c;
        int8_t gy = curr_y + r;
        if(gx < 0 || gx >= GRID_W || gy >= GRID_H){
          collision = 1;
          break;
        }
        if(gy >= 0 && grid[gy][gx]){
          collision = 1;
          break;
        }
      }
    }
    if(collision) break;
  }
  
  if(collision){
    // Revert rotation
    for(int r=0; r<4; r++){
      for(int c=0; c<4; c++){
        curr_block[r][c] = temp[r][c];
      }
    }
  } else {
    BUZZER_ON(); HAL_Delay(30); BUZZER_OFF();
  }
}

static void Block_Init(void){
  rng_seed ^= HAL_GetTick(); RNG_Next();
  block_score = 0;
  drop_delay = 15; // Frames before drop (faster: was 30)
  
  // Clear grid
  for(int r=0; r<GRID_H; r++){
    for(int c=0; c<GRID_W; c++){
      grid[r][c] = 0;
    }
  }
  
  beep_timer = HAL_GetTick(); beep_state = 0; beep_count = 0;
  LED_SetColor(0, 1, 0);
  COLOR_BG = COLOR_BG_DARK;
  COLOR_SCORE = COLOR_SCORE_DARK;
  TFT_FillScreen(COLOR_BG);
  
  // Draw grid border once
  TFT_FillRect(GRID_X-1, GRID_Y-1, GRID_W*BLOCK_SIZE+2, GRID_H*BLOCK_SIZE+2, 0xFFFF);
  TFT_FillRect(GRID_X, GRID_Y, GRID_W*BLOCK_SIZE, GRID_H*BLOCK_SIZE, COLOR_BG);
  
  Block_NewPiece();
  block_state = GS_PLAYING;
}

static void Block_Tick(void){
  static int8_t old_x = 0, old_y = 0;
  static uint8_t first_draw = 1;
  
  // Controls
  if(Button_Once(&sw4_prev, SW4_PRESSED)){
    if(!Block_CheckCollision(curr_x - 1, curr_y, curr_rot)){
      curr_x--;
      BUZZER_ON(); HAL_Delay(20); BUZZER_OFF();
    }
  }
  
  if(Button_Once(&sw2_prev, SW2_PRESSED)){
    if(!Block_CheckCollision(curr_x + 1, curr_y, curr_rot)){
      curr_x++;
      BUZZER_ON(); HAL_Delay(20); BUZZER_OFF();
    }
  }
  
  if(Button_Once(&sw1_prev, SW1_PRESSED)){
    Block_RotatePiece();
  }
  
  // Drop logic
  drop_timer++;
  if(drop_timer >= drop_delay){
    drop_timer = 0;
    if(!Block_CheckCollision(curr_x, curr_y + 1, curr_rot)){
      curr_y++;
    } else {
      // Lock piece
      Block_LockPiece();
      Block_ClearLines();
      
      // Redraw entire grid after locking
      TFT_FillRect(GRID_X, GRID_Y, GRID_W*BLOCK_SIZE, GRID_H*BLOCK_SIZE, COLOR_BG);
      for(int r=0; r<GRID_H; r++){
        for(int c=0; c<GRID_W; c++){
          if(grid[r][c]){
            uint16_t col = BLOCK_COL[(grid[r][c]-1) % 7];
            TFT_FillRect(GRID_X + c*BLOCK_SIZE, GRID_Y + r*BLOCK_SIZE, BLOCK_SIZE-1, BLOCK_SIZE-1, col);
          }
        }
      }
      
      Block_NewPiece();
      first_draw = 1;
    }
  }
  
  // Clear old piece position only if it moved
  if(!first_draw && (old_x != curr_x || old_y != curr_y)){
    for(int r=0; r<4; r++){
      for(int c=0; c<4; c++){
        if(curr_block[r][c]){
          int8_t gx = old_x + c;
          int8_t gy = old_y + r;
          if(gy >= 0 && gy < GRID_H && gx >= 0 && gx < GRID_W){
            // Check if this position is not part of locked grid
            if(!grid[gy][gx]){
              TFT_FillRect(GRID_X + gx*BLOCK_SIZE, GRID_Y + gy*BLOCK_SIZE, BLOCK_SIZE-1, BLOCK_SIZE-1, COLOR_BG);
            }
          }
        }
      }
    }
  }
  
  // Draw current piece at new position
  for(int r=0; r<4; r++){
    for(int c=0; c<4; c++){
      if(curr_block[r][c]){
        int8_t gx = curr_x + c;
        int8_t gy = curr_y + r;
        if(gy >= 0 && gy < GRID_H && gx >= 0 && gx < GRID_W){
          uint16_t col = BLOCK_COL[curr_shape];
          TFT_FillRect(GRID_X + gx*BLOCK_SIZE, GRID_Y + gy*BLOCK_SIZE, BLOCK_SIZE-1, BLOCK_SIZE-1, col);
        }
      }
    }
  }
  
  old_x = curr_x;
  old_y = curr_y;
  first_draw = 0;
  
  // Score (only update if changed)
  static uint32_t last_score = 0;
  if(block_score != last_score){
    TFT_FillRect(0, 0, 50, 15, COLOR_BG);
    TFT_DrawNumber(5, 5, block_score, 5, COLOR_SCORE);
    last_score = block_score;
  }
  
  Buzzer_Update();
}

static void Block_GameOver(void){
  BUZZER_OFF();
  LED_SetColor(1, 0, 0);
  
  for(int f=0;f<3;f++){
    TFT_FillRect(0,20,TFT_WIDTH,TFT_HEIGHT-20,COLOR_FLASH); HAL_Delay(90);
    TFT_FillRect(0,20,TFT_WIDTH,TFT_HEIGHT-20,COLOR_BG); HAL_Delay(90);
  }
  
  TFT_FillRect(20,60,88,40,COLOR_GROUND);
  TFT_FillRect(22,62,84,36,COLOR_BG);
  TFT_DrawNumber(30,68,block_score,5,COLOR_SCORE);
  TFT_DrawNumber(30,82,block_hi_score,5,COLOR_SCORE);
  
  for(int b=0; b<3; b++){
    BUZZER_ON(); HAL_Delay(1500); BUZZER_OFF();
    if(b < 2) HAL_Delay(250);
  }
  
  touch_prev=0;
  while(!Touch_Once()) HAL_Delay(20);
  HAL_Delay(200);
  
  app_state = APP_MENU;
  Menu_Init();
}

/* ===========================================================================
   MAIN
   =========================================================================== */
int main(void){
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_SPI1_Init();
  TFT_Init();
  
  LED_AllOff();
  BUZZER_OFF();
  Update_ColorScheme();
  
  dino_hi_score = 0;
  brick_hi_score = 0;
  block_hi_score = 0;
  
  Menu_Init();
  
  uint32_t lt=HAL_GetTick();
  while(1){
    if(app_state == APP_MENU){
      Menu_Update();
      HAL_Delay(50);
    }
    else if(app_state == APP_DINO){
      if(dino_state == GS_PLAYING){
        if(HAL_GetTick()-lt>=TICK_MS){ lt=HAL_GetTick(); Dino_Tick(); }
      } else {
        Dino_GameOver();
      }
    }
    else if(app_state == APP_BRICK){
      if(brick_state == GS_PLAYING){
        if(HAL_GetTick()-lt>=TICK_MS){ lt=HAL_GetTick(); Brick_Tick(); }
      } else {
        Brick_GameOver();
      }
    }
    else if(app_state == APP_BLOCKS){
      if(block_state == GS_PLAYING){
        if(HAL_GetTick()-lt>=TICK_MS){ lt=HAL_GetTick(); Block_Tick(); }
      } else {
        Block_GameOver();
      }
    }
  }
}

/* ===========================================================================
   SYSTEM CLOCK — STM32F401 @ 84 MHz
   =========================================================================== */
void SystemClock_Config(void){
  RCC_OscInitTypeDef osc={0};
  RCC_ClkInitTypeDef clk={0};
  
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  
  osc.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  osc.HSIState            = RCC_HSI_ON;
  osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  osc.PLL.PLLState        = RCC_PLL_ON;
  osc.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
  osc.PLL.PLLM            = 8;
  osc.PLL.PLLN            = 168;
  osc.PLL.PLLP            = RCC_PLLP_DIV4;
  osc.PLL.PLLQ            = 7;
  if(HAL_RCC_OscConfig(&osc)!=HAL_OK) Error_Handler();
  
  clk.ClockType      = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  clk.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  clk.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  clk.APB1CLKDivider = RCC_HCLK_DIV2;
  clk.APB2CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&clk,FLASH_LATENCY_2)!=HAL_OK) Error_Handler();
}

/* ===========================================================================
   SPI1 — APB2=84 MHz / 8 = 10.5 MHz
   =========================================================================== */
static void MX_SPI1_Init(void){
  hspi1.Instance               = SPI1;
  hspi1.Init.Mode              = SPI_MODE_MASTER;
  hspi1.Init.Direction         = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize          = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity       = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase          = SPI_PHASE_1EDGE;
  hspi1.Init.NSS               = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode            = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial     = 10;
  if(HAL_SPI_Init(&hspi1)!=HAL_OK) Error_Handler();
}

/* ===========================================================================
   GPIO
   =========================================================================== */
static void MX_GPIO_Init(void){
  GPIO_InitTypeDef g={0};
  
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET);
  
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1|GPIO_PIN_4,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0|GPIO_PIN_10,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,GPIO_PIN_RESET);
  
  g.Pin=GPIO_PIN_9; g.Mode=GPIO_MODE_OUTPUT_PP;
  g.Pull=GPIO_NOPULL; g.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA,&g);
  
  g.Pin=GPIO_PIN_6; HAL_GPIO_Init(GPIOB,&g);
  g.Pin=GPIO_PIN_7; HAL_GPIO_Init(GPIOC,&g);
  
  g.Pin=GPIO_PIN_1|GPIO_PIN_4; 
  g.Mode=GPIO_MODE_OUTPUT_PP;
  g.Pull=GPIO_NOPULL; 
  g.Speed=GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA,&g);
  
  g.Pin=GPIO_PIN_0|GPIO_PIN_10;
  HAL_GPIO_Init(GPIOB,&g);
  
  g.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  HAL_GPIO_Init(GPIOC,&g);
  
  g.Pin=GPIO_PIN_1; g.Mode=GPIO_MODE_INPUT; g.Pull=GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB,&g);
  
  g.Pin=GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_8; g.Mode=GPIO_MODE_INPUT; g.Pull=GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC,&g);
  
  g.Pin=GPIO_PIN_8|GPIO_PIN_9; g.Mode=GPIO_MODE_INPUT; g.Pull=GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB,&g);
}

void Error_Handler(void){ 
  __disable_irq(); 
  while(1){} 
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t*f,uint32_t l){
  (void)f;
  (void)l;
}
#endif
