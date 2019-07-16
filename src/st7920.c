#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <string.h>
#include "st7920.h"
// #include "bitmaps.h"
#include "tick.h"
// #include "fonts.h"

// extern const FONT_HEADER TahomaB16;

//send 2 byte
static void st7920_send(LCD_STRUCT *lcd, uint8_t dat1, uint8_t dat2);
static void st7920_send_command(LCD_STRUCT *lcd, uint8_t cmd);
static void st7920_send_data(LCD_STRUCT *lcd, uint8_t dat);
static void st7920_set_addr(LCD_STRUCT *lcd, uint8_t row, uint8_t col);

static void st7920_send(LCD_STRUCT *lcd, uint8_t dat1, uint8_t dat2)
{
	uint16_t timeOut = 0xFFFF;
  	spi_send(lcd->spi, dat1);
  	spi_send(lcd->spi, (dat2 & 0xF0));
  	spi_send(lcd->spi, (dat2 << 4));
  	while((SPI_SR(lcd->spi) & SPI_SR_BSY)&&(timeOut--));
}

static void st7920_send_command(LCD_STRUCT *lcd, uint8_t cmd)
{
	st7920_send(lcd, 0xF8, cmd);
}

static void st7920_send_data(LCD_STRUCT *lcd, uint8_t dat)
{
	st7920_send(lcd, 0xFA, dat);
}

static void st7920_set_addr(LCD_STRUCT *lcd, uint8_t row, uint8_t col)
{
	st7920_send_command(lcd, 0x3E);
  	st7920_send_command(lcd, 0x80 | (row & 31));
  	st7920_send_command(lcd, 0x80 | col | ((row & 32) >> 2));
  	tick_wait_us(50);
}

void st7920_reset(LCD_STRUCT *lcd)
{
	gpio_clear(lcd->ctrlGpio, lcd->rstPin);
	tick_wait_ms(50);
	gpio_set(lcd->ctrlGpio, lcd->rstPin);
}

void st7920_init(LCD_STRUCT *lcd, uint32_t spi, uint32_t ctrlGpio, uint16_t csPin, uint16_t rstPin, uint8_t width, uint8_t height)
{
	lcd->spi = spi;
	lcd->ctrlGpio = ctrlGpio;
	lcd->csPin = csPin;
	lcd->rstPin = rstPin;
	lcd->width = width;
	lcd->height = height;
	if(spi == SPI1)
	{
		rcc_periph_clock_enable(RCC_SPI1);
		gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO5 | GPIO7);
	}
	else if(spi == SPI2)
	{
		rcc_periph_clock_enable(RCC_SPI2);
		gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO13 | GPIO15);
	}
	else if(spi == SPI3)
	{
		rcc_periph_clock_enable(RCC_SPI3);
		gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO3 | GPIO5);
	}
	spi_reset(spi);
	spi_init_master(spi, SPI_CR1_BAUDRATE_FPCLK_DIV_64, SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
                  SPI_CR1_CPHA_CLK_TRANSITION_2, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
	spi_enable(spi);
	gpio_set_mode(ctrlGpio, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, csPin | rstPin);
	st7920_reset(lcd);
	gpio_set(ctrlGpio, csPin); //enable chip
	tick_wait_us(10);
	st7920_send_command(lcd, 0x38); //FUNCTION_SET: 8bit interface (DL=1), basic instruction set (RE=0) 0x38
	tick_wait_us(72);
	st7920_send_command(lcd, 0x08);  //display on, cursor & blink off; 0x08: all off
	tick_wait_us(72);
	st7920_send_command(lcd, 0x06); //Entry mode: cursor move to right, DRAM address counter (AC) plus 1, no shift
	tick_wait_us(72);
	st7920_send_command(lcd, 0x02); //disable scroll, enable CGRAM address
	tick_wait_us(100);
	st7920_send_command(lcd, 0x01); // clear RAM
	tick_wait_ms(2);
	gpio_clear(ctrlGpio, csPin); //disable chip
}

void st7920_turn_on(LCD_STRUCT *lcd)
{
	st7920_send_command(lcd, DISPLAY_CONTROL | 0x0C);
}

void st7920_turn_off(LCD_STRUCT *lcd)
{
	st7920_send_command(lcd, DISPLAY_CONTROL | 0x08);
}

