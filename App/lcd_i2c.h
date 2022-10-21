#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "stm32f10x.h"

/********** COMMANDS************/
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

/***** Flags for Display ON/OFF control ******/
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

/***** Flags for BACKLIGHT control *****/
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define LCD_RS_SET	0x01
#define LCD_RS_CLEAR	0x00

#define LCD_EN_SET	0x04
#define LCD_EN_CLEAR	0x00

#define LCD_ADDR_DEFAULT 0x27

typedef struct
{
	I2C_TypeDef *lcd_i2c;
	uint8_t			lcd_add;
}LCD_TypeDef;

/*------------------ LCD FUNCTIONS ----------------------*/
static void lcd_send_cmd(LCD_TypeDef *LCDx, uint8_t cmd);
static void lcd_send_data(LCD_TypeDef *LCDx, uint8_t data);
void lcd_putchar(LCD_TypeDef *LCDx, uint8_t data);
void lcd_clear_display(LCD_TypeDef *LCDx);
void lcd_display_off(LCD_TypeDef *LCDx);
void lcd_display_on(LCD_TypeDef *LCDx);
void lcd_backlight_on(LCD_TypeDef *LCDx);
void lcd_backlight_off(LCD_TypeDef *LCDx);
void lcd_set_cursor(LCD_TypeDef *LCDx, uint8_t row, uint8_t col);
void lcd_set_cursor_on(LCD_TypeDef *LCDx);
void lcd_set_cursor_off(LCD_TypeDef *LCDx);
void lcd_set_cursor_blink_on(LCD_TypeDef *LCDx);
void lcd_set_cursor_blink_off(LCD_TypeDef *LCDx);
void lcd_create_char(LCD_TypeDef *LCDx, uint8_t location, uint8_t charmap[]);
void lcd_write(LCD_TypeDef *LCDx, uint8_t location);
void lcd_printf(LCD_TypeDef *LCDx, const char* str, ...);
void lcd_init(LCD_TypeDef *LCDx, I2C_TypeDef *I2Cx, uint8_t lcd_add);

#endif
