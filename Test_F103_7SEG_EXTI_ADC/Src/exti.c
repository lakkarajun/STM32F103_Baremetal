/*
 * exti.c
 *
 *  Created on: Sep 23, 2025
 *      Author: Nagaraju Lakkaraju
 */

#include <stdint.h>
#include <stdio.h>
#include "stm32f103x.h"
#include "console.h"
#include "adc.h"

#define DEBUG_EXTI	1

/* Enable PA0 as External Interrupt */
void EXTI_PA0_Init(void)
{
    /* 1. Enable GPIOA clocks */
	RCC_APB2ENR |= (1 << 2); // IOPAEN

    /* 2. Configure PA0 as input with pull-down */
	GPIOA_CRL &= ~(0xF << 0);  // Clear CNF0 + MODE0
    GPIOA_CRL |= (0x8 << 0);   // CNF0 = 10 (pull-up/pull-down), MODE0 = 00 input
    GPIOA_ODR &= ~(1 << 0);    // Set pull-down

    /* 3. Enable AFIO clock for EXTI */
    RCC_APB2ENR |= (1 << 0);   // AFIOEN

    /* 4. Connect EXTI0 line to PA0 */
    AFIO_EXTICR1 &= ~(0xF << 0); // EXTI0 -> PA0

    /* 5. Configure EXTI0 */
    EXTI_IMR |= (1 << 0);       // Unmask EXTI0
    EXTI_FTSR |= (1 << 0);      // Falling edge trigger
    EXTI_PR |= (1 << 0);        // Clear pending bit just in case

    /* 6. Enable EXTI0 interrupt in NVIC */
    *NVIC_ISER0 |= (1 << EXTI0_IRQ);
}

/* GPIO PC13 - On Board LED control */
void PC13_LED_Init(void)
{
    /* 1. Enable GPIOC clocks */
	RCC_APB2ENR |= (1 << 4); // IOPCEN

    /* 2. Configure PC13 as push-pull output */
	GPIOC_CRH &= ~(0xF << 20); // Clear CNF13 + MODE13
	GPIOC_CRH |= (0x1 << 20);  // MODE13 = Output 10MHz, CNF=00 push-pull
}

/* GPIO PC13 - On Board LED Toggle */
void PC13_LED_Toggle(void)
{
	GPIOC_ODR ^= (1 << 13);  // Toggle LED
}

/* ------------------- EXTI0 ISR ------------------- */
void EXTI0_IRQHandler(void)
{
	static uint32_t exti_cnt = 0;

    if (EXTI_PR & (1 << 0)) {
        EXTI_PR |= (1 << 0);          // Clear pending bit
        GPIOC_ODR ^= (1 << 13);       // Toggle PC13 LED
        printf("%s(): EXTI (@PA0) occurred. count:%lu\r\n", __func__, ++exti_cnt);

        uint32_t value = ADC1_Read();
        if (DEBUG_EXTI)
        	printf("Button Pressed! ADC Value: %lu mV\r\n", value);
    }
}