void st7920_draw_pixel(LCD_STRUCT *lcd, uint8_t x, uint8_t y, enum PixelMode val)
{
	if((x >= lcd->width) || (y >= lcd->height)) return;
	uint16_t cnt;
	cnt = y*16 + x/8;
	if(val == 0) // clear pixel
		lcd->buff[cnt] &= ~(0x80 >> x%8); 
	else if(val == 1) // set pixel
		lcd->buff[cnt] |= (0x80 >> x%8);
	else  // invert pixelss
		lcd->buff[cnt] ^= (0x80 >> x%8);
}

uint8_t st7920_get_pixel(LCD_STRUCT *lcd, uint8_t x, uint8_t y)
{
	if((x >= lcd->width) || (y >= lcd->height)) return 0;
	return lcd->buff[x + (y*16 + x/8)] & (0x80 >> x%8) ? 1 : 0;
}

void st7920_refresh(LCD_STRUCT *lcd)
{
	uint8_t row;
	uint8_t col;
	uint8_t *ptr;
	gpio_set(lcd->ctrlGpio, lcd->csPin);
	ptr = lcd->buff;
	for(row = 0; row < 64; row++)
	{
		st7920_set_addr(lcd, row, 0);
		for(col = 0; col < 8; col++)
		{
			st7920_send_data(lcd, *ptr++);
			st7920_send_data(lcd, *ptr++);
			tick_wait_us(10);
		}
	}
	gpio_clear(lcd->ctrlGpio, lcd->csPin);
}

void st7920_clear_screen(LCD_STRUCT *lcd)
{
	memset(lcd->buff, 0x00, sizeof(lcd->buff));
}

void st7920_draw_box(LCD_STRUCT *lcd, int x1, int y1, int x2, int y2, bool set)
{
	int x, y;
	for(x = x1; x <= x2; x++)
		for(y = y1; y <= y2; y++)
			set ? st7920_draw_pixel(lcd, x, y, PixelSet) : st7920_draw_pixel(lcd, x, y, PixelClear);
}

void st7920_draw_line(LCD_STRUCT *lcd, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	uint8_t dy, dx;
	uint8_t addx, addy;
	int16_t p, diff, i;
	if(x1 >= x0)
	{
		dx = x1 - x0;
		addx = 1;
	} else {
		dx = x0 - x1;
		addx = -1;
	}
	if(y1 >= y0)
	{
		dy = y1 - y0;
		addy = 1;
	} else {
		dy = y0 - y1;
		addy = -1;
	}
	if(dx >= dy)
	{
		dy *=2;
		p = dy - dx;
		diff = p - dx;
		for(i = 0; i <= dx; ++i)
		{
			lcd->buff[x0/8 + (y0*16)] |= (0x80 >> x0%8);
			if(p < 0)
			{
				p += dy;
				x0 += addx;
			} else {
				p += diff;
				x0 += addx;
				y0 += addy;
			}
		}
	} else {
		dx *= 2;
		p = dx - dy;
		diff = p - dy;
		for(i = 0; i <= dy; ++i)
		{
			lcd->buff[x0/8 + (y0*16)] |= (0x80 >> x0%8);
			if(p < 0)
			{
				p += dx;
				y0 += addy;
			} else {
				p += diff;
				x0 += addx;
				y0 += addy;
			}
		}
	}
}

void st7920_draw_8_pixel(LCD_STRUCT *lcd, uint8_t xc, uint8_t yc, uint8_t x, uint8_t y)
{
	st7920_draw_pixel(lcd, x + xc, y + yc, PixelSet);
	st7920_draw_pixel(lcd, -x + xc, y + yc, PixelSet);
	st7920_draw_pixel(lcd, x + xc, -y + yc, PixelSet);
	st7920_draw_pixel(lcd, -x + xc, -y + yc, PixelSet);
	st7920_draw_pixel(lcd, y + xc, x + yc, PixelSet);
	st7920_draw_pixel(lcd, -y + xc, x + yc, PixelSet);
	st7920_draw_pixel(lcd, y + xc, -x + yc, PixelSet);
	st7920_draw_pixel(lcd, -y + xc, -x + yc, PixelSet);
}

