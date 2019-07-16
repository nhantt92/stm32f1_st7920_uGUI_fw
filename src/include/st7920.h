#ifndef __ST7920_H__
#define __ST7920_H__

#include <stdio.h>
#include <stdbool.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>
#include "tick.h"
// #include "bitmaps.h"
// #include "fonts.h"

#define DISPLAY_CLEAR     0x01
#define RETURN_HOME       0x02
#define ENTRY_MODE        0x04
#define DISPLAY_CONTROL   0x08
#define CD_SHIFT_CONTROL  0x10
#define FUNCTION_SET		  0x30
#define CGRAM_ADDRESS     0x40
#define DDRAM_ADDRESS		  0x80

enum SCREEN_SECTION{
  SCREEN_COMPLETE,
  SCREEN_TOP_HALF,
  SCREEN_BOTTOM_HALF,
  SCREEN_QUADRANT_1,
  SCREEN_QUADRANT_2,
  SCREEN_QUADRANT_3,
  SCREEN_QUADRANT_4  
};

typedef struct {
	uint32_t width;
	uint32_t height;
	uint8_t buff[(128 * 64)/8];
	uint32_t spi;
	uint32_t ctrlGpio;
	uint16_t csPin;
	uint16_t rstPin;
} LCD_STRUCT;

enum PixelMode {
	PixelClear = 0, //clear the pixel (s)
	PixelSet = 1,	// set the pixel (s)
	PixelFlip = 2  // invert the pixel (s)
};

void st7920_init(LCD_STRUCT *lcd, uint32_t spi, uint32_t ctrlGpio, uint16_t csPin, uint16_t rstPin, uint8_t width, uint8_t height);
void st7920_reset(LCD_STRUCT *lcd);
void st7920_turn_on(LCD_STRUCT *lcd);
void st7920_turn_off(LCD_STRUCT *lcd);
void st7920_draw_pixel(LCD_STRUCT *lcd, uint8_t x, uint8_t y, enum PixelMode val);
uint8_t st7920_get_pixel(LCD_STRUCT *lcd, uint8_t x, uint8_t y);
void st7920_refresh(LCD_STRUCT *lcd);
void st7920_clear_screen(LCD_STRUCT *lcd);
void st7920_draw_box(LCD_STRUCT *lcd, int x1, int y1, int x2, int y2, bool set);
void st7920_draw_line(LCD_STRUCT *lcd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void st7920_draw_circle(LCD_STRUCT *lcd, uint8_t x, uint8_t y, uint8_t radius);
void st7920_draw_8_pixel(LCD_STRUCT *lcd, uint8_t xc, uint8_t yc, uint8_t x, uint8_t y);
void st7920_draw_circle_midpoint(LCD_STRUCT *lcd, int xc, int yc, int r);
// void st7920_draw_bitmap(LCD_STRUCT *lcd, uint8_t x, uint8_t y, BITMAP bitmap);
// uint32_t st7920_text_add(LCD_STRUCT *lcd, uint8_t fontH, uint8_t fontL);
// GLYPH_RANGE st7920_text_get_offset(uint16_t ascii);
// uint16_t st7920_text_get_string_len(uint8_t *str);
// void st7920_text_get_string_to_buff(LCD_STRUCT *lcd, uint8_t *str, uint8_t row);
// void st7920_draw_string(LCD_STRUCT *lcd, uint8_t x, uint8_t y, const char *text);

#endif