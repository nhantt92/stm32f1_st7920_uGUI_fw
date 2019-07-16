#ifndef _TIMER_TICK_H_
#define _TIMER_TICK_H_

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

#define TICK_TIMER          	TIM4
#define TICK_RCC_TIMER      	RCC_TIM4
#define TICK_TIMER_IRQ      	NVIC_TIM4_IRQ
#define TICK_TIMER_ISR      	tim4_isr

#define TICK_CYCLE_US   		1000
#define TICK_CYCLE_MS   		0xFFFFFFFF

typedef struct
{
	uint16_t timeUS;
  	uint32_t timeMS;
}TIME;

void timer_tick_init(void);
void tick_initTime(TIME *pTime);
uint8_t tick_checkUS(TIME *pTime, uint16_t time);
uint8_t tick_checkMS(TIME *pTime, uint32_t time);
uint32_t tick_getTimeMS(void);
void tick_delayUS(TIME *pTime, uint16_t us);
void tick_delayMS(TIME *pTime, uint16_t ms);

#endif