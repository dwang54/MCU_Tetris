#ifndef DEFINES_H
#define DEFINES_H

#include <stdint.h>
#include <time.h>
#include "stm32f0xx.h"
#include "fifo.h"
#include "tty.h"
#include "commands.h"


// From sdcard.c
void init_usart5();
void init_spi1_slow();
void enable_sdcard();
void disable_sdcard();
void init_sdcard_io();
void sdcard_io_high_speed();
void enable_tty_interrupt(void);
char interrupt_getchar();
int __io_putchar(int c);
int __io_getchar(void);
void USART3_8_IRQHandler();
void init_lcd_spi();


// From gameloop.c 
typedef enum _DIRECTION {
    LEFT,
    RIGHT,
    DOWN
} DIRECTION;

typedef enum _ROTATION {
    CLOCKWISE,
    COUNTERCLOCKWISE
} ROTATION;


void init_game();
void draw_gridlines();
void rotate_clockwise(uint8_t mat[4][4], uint8_t out[4][4]);
void rotate_counterclockwise(uint8_t mat[4][4], uint8_t out[4][4]);
void draw_block(int x, int y, uint16_t color);  // good
void game_loop();
int clear_lines();
void spawn_piece();                             // good
void erase_piece();                             // good
void move_left();                               // good
void rotate_current(int clockwise);             // good
void drop_piece();



// from keypad.c
void init_keypad();

// from lcd.c
typedef uint8_t u8;
typedef uint16_t u16;

void LCD_Setup();
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 c);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 c);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 c);

// from commands.c
void lcd_init(int argc, char *argv[]);

#endif