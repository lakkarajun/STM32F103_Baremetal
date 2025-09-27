/*
 * console.c
 *
 *  Created on: Sep 23, 2025
 *      Author: Nagaraju Lakkaraju
 */

#include <stdio.h>
#include <stdint.h>

/* Base addresses */
#define RCC_BASE     0x40021000
#define GPIOA_BASE   0x40010800
#define USART1_BASE  0x40013800

/* RCC registers */
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x18))

/* GPIOA registers */
#define GPIOA_CRH   (*(volatile uint32_t *)(GPIOA_BASE + 0x04))
#define GPIOA_ODR   (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))
#define GPIOA_IDR   (*(volatile uint32_t *)(GPIOA_BASE + 0x08))

/* USART1 registers */
#define USART1_SR   (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART1_DR   (*(volatile uint32_t *)(USART1_BASE + 0x04))
#define USART1_BRR  (*(volatile uint32_t *)(USART1_BASE + 0x08))
#define USART1_CR1  (*(volatile uint32_t *)(USART1_BASE + 0x0C))
#define USART1_CR2  (*(volatile uint32_t *)(USART1_BASE + 0x10))
#define USART1_CR3  (*(volatile uint32_t *)(USART1_BASE + 0x14))

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
