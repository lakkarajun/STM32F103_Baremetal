/*
 * seg_led.c
 *
 *  Created on: Sep 23, 2025
 *      Author: Nagaraju Lakkaraju
 */

#include <stdio.h>
#include <stdint.h>
#include "stm32f103x.h"

/* ---- 7-Segment Lookup Table ------- *
 * Segments (7 pins):
 * PA3 → Segment A
 * PA4 → Segment B
 * PA5 → Segment C
 * PA6 → Segment D
 * PA7 → Segment E
 * PB12 → Segment F
 * PB13 → Segment G
 *
 * Digit selects (4 pins):
 * PB0 → Digit 1
 * PB1 → Digit 2
 * PB10 → Digit 3
 * PB11 → Digit 4
 */
const uint8_t seg_table[10] = {
/* Common Anode 7 Segment Display */
/*----*gfedcba--*/
	0b11000000, // 0
	0b11111001, // 1
	0b10100100, // 2
	0b10110000, // 3
	0b10011001, // 4
	0b10010010, // 5
	0b10000010, // 6
	0b11111000, // 7
	0b10000000, // 8
	0b10010000  // 9
};

#define DIG_1	0
#define DIG_2	1
#define DIG_3	10
#define DIG_4	11

/* -------------------- Global Variables -------------------- */
static uint8_t digits[] = {0, 0, 0, 0};
static uint8_t current_digit = 0;
uint16_t dist = 1234;

/* -------------------- GPIO Initialization ----------------- */
void seg_led_gpio_init(void)
{
	/* Enable GPIOA and GPIOB clocks */
	RCC_APB2ENR |= (IOPAEN | IOPBEN);

    /* Configure PA3..PA7 as output 2MHz push-pull */
    GPIOA_CRL &= ~((0xF << (3 * 4)) | (0xF << (4 * 4)) | \
    		       (0xF << (5 * 4)) | (0xF << (6 * 4)) | \
				   (0xF << (7 * 4)));
    GPIOA_CRL |=  ((0x2 << (3 * 4)) | (0x2 << (4 * 4)) | \
    		       (0x2 << (5 * 4)) | (0x2 << (6 * 4)) | \
				   (0x2 << (7 * 4)));

    /* Configure PB12, PB13 as output 2MHz push-pull */
    GPIOB_CRH &= ~((0xF << ((12-8) * 4)) | (0xF << ((13-8) * 4)));
    GPIOB_CRH |=  ((0x2 << ((12-8) * 4)) | (0x2 << ((13-8) * 4)));

    /* PB0, PB1, PB10, PB11 → digit selects as output 2MHz push-pull */
    GPIOB_CRL &= ~((0xF << (DIG_1 * 4)) | (0xF << (DIG_2 * 4)));
    GPIOB_CRL |=  ((0x2 << (DIG_1 * 4)) | (0x2 << (DIG_2 * 4)));
    GPIOB_CRH &= ~((0xF << ((DIG_3 - 8) * 4)) | (0xF << ((DIG_4 - 8) * 4)));
    GPIOB_CRH |=  ((0x2 << ((DIG_3 - 8) * 4)) | (0x2 << ((DIG_4 - 8) * 4)));
}

/* --------- Digit & Segment Control -------- */
void seg_led_digit_off_all(void)
{
    GPIOB_ODR &= ~((1U << DIG_1) | (1U << DIG_2) | (1U << DIG_3) | (1U << DIG_4));
}

void seg_led_digit_on(uint8_t d)
{
	/* Turn off all digits first */
    seg_led_digit_off_all();

    /* Enable selected digit */
    switch(d) {
    case 0:
		GPIOB_ODR |= (1U << DIG_1);
		break;
    case 1:
		GPIOB_ODR |= (1U << DIG_2);
		break;
    case 2:
		GPIOB_ODR |= (1U << DIG_3);
		break;
    case 3:
		GPIOB_ODR |= (1U << DIG_4);
		break;
    }
}

void seg_led_write(uint8_t segbits)
{
	uint8_t idx;

	/* PA3..PA7 → bits 0..4 */
    for(idx = 0; idx <= 4; idx++) {
        if(segbits & (1U << idx))
			GPIOA_ODR |= (1U << (idx + 3));
        else
			GPIOA_ODR &= ~(1U << (idx + 3));
    }

    /* PB12 → bit5, PB13 → bit6 */
    if(segbits & (1U << 5))
		GPIOB_ODR |= (1U << 12);
	else
		GPIOB_ODR &= ~(1U << 12);

    if(segbits & (1U << 6))
		GPIOB_ODR |= (1U << 13);
	else
		GPIOB_ODR &= ~(1U << 13);
}

/* ------ Display Number Function -------- */
void seg_led_display_number(uint16_t num)
{
    if(num > 9999)
		num = 9999; // limit to 4 digits

    digits[0] = (num / 1000) % 10;  // thousands
    digits[1] = (num / 100) % 10;   // hundreds
    digits[2] = (num / 10) %10;     // tens
    digits[3] = num % 10;           // units
}


void seg_led_display_update(void)
{
//	if ((msTicks % 500) == 0)
//		dist = (uint16_t) ADC1_Read();

	seg_led_display_number(dist);
	seg_led_write(seg_table[digits[current_digit]]);
	seg_led_digit_on(current_digit);
}

void seg_led_digit_change(void)
{
	current_digit++;
	if(current_digit >= 4)
		current_digit = 0;

	seg_led_display_update();
}

