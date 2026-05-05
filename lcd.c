// File: lcd.c
// Author: Camden Downey

#include "spi.h"
#include "lcd.h"
#include "init.h"
#include <xc.h>
#include <string.h>
#include <stdio.h>
#include <sys/attribs.h>

// ---------------- Font Table (ASCII 0x20 to 0x7E, 6 bytes per character) ----------------
static const uint8_t font_table[][6] = 
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // ' '
    { 0x00, 0x00, 0x5F, 0x00, 0x00, 0x00 }, // '!'
    { 0x00, 0x07, 0x00, 0x07, 0x00, 0x00 }, // '"'
    { 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00 }, // '#'
    { 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00 }, // '$'
    { 0x23, 0x13, 0x08, 0x64, 0x62, 0x00 }, // '%'
    { 0x36, 0x49, 0x55, 0x22, 0x50, 0x00 }, // '&'
    { 0x00, 0x05, 0x03, 0x00, 0x00, 0x00 }, // '''
    { 0x00, 0x1C, 0x22, 0x41, 0x00, 0x00 }, // '('
    { 0x00, 0x41, 0x22, 0x1C, 0x00, 0x00 }, // ')'
    { 0x08, 0x2A, 0x1C, 0x2A, 0x08, 0x00 }, // '*'
    { 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00 }, // '+'
    { 0x00, 0x50, 0x30, 0x00, 0x00, 0x00 }, // ','
    { 0x08, 0x08, 0x08, 0x08, 0x08, 0x00 }, // '-'
    { 0x00, 0x60, 0x60, 0x00, 0x00, 0x00 }, // '.'
    { 0x20, 0x10, 0x08, 0x04, 0x02, 0x00 }, // '/'
    { 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00 }, // '0'
    { 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00 }, // '1'
    { 0x42, 0x61, 0x51, 0x49, 0x46, 0x00 }, // '2'
    { 0x21, 0x41, 0x45, 0x4B, 0x31, 0x00 }, // '3'
    { 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00 }, // '4'
    { 0x27, 0x45, 0x45, 0x45, 0x39, 0x00 }, // '5'
    { 0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00 }, // '6'
    { 0x01, 0x71, 0x09, 0x05, 0x03, 0x00 }, // '7'
    { 0x36, 0x49, 0x49, 0x49, 0x36, 0x00 }, // '8'
    { 0x06, 0x49, 0x49, 0x29, 0x1E, 0x00 }, // '9'
    { 0x00, 0x36, 0x36, 0x00, 0x00, 0x00 }, // ':'
    { 0x00, 0x56, 0x36, 0x00, 0x00, 0x00 }, // ';'
    { 0x00, 0x08, 0x14, 0x22, 0x41, 0x00 }, // '<'
    { 0x14, 0x14, 0x14, 0x14, 0x14, 0x00 }, // '='
    { 0x41, 0x22, 0x14, 0x08, 0x00, 0x00 }, // '>'
    { 0x02, 0x01, 0x51, 0x09, 0x06, 0x00 }, // '?'
    { 0x32, 0x49, 0x79, 0x41, 0x3E, 0x00 }, // '@'
    { 0x7E, 0x11, 0x11, 0x11, 0x7E, 0x00 }, // 'A'
    { 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00 }, // 'B'
    { 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00 }, // 'C'
    { 0x7F, 0x41, 0x41, 0x22, 0x1C, 0x00 }, // 'D'
    { 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00 }, // 'E'
    { 0x7F, 0x09, 0x09, 0x01, 0x01, 0x00 }, // 'F'
    { 0x3E, 0x41, 0x41, 0x51, 0x32, 0x00 }, // 'G'
    { 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00 }, // 'H'
    { 0x00, 0x41, 0x7F, 0x41, 0x00, 0x00 }, // 'I'
    { 0x20, 0x40, 0x41, 0x3F, 0x01, 0x00 }, // 'J'
    { 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00 }, // 'K'
    { 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00 }, // 'L'
    { 0x7F, 0x02, 0x04, 0x02, 0x7F, 0x00 }, // 'M'
    { 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00 }, // 'N'
    { 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00 }, // 'O'
    { 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00 }, // 'P'
    { 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00 }, // 'Q'
    { 0x7F, 0x09, 0x19, 0x29, 0x46, 0x00 }, // 'R'
    { 0x46, 0x49, 0x49, 0x49, 0x31, 0x00 }, // 'S'
    { 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00 }, // 'T'
    { 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00 }, // 'U'
    { 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00 }, // 'V'
    { 0x7F, 0x20, 0x18, 0x20, 0x7F, 0x00 }, // 'W'
    { 0x63, 0x14, 0x08, 0x14, 0x63, 0x00 }, // 'X'
    { 0x03, 0x04, 0x78, 0x04, 0x03, 0x00 }, // 'Y'
    { 0x61, 0x51, 0x49, 0x45, 0x43, 0x00 }, // 'Z'
    { 0x00, 0x00, 0x7F, 0x41, 0x41, 0x00 }, // '['
    { 0x02, 0x04, 0x08, 0x10, 0x20, 0x00 }, // '\'
    { 0x41, 0x41, 0x7F, 0x00, 0x00, 0x00 }, // ']'
    { 0x04, 0x02, 0x01, 0x02, 0x04, 0x00 }, // '^'
    { 0x40, 0x40, 0x40, 0x40, 0x40, 0x00 }, // '_'
    { 0x00, 0x01, 0x02, 0x04, 0x00, 0x00 }, // '`'
    { 0x20, 0x54, 0x54, 0x54, 0x78, 0x00 }, // 'a'
    { 0x7F, 0x48, 0x44, 0x44, 0x38, 0x00 }, // 'b'
    { 0x38, 0x44, 0x44, 0x44, 0x20, 0x00 }, // 'c'
    { 0x38, 0x44, 0x44, 0x48, 0x7F, 0x00 }, // 'd'
    { 0x38, 0x54, 0x54, 0x54, 0x18, 0x00 }, // 'e'
    { 0x08, 0x7E, 0x09, 0x01, 0x02, 0x00 }, // 'f'
    { 0x08, 0x54, 0x54, 0x54, 0x3C, 0x00 }, // 'g'
    { 0x7F, 0x08, 0x04, 0x04, 0x78, 0x00 }, // 'h'
    { 0x00, 0x44, 0x7D, 0x40, 0x00, 0x00 }, // 'i'
    { 0x20, 0x40, 0x44, 0x3D, 0x00, 0x00 }, // 'j'
    { 0x00, 0x7F, 0x10, 0x28, 0x44, 0x00 }, // 'k'
    { 0x00, 0x41, 0x7F, 0x40, 0x00, 0x00 }, // 'l'
    { 0x7C, 0x04, 0x18, 0x04, 0x78, 0x00 }, // 'm'
    { 0x7C, 0x08, 0x04, 0x04, 0x78, 0x00 }, // 'n'
    { 0x38, 0x44, 0x44, 0x44, 0x38, 0x00 }, // 'o'
    { 0x7C, 0x14, 0x14, 0x14, 0x08, 0x00 }, // 'p'
    { 0x08, 0x14, 0x14, 0x18, 0x7C, 0x00 }, // 'q'
    { 0x7C, 0x08, 0x04, 0x04, 0x08, 0x00 }, // 'r'
    { 0x48, 0x54, 0x54, 0x54, 0x20, 0x00 }, // 's'
    { 0x04, 0x3F, 0x44, 0x40, 0x20, 0x00 }, // 't'
    { 0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00 }, // 'u'
    { 0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00 }, // 'v'
    { 0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00 }, // 'w'
    { 0x44, 0x28, 0x10, 0x28, 0x44, 0x00 }, // 'x'
    { 0x0C, 0x50, 0x50, 0x50, 0x3C, 0x00 }, // 'y'
    { 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00 }, // 'z'
    { 0x00, 0x08, 0x36, 0x41, 0x00, 0x00 }, // '{'
    { 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00 }, // '|'
    { 0x00, 0x41, 0x36, 0x08, 0x00, 0x00 }, // '}'
    { 0x08, 0x08, 0x2A, 0x1C, 0x08, 0x00 }, // '~'
};

