#ifndef _CONFIG_POOL_H_
#define _CONFIG_POOL_H_

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/timer.h>
#include <string.h>

#define PAGE_SIZE                         (0x400)    /* 1 Kbyte */
#define FLASH_SIZE                        (0x20000)  /* 128 KBytes */
#define CONFIG_AREA_SIZE                  4096
#define CONFIG_AREA_START                 ((uint32_t)((0x8000000+FLASH_SIZE) - PAGE_SIZE*(CONFIG_AREA_SIZE/PAGE_SIZE)))

#define CFG_HOLDER  					0x00FF55A1

#define U32                               uint32_t

typedef struct __attribute__((packed)){
	uint32_t cfg_holder;
	uint8_t bottle_curr;
	uint8_t bottle_sum;
	float temp_set_high;
	float temp_set_low;
}CONFIG_POOL;

extern CONFIG_POOL Save_Cfg;

void CFG_Save(void);
void CFG_Load(void);

#endif