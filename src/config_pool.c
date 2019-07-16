#include "config_pool.h"
#include <libopencm3/stm32/flash.h>
#include <string.h>
#include <stdio.h>

CONFIG_POOL Save_Cfg;


void CFG_Save()
{
	uint32_t *cfgdest, i;
	cfgdest = (uint32_t*)&Save_Cfg;
	/*Disable Interrupt*/
	//_disable_interrupts();
	/*Unlock Flash to write*/
	flash_unlock();
	/*Erase page*/
	for(i=0; i <= sizeof(CONFIG_POOL)/ PAGE_SIZE; i++) {
		flash_erase_page((U32)(CONFIG_AREA_START + i*PAGE_SIZE));
	}
	/*Flash*/
	for(i=0; i< sizeof(CONFIG_POOL); i+=4) {
		flash_program_word(CONFIG_AREA_START + i, *(U32*)(cfgdest + i/4));
		if(*(U32*)(cfgdest + i/4) != *(U32*)(CONFIG_AREA_START + i)) {
			return;
		}
	}
	flash_lock();
	//_enable_interrupts();
}

void CFG_Load()
{
	memcpy(&Save_Cfg, (void*)CONFIG_AREA_START, sizeof(CONFIG_POOL));
	if(Save_Cfg.cfg_holder != CFG_HOLDER){
		/*Add default configurations here*/
		memset(&Save_Cfg, 0x00, sizeof Save_Cfg);
		Save_Cfg.cfg_holder = CFG_HOLDER;
		Save_Cfg.bottle_curr = 1;
		Save_Cfg.bottle_sum = 1;
		Save_Cfg.temp_set_high = 6;
		Save_Cfg.temp_set_low = 2;
		CFG_Save();
	}
}