// ----------- LCD status update -----------
void LCD_updateStatus(SystemStatus status, float az_deg, float el_deg)
{
    static SystemStatus last_status = STATUS_STOPPED;

    
    char az_str[16];
    char el_str[16];
    
    static int last_az = -9999;
    static int last_el = -9999;
    
    if(status != last_status)
    {
        last_status = status;
        
        // Disable Timer 2 interrupts during screen update to prevent SPI corruption
        IEC0bits.T2IE = 0;
        
        // Clear entire screen
        LCD_fillScreen(COLOR_BLACK);
        
        switch(status)
        {
            case STATUS_OK:
                // Format degree strings
                sprintf(az_str, "AZ: %03d", (int)az_deg);
                sprintf(el_str, "EL: %03d", (int)el_deg);
                LCD_drawStringCentered(90, "STATUS: OK", COLOR_GREEN, COLOR_BLACK, 5);
                // force redraw of AZ/EL
                last_az = -9999;
                last_el = -9999;
                break;
                
            case STATUS_HOMING:
                LCD_drawStringCentered(140, "STATUS: HOMING", COLOR_YELLOW, COLOR_BLACK, 5);
                break;
            
            case STATUS_STOPPED:
                LCD_drawStringCentered(140, "STATUS: FAULT", COLOR_RED, COLOR_BLACK, 5);
                break;
        }
        
        // Enable Timer 2 interrupts
        IEC0bits.T2IE = 1;
    }
    
    if(status == STATUS_OK)
    {
        int az_int = (int)az_deg;
        int el_int = (int)el_deg;

        IEC0bits.T2IE = 0;

        if(az_int != last_az)
        {
            last_az = az_int;
            char az_str[16];  // declare AND format right here
            sprintf(az_str, "AZ: %03d", az_int);
            LCD_drawStringCentered(155, az_str, COLOR_WHITE, COLOR_BLACK, 5);
        }
        if(el_int != last_el)
        {
            last_el = el_int;
            char el_str[16];  // declare AND format right here
            sprintf(el_str, "EL: %03d", el_int);
            LCD_drawStringCentered(210, el_str, COLOR_WHITE, COLOR_BLACK, 5);
        }

        IEC0bits.T2IE = 1;
    }
}

