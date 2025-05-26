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

#endif