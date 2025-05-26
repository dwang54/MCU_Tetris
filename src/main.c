/**
  ******************************************************************************
  * @file    main.c
  * @author  Daniel Wang
  * @date    Mar 3, 2024
  * @brief   ECE 362 Lab 7 student template
  ******************************************************************************
*/

/*******************************************************************************/

// Fill out your username!  Even though we're not using an autotest, 
// it should be a habit to fill out your username in this field now.
const char* username = "wang6161";

/*******************************************************************************/ 

#include "stm32f0xx.h"
#include <stdint.h>

void internal_clock();

// Uncomment only one of the following to test each step
// #define STEP1
// #define STEP2
// #define STEP3
// #define STEP4
// # define SHELL
#define STEP5

void init_usart5() {
    // TODO
    RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC -> AHBENR |= RCC_AHBENR_GPIODEN;

    GPIOC -> MODER &= ~GPIO_MODER_MODER12;
    GPIOD -> MODER &= ~GPIO_MODER_MODER2;

    GPIOC -> MODER |= 0x2 << GPIO_MODER_MODER12_Pos;
    GPIOD -> MODER |= 0x2 << GPIO_MODER_MODER2_Pos;

    GPIOC -> AFR[1] &= ~0x000F0000;
    GPIOC -> AFR[1] |= 0x00020000;
    GPIOD -> AFR[0] &= ~0x00000F00;
    GPIOD -> AFR[0] |= 0x00000200;

    RCC -> APB1ENR |= RCC_APB1ENR_USART5EN;
    USART5 -> CR1 &= ~USART_CR1_UE;
    USART5 -> CR1 &= ~USART_CR1_M;
    USART5 -> CR2 &= ~USART_CR2_STOP;
    USART5 -> CR1 &= ~USART_CR1_PCE;
    USART5 -> CR1 &= ~USART_CR1_OVER8;
    USART5 -> BRR = 0x1A1;
    USART5 -> CR1 |= USART_CR1_TE;
    USART5 -> CR1 |= USART_CR1_RE;
    USART5 -> CR1 |= USART_CR1_UE;
    while(!((USART5 -> ISR) & USART_ISR_TEACK) && ((USART5 -> ISR) & USART_ISR_REACK));
  


}

#ifdef STEP1
int main(void){
    internal_clock();
    init_usart5();
    for(;;) {
        while (!(USART5->ISR & USART_ISR_RXNE)) { }
        char c = USART5->RDR;
        while(!(USART5->ISR & USART_ISR_TXE)) { }
        USART5->TDR = c;
    }
}
#endif

#ifdef STEP2
#include <stdio.h>

// TODO Resolve the echo and carriage-return problem

int __io_putchar(int c) {
    // TODO
    if (c == '\n') {
        while(!(USART5 -> ISR & USART_ISR_TXE));
        USART5 -> TDR = '\r'; 
    }
    while(!(USART5 -> ISR & USART_ISR_TXE));
    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
    while (!(USART5->ISR & USART_ISR_RXNE));
    char c = USART5->RDR;
    if (c == '\r') {
        c = '\n';
    }
    __io_putchar(c);
    // TODO
    return c;
}