static const uint8_t* font_getChar(char c)
{
    if (c < 0x20 || c > 0x7E) c = '?';
    return font_table[c - 0x20];
}

// ---------------- Low-level raw SPI write (no CS, no DC) ----------------
static inline void SPI2_write_raw(uint8_t data)
{
    while (SPI2STATbits.SPITBF);
    SPI2BUF = data;
    while (SPI2STATbits.SPIBUSY);
    (void)SPI2BUF;
}

// ---------------- Send command + optional data in one CS transaction ----------------
void LCD_send(uint8_t cmd, const uint8_t *data, uint8_t len)
{
    LCD_CS_LOW();
    LCD_DC_CMD();
    Nop(); Nop(); Nop();
    SPI2_write_raw(cmd);
    if (data && len > 0)
    {
        LCD_DC_DATA();
        Nop(); Nop(); Nop();
        for (uint8_t i = 0; i < len; i++)
            SPI2_write_raw(data[i]);
    }
    LCD_CS_HIGH();
}

// ---------------- Single command ----------------
void LCD_cmd(uint8_t cmd)
{
    LCD_send(cmd, NULL, 0);
}

// ---------------- Single data byte ----------------
void LCD_data(uint8_t data)
{
    LCD_CS_LOW();
    LCD_DC_DATA();
    Nop(); Nop(); Nop();
    SPI2_write_raw(data);
    LCD_CS_HIGH();
}

// ---------------- Data buffer ----------------
void LCD_data_buffer(const uint8_t *data, int length)
{
    LCD_CS_LOW();
    LCD_DC_DATA();
    Nop(); Nop(); Nop();
    for (int i = 0; i < length; i++)
        SPI2_write_raw(data[i]);
    LCD_CS_HIGH();
}

// ---------------- Set address window ----------------
void LCD_setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t col[] = { x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF };
    uint8_t row[] = { y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF };
    LCD_send(0x2A, col, 4);     // CASET
    LCD_send(0x2B, row, 4);     // PASET
}

// ---------------- Draw single pixel ----------------
void LCD_drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) return;
    LCD_setAddrWindow(x, y, x, y);
    uint8_t px[] = { color >> 8, color & 0xFF };
    LCD_send(0x2C, px, 2);      // RAMWR + pixel
}

