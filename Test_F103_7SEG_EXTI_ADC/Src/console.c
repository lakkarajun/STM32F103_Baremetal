/*
 * console.c
 *
 *  Created on: Sep 23, 2025
 *      Author: Nagaraju Lakkaraju
 */

#include <stdio.h>
#include <stdint.h>
#include "stm32f103x.h"

/* --------------------- UART1 Initialization --------------------- */
/* USART-1 PA9 - Tx, PA10 - Rx  */
void uart1_init(void)
{
    /* 1. Enable clocks: GPIOA + USART1 */
    RCC_APB2ENR |= (1<<2) | (1<<14); // IOPAEN | USART1EN

    /* 2. Configure PA9 = TX (AF push-pull), PA10 = RX (input floating) */
    GPIOA_CRH &= ~(0xFF << 4);        // Clear CNF9+MODE9, CNF10+MODE10
    GPIOA_CRH |=  (0x0B << 4);        // PA9 = AF Push-Pull 50MHz, PA10 = Input floating

    /* 3. Configure USART1 */
    // Assuming 8MHz APB2 clock, baud rate 115200
    // USARTDIV = 8,000,000 / (16*115200) ≈ 4.340
    // Mantissa = 4, Fraction = 0.340*16≈5 → BRR = 0x45
    USART1_BRR = (4 << 4) | 5;

    USART1_CR1 = (1<<13) | (1<<3) | (1<<2); // UE=1, TE=1, RE=1
    USART1_CR2 = 0;
    USART1_CR3 = 0;
}

/* --------------------- UART1 Transmit --------------------- */
void uart1_tx(char c)
{
    while(!(USART1_SR & (1<<7))); // Wait for TXE
    USART1_DR = c;
}

/* --------------------- UART1 Receive --------------------- */
char uart1_rx(void)
{
    while(!(USART1_SR & (1<<5))); // Wait for RXNE
    return (char)(USART1_DR & 0xFF);
}

/* --------------------- Print string --------------------- */
void uart1_print(const char *str)
{
    while(*str)
        uart1_tx(*str++);
}

int _write(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++) {
    	uart1_tx(ptr[i]);
    }
    return len;
}
