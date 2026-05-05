// File: lcd.h
// Author: Camden Downey

#ifndef LCD_H
#define LCD_H

#include <xc.h>
#include <stdint.h>

// ----------- Macros for ease of dev/reading -----------
#define LCD_CS_LOW()      LATBbits.LATB5 = 0
#define LCD_CS_HIGH()     LATBbits.LATB5 = 1
#define LCD_RST_LOW()     LATBbits.LATB6 = 0
#define LCD_RST_HIGH()    LATBbits.LATB6 = 1
#define LCD_DC_CMD()      LATCbits.LATC10 = 0
#define LCD_DC_DATA()     LATCbits.LATC10 = 1

#define LCD_WIDTH         480
#define LCD_HEIGHT        320

#define FONT_WIDTH        6
#define FONT_HEIGHT       8

// ----------- Color definitions -----------
#define COLOR_BLACK       0x0000
#define COLOR_WHITE       0xFFFF
#define COLOR_RED         0xF800
#define COLOR_GREEN       0x07E0
#define COLOR_BLUE        0x001F
#define COLOR_YELLOW      0xFFE0
#define COLOR_CYAN        0x07FF


// ----------- System status enum -----------
typedef enum
{
    STATUS_OK,
    STATUS_HOMING,
    STATUS_STOPPED
} SystemStatus;

// ----------- Function prototypes -----------
void LCD_send(uint8_t cmd, const uint8_t *data, uint8_t len);
void LCD_cmd(uint8_t cmd);
void LCD_data(uint8_t data);
void LCD_data_buffer(const uint8_t *data, int length);
void LCD_setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void LCD_drawPixel(uint16_t x, uint16_t y, uint16_t color);
void LCD_fillScreen(uint16_t color);
void LCD_drawBlock(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void LCD_drawChar(uint16_t x, uint16_t y, char c, uint16_t fg, uint16_t bg, uint8_t scale);
void LCD_drawString(uint16_t x, uint16_t y, const char *str, uint16_t fg, uint16_t bg, uint8_t scale);
void LCD_drawStringLine(uint16_t x, uint16_t y, const char *str, uint16_t fg, uint16_t bg, uint8_t scale);
void LCD_drawStringCentered(uint16_t y, const char *str, uint16_t fg, uint16_t bg, uint8_t scale);
void LCD_updateStatus(SystemStatus status, float az_deg, float el_deg);
void ST7796_init(void);

#endif