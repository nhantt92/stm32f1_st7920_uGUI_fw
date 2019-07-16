#include "key.h"
#include "tick.h"
#include "debug.h"

KEY_STRUCT key;

void KeyInit(void)
{
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);
	gpio_set_mode(KEY_PORT1, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, KEY_PIN1|KEY_PIN2|KEY_PIN3);
	gpio_set_mode(KEY_PORT2, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, KEY_PIN4);
	for(key.name = 0; key.name < NUM_KEY; key.name++)
	{
		key.ext[key.name].press = key.ext[key.name].pressed = KEY_RELEASE;
		key.ext[key.name].waitRelease = 0;
		key.ext[key.name].cnt = 0; 
		key.tick = 0;
	}
}

void KeyGet(uint8_t port_id)
{
	uint16_t keyGet;
	switch(port_id)
	{
		case 0: 
			keyGet = KEY1_IN();
			if(keyGet == !KEY_PIN1) key.ext[port_id].press = KEY_PRESS;
			else key.ext[port_id].press = KEY_RELEASE;
			break;
		case 1: 
			keyGet = KEY2_IN();
			if(keyGet == !KEY_PIN2) key.ext[port_id].press = KEY_PRESS;
			else key.ext[port_id].press = KEY_RELEASE;
			break;
		case 2: 
			keyGet = KEY3_IN();
			if(keyGet == !KEY_PIN3) key.ext[port_id].press = KEY_PRESS;
			else key.ext[port_id].press = KEY_RELEASE;
			break;
		case 3: 
			keyGet = KEY4_IN();
			if(keyGet == !KEY_PIN4) key.ext[port_id].press = KEY_PRESS;
			else key.ext[port_id].press = KEY_RELEASE;
			break;
	}
	if(key.ext[port_id].press != key.ext[port_id].pressed)
	{
		++(key.ext[port_id].cnt);
		if(key.ext[port_id].cnt == NUM_OF_SAMPLE_KEY)
		{
			key.ext[port_id].pressed = key.ext[port_id].press;
			key.ext[port_id].cnt = 0;
		}
	}
	else
	{
		key.ext[port_id].cnt = 0;
	}
}

// uint8_t KeyPress(uint8_t id)
// {
// 	return key.ext[id].pressed;
// }

BUTTON_ID KeyProcess(void)
{	
	uint8_t currentState = NONE;
	for(key.name = 0; key.name < NUM_KEY; key.name++)
	{
		KeyGet(key.name);
		
		if(key.ext[key.name].pressed == KEY_PRESS)
		{
			if(key.ext[key.name].waitRelease == 0)
			{
				switch(key.name)
				{
					case KEY_MENU:
						currentState = MENU;
						//INFO("KEY MENU\n");
						break;
					case KEY_UP:
						currentState = UP;
						//INFO("KEY UP\n");
						break;
					case KEY_DOWN:
						currentState = DOWN;
						//INFO("KEY DOWN\n");
						break;
					case KEY_EXIT:
						currentState = EXIT;
						//INFO("KEY EXIT\n");
						break;
					default:
						currentState = NONE;
						break;
				}
			}
			key.ext[key.name].waitRelease = 1;
		}
		if(key.ext[key.name].pressed == KEY_RELEASE)
		{
			// if(key.ext[key.name].waitRelease == 1)
			// {	
			// 	switch(key.name)
			// 	{
			// 		case KEY_MENU:
			// 			//HD44780_Clear();
			// 			HD44780_Puts(0,1,"MENU RELEASE");
			// 			break;
			// 		case KEY_UP:
			// 			//HD44780_Clear();
			// 			HD44780_Puts(0,1,"UP RELEASE");
			// 			break;
			// 		case KEY_DOWN:
			// 			//HD44780_Clear();
			// 			HD44780_Puts(0,1,"DOWN RELEASE");
			// 			break;
			// 		case KEY_EXIT:
			// 			//HD44780_Clear();
			// 			HD44780_Puts(0,1,"EXIT RELEASE");
			// 			break;
			// 	}
			// }
			key.ext[key.name].waitRelease = 0;
		}
	}
	return currentState;
}

void KeyManage(void)
{
	if(tick_expire_ms(&key.tick, 20))
	{
		switch(KeyProcess())
		{
			case MENU: 
				INFO("KEY MENU press\n"); 
				break;
			case DOWN:
				INFO("KEY DOWN press\n");
				break;
			case UP:
				INFO("KEY UP press\n");
				break;
			case EXIT: 
				INFO("KEY EXIT press\n"); 
				break;
			default: 
				break; 
		}
		//MenuUpdate(KeyProcess());
	}
}
