/*
 * stm32f103x.h
 *
 *  Created on: Sep 23, 2025
 *      Author: Nagaraju Lakkaraju
 */

#ifndef STM32F103X_H_
#define STM32F103X_H_

#include <stdint.h>

/* Base addresses */
#define RCC_BASE     0x40021000
#define GPIOA_BASE   0x40010800
#define GPIOB_BASE   0x40010C00
#define GPIOC_BASE   0x40011000
#define ADC1_BASE    0x40012400
#define USART1_BASE  0x40013800
#define AFIO_BASE    0x40010000
#define EXTI_BASE    0x40010400
#define NVIC_ISER0   ((volatile uint32_t*)0xE000E100)

/* Registers */
#define RCC_APB2ENR   (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB1ENR   (*(volatile uint32_t *)(RCC_BASE + 0x1C))

/* GPIO Registers */
#define GPIOA_CRL     (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_CRH     (*(volatile uint32_t *)(GPIOA_BASE + 0x04))
#define GPIOA_IDR     (*(volatile uint32_t *)(GPIOA_BASE + 0x08))
#define GPIOA_ODR     (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

#define GPIOB_CRL     (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_CRH     (*(volatile uint32_t *)(GPIOB_BASE + 0x04))
#define GPIOB_IDR     (*(volatile uint32_t *)(GPIOB_BASE + 0x08))
#define GPIOB_ODR     (*(volatile uint32_t *)(GPIOB_BASE + 0x0C))

#define GPIOC_CRH     (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_ODR     (*(volatile uint32_t *)(GPIOC_BASE + 0x0C))

/* --- ADC1 registers --- */
#define ADC1_SR            (*(volatile unsigned long *)(ADC1_BASE + 0x00))
#define ADC1_CR1           (*(volatile unsigned long *)(ADC1_BASE + 0x04))
#define ADC1_CR2           (*(volatile unsigned long *)(ADC1_BASE + 0x08))
#define ADC1_SMPR2         (*(volatile unsigned long *)(ADC1_BASE + 0x10))
#define ADC1_SQR3          (*(volatile unsigned long *)(ADC1_BASE + 0x34))
#define ADC1_DR            (*(volatile unsigned long *)(ADC1_BASE + 0x4C))

/* USART1 registers */
#define USART1_SR   (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART1_DR   (*(volatile uint32_t *)(USART1_BASE + 0x04))
#define USART1_BRR  (*(volatile uint32_t *)(USART1_BASE + 0x08))
#define USART1_CR1  (*(volatile uint32_t *)(USART1_BASE + 0x0C))
#define USART1_CR2  (*(volatile uint32_t *)(USART1_BASE + 0x10))
#define USART1_CR3  (*(volatile uint32_t *)(USART1_BASE + 0x14))

#define AFIO_EXTICR1  (*(volatile uint32_t *)(AFIO_BASE + 0x08))
#define EXTI_IMR      (*(volatile uint32_t *)(EXTI_BASE + 0x00))
#define EXTI_RTSR     (*(volatile uint32_t *)(EXTI_BASE + 0x08))
#define EXTI_FTSR     (*(volatile uint32_t *)(EXTI_BASE + 0x0C))
#define EXTI_PR       (*(volatile uint32_t *)(EXTI_BASE + 0x14))

/* NVIC IRQ number for EXTI0 */
#define EXTI0_IRQ 6


#define IOPAEN			(1 << 2)
#define IOPBEN			(1 << 3)

#endif /* STM32F103X_H_ */
