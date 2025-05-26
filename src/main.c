/**
  ******************************************************************************
  * @file    main.c
  * @author  Daniel Wang
  * @date    May 26, 2025
  * @brief   main.c
  ******************************************************************************
*/

#include <stdio.h>
#include "defines.h"


void internal_clock();


int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    command_shell();
}

