#ifndef __KEY_H
#define __KEY_H

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/iwdg.h>
#include "tick.h"

#define KEY_PORT1 GPIOC
#define KEY_PORT2 GPIOD
#define KEY_PIN1 GPIO10
#define KEY_PIN2 GPIO11
#define KEY_PIN3 GPIO12
#define KEY_PIN4 GPIO2

#define KEY1_IN() GPIO_IDR(KEY_PORT1)&KEY_PIN1
#define KEY2_IN() GPIO_IDR(KEY_PORT1)&KEY_PIN2
#define KEY3_IN() GPIO_IDR(KEY_PORT1)&KEY_PIN3
#define KEY4_IN() GPIO_IDR(KEY_PORT2)&KEY_PIN4

#define NUM_OF_SAMPLE_KEY 10
#define NUM_KEY 4

typedef enum {
	KEY_MENU = 0,
	KEY_UP,
	KEY_DOWN,
	KEY_EXIT
}KEY_NAME;

typedef enum {
	KEY_RELEASE = 0,
	KEY_PRESS = 1
}KEY_EVENT;

typedef struct {
	uint8_t pressed;
	uint8_t press;
	uint8_t waitRelease;
	uint8_t cnt;
}KEY_EXT;

typedef struct {
	KEY_NAME name;
	KEY_EXT ext[NUM_KEY];
	uint32_t tick;
}KEY_STRUCT;

typedef enum {
	NONE = 0,
	MENU,
	UP,
	DOWN,
	EXIT
}BUTTON_ID;

void KeyInit(void);
void KeyGet(uint8_t port_id);
BUTTON_ID KeyProcess(void);
void KeyManage(void);

#endif /* __KEY_H */