// ---------------- Fill entire screen with one color ----------------
void LCD_fillScreen(uint16_t color)
{
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    LCD_setAddrWindow(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    LCD_CS_LOW();
    LCD_DC_CMD();
    Nop(); Nop(); Nop();
    SPI2_write_raw(0x2C);       // RAMWR
    LCD_DC_DATA();
    for (uint32_t i = 0; i < (uint32_t)LCD_WIDTH * LCD_HEIGHT; i++)
    {
        SPI2_write_raw(hi);
        SPI2_write_raw(lo);
    }
    LCD_CS_HIGH();
}

// ---------------- Draw solid color block ----------------

void LCD_drawBlock(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    LCD_setAddrWindow(x, y, x + w - 1, y + h - 1);
    LCD_CS_LOW();
    LCD_DC_CMD();
    Nop(); Nop(); Nop();
    SPI2_write_raw(0x2C);       // RAMWR
    LCD_DC_DATA();
    for (uint32_t i = 0; i < (uint32_t)w * h; i++)
    {
        SPI2_write_raw(hi);
        SPI2_write_raw(lo);
    }
    LCD_CS_HIGH();
}

// ---------------- Draw single character ----------------
void LCD_drawChar(uint16_t x, uint16_t y, char c,
                  uint16_t fg, uint16_t bg, uint8_t scale)
{
    const uint8_t *bitmap = font_getChar(c);
    for (uint8_t col = 0; col < FONT_WIDTH; col++)
    {
        uint8_t column = bitmap[col];
        for (uint8_t row = 0; row < FONT_HEIGHT; row++)
        {
            uint16_t color = (column & (1 << row)) ? fg : bg;
            uint16_t px = x + col * scale;
            uint16_t py = y + row * scale;
            if (scale == 1)
                LCD_drawPixel(px, py, color);
            else
                LCD_drawBlock(px, py, scale, scale, color);
        }
    }
}

// ---------------- Draw string ----------------
void LCD_drawString(uint16_t x, uint16_t y, const char *str,
                    uint16_t fg, uint16_t bg, uint8_t scale)
{
    uint16_t cursor_x = x;
    while (*str)
    {
        LCD_drawChar(cursor_x, y, *str, fg, bg, scale);
        cursor_x += FONT_WIDTH * scale;
        str++;
    }
}

// ---------------- Draw string and clear rest of line ----------------
void LCD_drawStringLine(uint16_t x, uint16_t y, const char *str,
                        uint16_t fg, uint16_t bg, uint8_t scale)
{
    LCD_drawString(x, y, str, fg, bg, scale);
    uint16_t text_end_x = x + strlen(str) * FONT_WIDTH * scale;
    uint16_t line_height = FONT_HEIGHT * scale;
    if (text_end_x < LCD_WIDTH)
        LCD_drawBlock(text_end_x, y, LCD_WIDTH - text_end_x, line_height, bg);
}

// ---------------- Draw string centered horizontally ----------------
void LCD_drawStringCentered(uint16_t y, const char *str,
                            uint16_t fg, uint16_t bg, uint8_t scale)
{
    uint16_t str_width = strlen(str) * FONT_WIDTH * scale;
    uint16_t x = (LCD_WIDTH - str_width) / 2;
    LCD_drawStringLine(x, y, str, fg, bg, scale);
}

// ---------------- ST7796S initialization ----------------
void ST7796_init(void)
{
    delay_ms(10);

    LCD_send(0x01, NULL, 0);    // SWRESET
    delay_ms(120);

    LCD_send(0x11, NULL, 0);    // SLPOUT
    delay_ms(120);

    LCD_send(0x36, (uint8_t[]){0xE8}, 1);   // MADCTL - landscape
    LCD_send(0x3A, (uint8_t[]){0x55}, 1);   // COLMOD - 16-bit

    LCD_send(0xF0, (uint8_t[]){0xC3}, 1);   // unlock extended commands
    LCD_send(0xF0, (uint8_t[]){0x96}, 1);

    LCD_send(0xB4, (uint8_t[]){0x00}, 1);   // INVTR - normal mode

    LCD_send(0xE8, (uint8_t[]){0x40, 0x8A, 0x00, 0x00,
                            0x29, 0x19, 0xA5, 0x33}, 8);    // DOCA

    LCD_send(0xC5, (uint8_t[]){0x27}, 1);   // VCMPCTL

    LCD_send(0xF0, (uint8_t[]){0x3C}, 1);   // relock extended commands
    LCD_send(0xF0, (uint8_t[]){0x69}, 1);

    LCD_send(0x29, NULL, 0);    // DISPON
    delay_ms(20);
}