void st7920_draw_circle_midpoint(LCD_STRUCT *lcd, int xc, int yc, int r)
{
	int x = 0; int y = r;
	int f = 1 - r;
	st7920_draw_8_pixel(lcd, xc, yc, x, y);
	while (x < y)
	{
		if(f < 0) f += (x<<1) + 3;
		else
		{
			y--;
			f += ((x-y) << 1) + 5;
		}
		x++;
		st7920_draw_8_pixel(lcd, xc, yc, x, y);
	}
}

void st7920_draw_circle(LCD_STRUCT *lcd, uint8_t x, uint8_t y, uint8_t radius)
{
	int16_t a, b, p;
	a = 0;
	b = radius;
	p = 1 - radius;
	do {
		// lcd->buff[(x/8 + a) + ((y+b)*16)] |= (0x80 >> x%8);
		// lcd->buff[(x/8 + b) + ((y+a)*16)] |= (0x80 >> x%8);
		// lcd->buff[(x/8 - a) + ((y+b)*16)] |= (0x80 >> x%8);
		// lcd->buff[(x/8 - b) + ((y+a)*16)] |= (0x80 >> x%8);
		// lcd->buff[(x/8 + a) + ((y+b)*16)] |= (0x80 >> x%8);
		// lcd->buff[(x/8 + b) + ((y-a)*16)] |= (0x80 >> x%8);
		// lcd->buff[(x/8 - a) + ((y-b)*16)] |= (0x80 >> x%8);
		// lcd->buff[(x/8 - b) + ((y-a)*16)] |= (0x80 >> x%8);
		st7920_draw_8_pixel(lcd, x, y, a, b);
			// st7920_draw_pixel(lcd, a + x, b + y, PixelSet);
			// st7920_draw_pixel(lcd, -a + x, b + y, PixelSet);
			// st7920_draw_pixel(lcd, a + x, -b + y, PixelSet);
			// st7920_draw_pixel(lcd, -a + x, -b + y, PixelSet);
			// st7920_draw_pixel(lcd, b + x, b + y, PixelSet);
			// st7920_draw_pixel(lcd, -b + x, b + y, PixelSet);
			// st7920_draw_pixel(lcd, b + x, -b + y, PixelSet);
			// st7920_draw_pixel(lcd, -b + x, -b + y, PixelSet);
		if(p < 0)
			p += 3 + 2*a++;
		else
			p += 5 + 2*(a++ -b--);
	} while(a <= b);
}

// void st7920_draw_bitmap(LCD_STRUCT *lcd, uint8_t x, uint8_t y, BITMAP bitmap)
// {
// 	st7920_clear_screen(lcd);
// 	int16_t byteWidth = (bitmap.width + 7)/8;
// 	uint8_t byte = 0;
// 	for(int16_t j = 0; j < bitmap.height; j++, y++)
// 	{
// 		for(int16_t i = 0; i < bitmap.width; i++)
// 		{
// 			if(i & 7) byte <<= 1;
// 			else byte = bitmap.data[j * byteWidth + i/8];
// 			if(byte & 0x80) st7920_draw_pixel(lcd, x+i, y, PixelSet);
// 		}
// 	}
// }


// GLYPH_RANGE st7920_text_get_offset(uint16_t ascii)
// {
// 	uint8_t add;
// 	uint16_t charL;
// 	uint16_t charH;
// 	uint16_t charLen = 0;
// 	GLYPH_RANGE range;
// 	for(add = 0; add < TahomaB16.extendedIndexTable->nRanges; add++)
// 	{
// 		charL = TahomaB16.extendedIndexTable->pRanges[add].charLow;
// 		charH = TahomaB16.extendedIndexTable->pRanges[add + 1].charLow;
// 		if(add >= 1)

// 			charLen += TahomaB16.extendedIndexTable->pRanges[add-1].nGlyphs;
// 		if(((ascii >= charL) && (ascii < charH)) || ((ascii >= charL)&&(add == TahomaB16.extendedIndexTable->nRanges-1)))
// 		{
// 			range.charLow = charL;
// 			range.nGlyphs = charLen;
// 			return range;
// 		}
// 	}
// }

// uint16_t st7920_text_get_string_len(uint8_t *str)
// {
// 	uint16_t charLen = 0;
// 	uint16_t charCnt = 0;
// 	while(str[charLen])
// 	{
// 		if(str[charLen] <= 0x7F)
// 			charLen++;
// 		else
// 			charLen+=2;
// 		charCnt++;
// 	}
// 	return charCnt;
// }

