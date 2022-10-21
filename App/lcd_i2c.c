#include "lcd_i2c.h"
#include "i2c_driver.h"
#include "SysClock.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

static uint8_t DisplayControl;

static void lcd_send_cmd(LCD_TypeDef *LCDx, uint8_t cmd)
{
	uint8_t data_h, data_l;
	uint8_t data_t[4];
	data_h = cmd & 0xF0;
	data_l = (cmd << 4) & 0xF0;
	data_t[0] = data_h | (LCD_BACKLIGHT | LCD_RS_CLEAR | LCD_EN_SET);
	data_t[1] = data_h | (LCD_BACKLIGHT | LCD_RS_CLEAR | LCD_EN_CLEAR);
	data_t[2] = data_l | (LCD_BACKLIGHT | LCD_RS_CLEAR | LCD_EN_SET);
	data_t[3] = data_l | (LCD_BACKLIGHT | LCD_RS_CLEAR | LCD_EN_CLEAR);
	I2C_WriteStr(LCDx->lcd_i2c, LCDx->lcd_add, (uint8_t *)data_t, 4);
}

static void lcd_send_data(LCD_TypeDef *LCDx, uint8_t data)
{
	uint8_t data_h, data_l;
	uint8_t data_t[4];
	data_h = data & 0xF0;
	data_l = (data << 4) & 0xF0;
	data_t[0] = data_h | (LCD_BACKLIGHT | LCD_RS_SET | LCD_EN_SET);
	data_t[1] = data_h | (LCD_BACKLIGHT | LCD_RS_SET | LCD_EN_CLEAR);
	data_t[2] = data_l | (LCD_BACKLIGHT | LCD_RS_SET | LCD_EN_SET);
	data_t[3] = data_l | (LCD_BACKLIGHT | LCD_RS_SET | LCD_EN_CLEAR);
	I2C_WriteStr(LCDx->lcd_i2c, LCDx->lcd_add, (uint8_t *)data_t, 4);
}

void lcd_putchar(LCD_TypeDef *LCDx, uint8_t data)
{
	lcd_send_data(LCDx, data);
}

void lcd_clear_display(LCD_TypeDef *LCDx)
{
	lcd_send_cmd(LCDx, LCD_CLEARDISPLAY);
	Delay_ms(2);
}

void lcd_display_off(LCD_TypeDef *LCDx)
{
	DisplayControl &= ~LCD_DISPLAYON;
	lcd_send_cmd(LCDx, LCD_DISPLAYCONTROL | DisplayControl);
}

void lcd_display_on(LCD_TypeDef *LCDx)
{
	DisplayControl |= LCD_DISPLAYON;
	lcd_send_cmd(LCDx, LCD_DISPLAYCONTROL | DisplayControl);
}

void lcd_backlight_on(LCD_TypeDef *LCDx)
{
	I2C_WriteStr(LCDx->lcd_i2c, LCDx->lcd_add, (uint8_t *)LCD_BACKLIGHT, 1);
}

void lcd_backlight_off(LCD_TypeDef *LCDx)
{
	I2C_WriteStr(LCDx->lcd_i2c, LCDx->lcd_add, (uint8_t *)LCD_NOBACKLIGHT, 1);
}

void lcd_set_cursor(LCD_TypeDef *LCDx, uint8_t row, uint8_t col)
{
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	lcd_send_cmd(LCDx, LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void lcd_set_cursor_on(LCD_TypeDef *LCDx)
{
	DisplayControl |= LCD_CURSORON;
	lcd_send_cmd(LCDx, LCD_DISPLAYCONTROL | DisplayControl);
}

void lcd_set_cursor_off(LCD_TypeDef *LCDx)
{
	DisplayControl &= ~LCD_CURSORON;
	lcd_send_cmd(LCDx, LCD_DISPLAYCONTROL | DisplayControl);
}

void lcd_set_cursor_blink_on(LCD_TypeDef *LCDx)
{
	DisplayControl |= LCD_BLINKON;
	lcd_send_cmd(LCDx, LCD_DISPLAYCONTROL | DisplayControl);
}

void lcd_set_cursor_blink_off(LCD_TypeDef *LCDx)
{
	DisplayControl &= ~LCD_BLINKON;
	lcd_send_cmd(LCDx, LCD_DISPLAYCONTROL | DisplayControl);
}

void lcd_create_char(LCD_TypeDef *LCDx, uint8_t location, uint8_t charmap[])
{
	location &= 0x7;
	lcd_send_cmd(LCDx, LCD_SETCGRAMADDR | (uint8_t)(location << 3));
	for(int i = 0; i < 8; i++)
	{
		lcd_send_data(LCDx, charmap[i]);
	}
}

void lcd_write(LCD_TypeDef *LCDx, uint8_t location)
{
	lcd_send_data(LCDx, location);
}

void lcd_printf(LCD_TypeDef *LCDx, const char* str, ...)
{
	char stringArray[20];
	
	va_list args;
	va_start(args, str);
	vsprintf(stringArray, str, args);
	va_end(args);
	
	for(uint8_t i = 0; i < strlen(stringArray) && i < 16; i++)
	{
		lcd_send_data(LCDx, (uint8_t)stringArray[i]);
	}
}

void lcd_init(LCD_TypeDef *LCDx, I2C_TypeDef *I2Cx, uint8_t lcd_add)
{
	LCDx->lcd_i2c = I2Cx;
	LCDx->lcd_add = lcd_add;
	DisplayControl |= LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	lcd_send_cmd(LCDx, 0x33);
	lcd_send_cmd(LCDx, 0x32);
	Delay_ms(5);
	lcd_send_cmd(LCDx, LCD_CLEARDISPLAY);
	Delay_ms(5);
	lcd_send_cmd(LCDx, 0x0C);
	Delay_ms(5);
	lcd_send_cmd(LCDx, LCD_RETURNHOME);
	Delay_ms(5);
	lcd_send_cmd(LCDx, LCD_SETDDRAMADDR);
	lcd_set_cursor_blink_off(LCDx);
}

