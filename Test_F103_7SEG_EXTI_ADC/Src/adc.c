/*
 * adc.c
 *
 *  Created on: Sep 23, 2025
 *      Author: Nagaraju Lakkaraju
 */

#include <stdio.h>
#include "stm32f103x.h"
#include "exti.h"

#define DEBUG_ADC	0

/* --- ADC1 Initialization (PA2 = ADC1_IN2) --- */
void ADC1_Init(void)
{
    /* Enable GPIOA and ADC1 clocks */
    RCC_APB2ENR |= (1 << 2);   // IOPAEN (bit 2)
    RCC_APB2ENR |= (1 << 9);   // ADC1EN (bit 9)

    /* PA2 as analog input: MODE=00, CNF=00 */
    GPIOA_CRL &= ~(0xF << (2 * 4));

    /* ADC1 configuration */
    ADC1_CR1 = 0x00000000;     // Independent mode
    ADC1_CR2 = 0x00000000;     // Reset

    ADC1_SMPR2 |= (7 << (3 * 2));   // Channel 2 sample time = 239.5 cycles
    ADC1_SQR3 = 2;                  // 1st conversion = channel 2

    ADC1_CR2 |= (1 << 1);      // Continuous conversion
    ADC1_CR2 |= (1 << 0);      // ADON: Turn on ADC

    /* Calibration */
    ADC1_CR2 |= (1 << 3);      // RSTCAL
    while (ADC1_CR2 & (1 << 3));
    ADC1_CR2 |= (1 << 2);      // CAL
    while (ADC1_CR2 & (1 << 2));

    /* Continuous mode */
    ADC1_CR2 |= (1 << 1);

    /* Start conversion */
    ADC1_CR2 |= (1 << 22);     // SWSTART

    if(DEBUG_ADC)
    	printf("%s(): ADC Init: Done\r\n", __func__);
}

/* --- ADC1 Read --- */
uint32_t ADC1_Read(void)
{
	uint16_t raw_adc;   // range: 0 - 4095
	uint32_t volt_mV;   // range (in milli volt) - 0 - 3300

    /* Start conversion */
    ADC1_CR2 |= (1 << 22);     // SWSTART
    while (!(ADC1_SR & (1 << 1)));       // Wait for end of conversion
    raw_adc = (uint16_t)ADC1_DR;         // Return 12-bit value

    volt_mV = (raw_adc * 3300UL) / 4095UL;
    if (DEBUG_ADC)
    	printf("%s(): ADC: %u, Voltage :%lu mV\r\n", __func__, raw_adc, volt_mV);

    /* Toggle on Board LED */
    PC13_LED_Toggle();

    return volt_mV;
}