// void st7920_text_get_string_to_buff(LCD_STRUCT *lcd, uint8_t *str, uint8_t row)
// {
// 	uint16_t charLen = 0;
// 	uint8_t charW;
// 	uint16_t offSet;
// 	uint16_t add;
// 	uint16_t textInc;
// 	uint16_t colorCnt = 0;
// 	uint16_t dataLen;
// 	uint16_t data[128];
// 	GLYPH_RANGE charRange;
// 	dataLen = 0;
// 	uint8_t j = 0;
// 	memset(data, 0x00, sizeof(data));
// 	while(str[charLen])
// 	{
// 		if(str[charLen] <= 0x7F)
// 			add = (str[charLen] - 0x20) + 0;
// 		else
// 		{
// 			if(str[charLen] == 0xC3)
// 				textInc = 0x40;
// 			else if(str[charLen] == 0xC4)
// 				textInc = 0x80;
// 			else if(str[charLen] == 0xC5)
// 				textInc = 0xC0;
// 			else if(str[charLen] == 0xC6)
// 				textInc = 0x100;
// 			else
// 			{
// 				charLen++;
// 				if(str[charLen] == 0xBA)
// 					textInc = 0x1E00;
// 				else if(str[charLen] == 0xBB)
// 					textInc = 0x1E40;
// 			}
// 			charLen++;
// 			charRange = st7920_text_get_offset(((uint16_t)str[charLen] +textInc));
// 			add = (((uint16_t)str[charLen] + textInc) - charRange.charLow) + charRange.nGlyphs;
// 		}
// 		offSet = TahomaB16.offsetTable[add];
// 		charW = TahomaB16.widthTable[add];
// 		for(add = 0; add < charW; add+= 2)
// 		{
// 			data[dataLen] = ((uint16_t)TahomaB16.dataTable[offSet + 1]<<8)|TahomaB16.dataTable[offSet];
// 			dataLen++;
// 			offSet+=2;
// 		}
// 		charLen++;
// 		colorCnt++;
// 	}

// 	if(row == 0)
// 	{
// 		for(add = 0; add < 128; add++)
// 		{
// 			if(j < 128)
// 			{
// 				for(uint8_t i = 0; i < 16; i++)
// 				{
// 					if(data[add] & 0x8000)
// 						st7920_draw_pixel(lcd, j, 15-i, 1);
// 					else
// 						st7920_draw_pixel(lcd, j, 15-i, 0);
// 					data[add] <<= 1;
// 				}
// 				j++;
// 			}
// 		}
// 	}
// 	if(row == 1)
// 	{
// 		for(add = 0; add < 128; add++)
// 		{
// 			if(j < 128)
// 			{
// 				for(uint8_t i = 0; i < 16; i++)
// 				{
// 					if(data[add] & 0x8000)
// 						st7920_draw_pixel(lcd, j, 31-i, 1);
// 					else
// 						st7920_draw_pixel(lcd, j, 31-i, 0);
// 					data[add] <<= 1;
// 				}
// 				j++;
// 			}
// 		}
// 	}

// 	if(row == 2)
// 	{
// 		for(add = 0; add < 128; add++)
// 		{
// 			if(j < 128)
// 			{
// 				for(uint8_t i = 0; i < 16; i++)
// 				{
// 					if(data[add] & 0x8000)
// 						st7920_draw_pixel(lcd, j, 47-i, 1);
// 					else
// 						st7920_draw_pixel(lcd, j, 47-i, 0);
// 					data[add] <<= 1;
// 				}
// 				j++;
// 			}
// 		}
// 	}

// 	if(row == 3)
// 	{
// 		for(add = 0; add < 128; add++)
// 		{
// 			if(j < 128)
// 			{
// 				for(uint8_t i = 0; i < 16; i++)
// 				{
// 					if(data[add] & 0x8000)
// 						st7920_draw_pixel(lcd, j, 63-i, 1);
// 					else
// 						st7920_draw_pixel(lcd, j, 63-i, 0);
// 					data[add] <<= 1;
// 				}
// 				j++;
// 			}
// 		}
// 	}
// }

// void st7920_draw_string(LCD_STRUCT *lcd, uint8_t x, uint8_t y, const char *text)
// {
// 	st7920_text_get_string_to_buff(lcd, text, y);
// }