int main() {
    internal_clock();
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

#ifdef STEP3
#include <stdio.h>
#include "fifo.h"
#include "tty.h"
int __io_putchar(int c) {
    if (c == '\n') {
        while(!(USART5 -> ISR & USART_ISR_TXE));
        USART5 -> TDR = '\r'; 
    }
    while(!(USART5 -> ISR & USART_ISR_TXE));
    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
    // TODO
    char c = line_buffer_getchar();
    return c;
}

int main() {
    internal_clock();
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

#ifdef STEP4

#include <stdio.h>
#include "fifo.h"
#include "tty.h"

// TODO DMA data structures
#define FIFOSIZE 16
char serfifo[FIFOSIZE];
int seroffset = 0;

void enable_tty_interrupt(void) {
    // TODO
    //enable clk for USART5

    USART5 -> CR1 |= USART_CR1_RXNEIE;       //raises interrupt when RDR becomes not empty
    NVIC -> ISER[0] = (1 << USART3_8_IRQn);  //sets relevant bit in NVIC
    USART5 -> CR3 |= USART_CR3_DMAR;         //enables dma for reception


    RCC -> AHBENR |= RCC_AHBENR_DMA2EN;
    DMA2 -> CSELR |= DMA2_CSELR_CH2_USART5_RX;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_EN;


    DMA2_Channel2 -> CMAR = (uint32_t) serfifo;
    DMA2_Channel2 -> CPAR = (uint32_t) &(USART5 -> RDR);
    DMA2_Channel2 -> CNDTR = FIFOSIZE;

    DMA2_Channel2 -> CCR &= ~DMA_CCR_DIR;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_MSIZE;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_PSIZE;


    DMA2_Channel2 -> CCR |= DMA_CCR_TCIE;

    
    DMA2_Channel2 -> CCR |= DMA_CCR_MINC;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_PINC;
    DMA2_Channel2 -> CCR |= DMA_CCR_CIRC;

    DMA2_Channel2 -> CCR &= ~DMA_CCR_MEM2MEM;
    DMA2_Channel2 -> CCR |= DMA_CCR_PL;
    DMA2_Channel2 -> CCR |= DMA_CCR_EN;

}

// Works like line_buffer_getchar(), but does not check or clear ORE nor wait on new characters in USART
char interrupt_getchar() {
    // TODO
    // check if input fifo contains a newline, while there is no newline, call asm volatile
    // if it does contain a newline, remove first character from fifo and return it
    while(fifo_newline(&input_fifo) == 0) {
        asm volatile ("wfi");
    }
    // Return a character from the line buffer.
    char ch = fifo_remove(&input_fifo);
    return ch;
}
    

int __io_putchar(int c) {
    // TODO copy from STEP2
    if (c == '\n') {
        while(!(USART5 -> ISR & USART_ISR_TXE));
        USART5 -> TDR = '\r'; 
    }
    while(!(USART5 -> ISR & USART_ISR_TXE));
    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
    // TODO Use interrupt_getchar() instead of line_buffer_getchar()
    char c = interrupt_getchar();
    return c;
}

// TODO Copy the content for the USART5 ISR here
// TODO Remember to look up for the proper name of the ISR function
void USART3_8_IRQHandler() {
    while (DMA2_Channel2 -> CNDTR != sizeof serfifo - seroffset) {
        if (!fifo_full(&input_fifo)) {
            insert_echo_char(serfifo[seroffset]);
        }
        seroffset = (seroffset + 1) % sizeof serfifo;

    }
}


int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    

    setbuf(stdin,0); // These turn off buffering; more efficient, but makes it hard to explain why first 1023 characters not dispalyed
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: "); // Types name but shouldn't echo the characters; USE CTRL-J to finish
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n"); // After, will type TWO instead of ONE
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

#ifdef SHELL
#include "commands.h"
#include "fifo.h"
#include "tty.h"
#include <stdio.h>
#define FIFOSIZE 16
char serfifo[FIFOSIZE];
int seroffset = 0;


void enable_tty_interrupt(void) {
    // TODO
    //enable clk for USART5

    USART5 -> CR1 |= USART_CR1_RXNEIE;       //raises interrupt when RDR becomes not empty
    NVIC -> ISER[0] = (1 << USART3_8_IRQn);  //sets relevant bit in NVIC
    USART5 -> CR3 |= USART_CR3_DMAR;         //enables dma for reception


    RCC -> AHBENR |= RCC_AHBENR_DMA2EN;
    DMA2 -> CSELR |= DMA2_CSELR_CH2_USART5_RX;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_EN;


    DMA2_Channel2 -> CMAR = (uint32_t) serfifo;
    DMA2_Channel2 -> CPAR = (uint32_t) &(USART5 -> RDR);
    DMA2_Channel2 -> CNDTR = FIFOSIZE;

    DMA2_Channel2 -> CCR &= ~DMA_CCR_DIR;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_MSIZE;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_PSIZE;


    DMA2_Channel2 -> CCR |= DMA_CCR_TCIE;

    
    DMA2_Channel2 -> CCR |= DMA_CCR_MINC;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_PINC;
    DMA2_Channel2 -> CCR |= DMA_CCR_CIRC;

    DMA2_Channel2 -> CCR &= ~DMA_CCR_MEM2MEM;
    DMA2_Channel2 -> CCR |= DMA_CCR_PL;
    DMA2_Channel2 -> CCR |= DMA_CCR_EN;

}

// Works like line_buffer_getchar(), but does not check or clear ORE nor wait on new characters in USART
char interrupt_getchar() {
    // TODO
    // check if input fifo contains a newline, while there is no newline, call asm volatile
    // if it does contain a newline, remove first character from fifo and return it
    while(fifo_newline(&input_fifo) == 0) {
        asm volatile ("wfi");
    }
    // Return a character from the line buffer.
    char ch = fifo_remove(&input_fifo);
    return ch;
}
    

int __io_putchar(int c) {
    // TODO copy from STEP2
    if (c == '\n') {
        while(!(USART5 -> ISR & USART_ISR_TXE));
        USART5 -> TDR = '\r'; 
    }
    while(!(USART5 -> ISR & USART_ISR_TXE));
    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
    // TODO Use interrupt_getchar() instead of line_buffer_getchar()
    char c = interrupt_getchar();
    return c;
}

// TODO Copy the content for the USART5 ISR here
// TODO Remember to look up for the proper name of the ISR function
void USART3_8_IRQHandler() {
    while (DMA2_Channel2 -> CNDTR != sizeof serfifo - seroffset) {
        if (!fifo_full(&input_fifo)) {
            insert_echo_char(serfifo[seroffset]);
        }
        seroffset = (seroffset + 1) % sizeof serfifo;

    }
}

int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    my_command_shell();
}
#endif

#ifdef STEP5
#include <stdio.h>
#include "fifo.h"
#include "tty.h"
#include "commands.h"

#define FIFOSIZE 16
char serfifo[FIFOSIZE];
int seroffset = 0;

void init_spi1_slow() {
    RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC -> APB2ENR |= RCC_APB2ENR_SPI1EN;

    // configures pb3,4,5 as alt func.
    GPIOB -> MODER &= ~0x00000FC0;
    GPIOB -> MODER |= 0x00000A80;
    // sets alternate function for af0 for pins 3,4,5
    GPIOB -> AFR[0] &= ~0x00FFF000;

    // clear spe bit (many bits set in cr register require spi channel to be disabled)
    SPI1 -> CR1 &= ~SPI_CR1_SPE;

    // sets baud rate as low as possible, enables master config, software slave management and internal slave select are active
    SPI1 -> CR1 |= SPI_CR1_BR | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;

    // configures for 8 bit data size
    // FIXME: try to configure for 16 bit data size
    SPI1 -> CR2 = SPI_CR2_DS;
    SPI1 -> CR2 &= ~0x0800;
    // setes FIFO reception threshold bit
    SPI1 -> CR2 |= SPI_CR2_FRXTH;
    SPI1 -> CR1 |= SPI_CR1_SPE;
}


void enable_sdcard() {
    RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;
    // sets pb2 to low
    GPIOB -> BSRR = GPIO_BSRR_BR_2;
}

void disable_sdcard() {
    RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;
    // sets pb2 to high
    GPIOB -> BSRR = GPIO_BSRR_BS_2;
}

void init_sdcard_io() {
    init_spi1_slow();
    // config. pb2 as output
    GPIOB -> MODER &= ~0x00000030;
    GPIOB -> MODER |= 0x00000010;
    disable_sdcard();
}


void sdcard_io_high_speed() {
    SPI1 -> CR1 &= ~SPI_CR1_SPE;
    // assuming fclck of 48MHz, div. by 4 to get clock rate of 12MHz
    SPI1 -> CR1 &= ~SPI_CR1_BR;
    SPI1 -> CR1 |= SPI_CR1_BR_0;
    SPI1 -> CR1 |= SPI_CR1_SPE;
}

void enable_tty_interrupt(void) {
    // TODO
    //enable clk for USART5

    USART5 -> CR1 |= USART_CR1_RXNEIE;       //raises interrupt when RDR becomes not empty
    NVIC -> ISER[0] = (1 << USART3_8_IRQn);  //sets relevant bit in NVIC
    USART5 -> CR3 |= USART_CR3_DMAR;         //enables dma for reception


    RCC -> AHBENR |= RCC_AHBENR_DMA2EN;
    DMA2 -> CSELR |= DMA2_CSELR_CH2_USART5_RX;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_EN;


    DMA2_Channel2 -> CMAR = (uint32_t) serfifo;
    DMA2_Channel2 -> CPAR = (uint32_t) &(USART5 -> RDR);
    DMA2_Channel2 -> CNDTR = FIFOSIZE;

    DMA2_Channel2 -> CCR &= ~DMA_CCR_DIR;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_MSIZE;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_PSIZE;


    DMA2_Channel2 -> CCR |= DMA_CCR_TCIE;

    
    DMA2_Channel2 -> CCR |= DMA_CCR_MINC;
    DMA2_Channel2 -> CCR &= ~DMA_CCR_PINC;
    DMA2_Channel2 -> CCR |= DMA_CCR_CIRC;

    DMA2_Channel2 -> CCR &= ~DMA_CCR_MEM2MEM;
    DMA2_Channel2 -> CCR |= DMA_CCR_PL;
    DMA2_Channel2 -> CCR |= DMA_CCR_EN;

}

// Works like line_buffer_getchar(), but does not check or clear ORE nor wait on new characters in USART
char interrupt_getchar() {
    // TODO
    // check if input fifo contains a newline, while there is no newline, call asm volatile
    // if it does contain a newline, remove first character from fifo and return it
    while(fifo_newline(&input_fifo) == 0) {
        asm volatile ("wfi");
    }
    // Return a character from the line buffer.
    char ch = fifo_remove(&input_fifo);
    return ch;
}
    

int __io_putchar(int c) {
    // TODO copy from STEP2
    if (c == '\n') {
        while(!(USART5 -> ISR & USART_ISR_TXE));
        USART5 -> TDR = '\r'; 
    }
    while(!(USART5 -> ISR & USART_ISR_TXE));
    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
    // TODO Use interrupt_getchar() instead of line_buffer_getchar()
    char c = interrupt_getchar();
    return c;
}

// TODO Copy the content for the USART5 ISR here
// TODO Remember to look up for the proper name of the ISR function
void USART3_8_IRQHandler() {
    while (DMA2_Channel2 -> CNDTR != sizeof serfifo - seroffset) {
        if (!fifo_full(&input_fifo)) {
            insert_echo_char(serfifo[seroffset]);
        }
        seroffset = (seroffset + 1) % sizeof serfifo;

    }
}

void init_lcd_spi() {
    RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;
    // configures pb 8, 11, 14 as outputs
    GPIOB -> MODER &= ~0x30C30000;
    GPIOB -> MODER |= 0x10410000;
    init_spi1_slow();
    sdcard_io_high_speed();

}

int main() {
    internal_clock();
    init_usart5();
    enable_tty_interrupt();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    command_shell();

}

#endif