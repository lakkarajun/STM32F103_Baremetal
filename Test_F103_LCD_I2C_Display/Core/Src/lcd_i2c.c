/*
 * lcd_i2c.c
 *
 *  Created on: Oct 12, 2025
 *      Author: Nagaraju Lakkaraju
 */

#include <stdio.h>
#include "lcd_i2c.h"

#define DBG_LCD_I2C			0

extern I2C_HandleTypeDef hi2c1; // defined in main.c

#define SLAVE_ADDRESS_LCD 0x4E    // Adjust based on your module (0x27<<1 or 0x3F<<1)

#define LCD_BL 0x08   // P7 = backlight in many modules; adjust if needed
#define LCD_EN 0x04   // EN bit mask in the PCF mapping used here
#define LCD_RS 0x01   // RS bit mask in the PCF mapping used here

#define ROW_1	0
#define ROW_2	1

static void lcd_i2c_write(uint8_t data)
{
    HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, &data, 1, 100);
}

static void lcd_write4(uint8_t nibble, uint8_t control_mask)
{
	/* nibble: upper 4 bits should be placed onto P0..P3 (D4..D7) 
	 * control_mask: combination of LCD_RS and LCD_BL and optionally others */
	uint8_t data = (nibble & 0xF0) | control_mask;  // nibble already shifted to upper nibble
	/* EN high */
	lcd_i2c_write(data | LCD_EN);
	HAL_Delay(1); // short pulse
	/* EN low (keep other bits) */
	lcd_i2c_write(data & ~LCD_EN);
	HAL_Delay(1);
}

void lcd_send_cmd(uint8_t cmd)
{
	uint8_t upper = cmd & 0xF0;
	uint8_t lower = (cmd << 4) & 0xF0;
	uint8_t control = LCD_BL; // keep backlight on

	lcd_write4(upper, control); // RS = 0
	lcd_write4(lower, control);
}

void lcd_send_data(uint8_t data)
{
	uint8_t upper = data & 0xF0;
	uint8_t lower = (data << 4) & 0xF0;
	uint8_t control = LCD_BL | LCD_RS; // RS = 1 for data

	lcd_write4(upper, control);
	lcd_write4(lower, control);
}

void lcd_clear(void)
{
    lcd_send_cmd(0x01);
    HAL_Delay(2);
}

void lcd_put_cur(int row, int col)
{
	switch (row) {
	case ROW_1:
		lcd_send_cmd(0x80 + col);
		break;
	case ROW_2:
		lcd_send_cmd(0xC0 + col);
		break;
	}
}

void lcd_init(void)
{
	HAL_Delay(50);
	/* Wake-up sequence (4-bit) */
	lcd_i2c_write(LCD_BL);
	HAL_Delay(5);
	lcd_send_cmd(0x33);
	HAL_Delay(5);
	lcd_send_cmd(0x32); // set 4-bit mode
	HAL_Delay(5);
	lcd_send_cmd(0x28); // 2-line, 5x8 font
	lcd_send_cmd(0x0C); // display ON, cursor OFF
	lcd_send_cmd(0x06); // entry mode
	lcd_clear();
	if (DBG_LCD_I2C)
		printf("%s(): LCD Init: Done\r\n", __func__);
}

void lcd_send_string(char *str)
{
	while (*str)
		lcd_send_data(*str++);
}

void delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

void lcd_backlight_test(void)
{
	uint8_t data;
	uint8_t pattern[] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	uint8_t pattern_count = sizeof(pattern) / sizeof(pattern[0]);

	for (uint8_t i = 0; i < pattern_count; i++) {
		data = pattern[i];
		HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, &data, 1, HAL_MAX_DELAY);
		if (DBG_LCD_I2C)
			printf("Sent: 0x%02X\r\n", data);
		delay_ms(1000);
	}
	if (DBG_LCD_I2C)
		printf("%s(): LCD back light test: Done\r\n", __func__);
}
