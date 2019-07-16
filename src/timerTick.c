#include "timerTick.h"

uint16_t timeGet;
uint16_t timeBack;
uint32_t timeTickMs;      
void timer_tick_init(void)
{
	rcc_periph_clock_enable(TICK_RCC_TIMER);

	//timer_reset(TICK_TIMER);
	timer_set_mode(TICK_TIMER, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE,
	             TIM_CR1_DIR_UP);
	timer_set_period(TICK_TIMER, TICK_CYCLE_US);
	timer_set_prescaler(TICK_TIMER, 71);
	timer_set_clock_division(TICK_TIMER, 0x0);

	/* Generate TRGO on every update. */
	timer_set_master_mode(TICK_TIMER, TIM_CR2_MMS_UPDATE);
	timer_enable_counter(TICK_TIMER);

	timer_enable_irq(TICK_TIMER, TIM_DIER_UIE);
	nvic_set_priority(TICK_TIMER_IRQ, 255);
	nvic_enable_irq(TICK_TIMER_IRQ);
}

void TICK_TIMER_ISR()
{
	if(timer_get_flag(TICK_TIMER, TIM_SR_UIF))
	{
		timer_clear_flag(TICK_TIMER, TIM_SR_UIF);
    	timeTickMs++;
	}
}

static uint32_t tick_delta_time(uint8_t um, uint32_t start, uint32_t end)
{
	uint16_t diff;
	uint32_t reload_val;
	if(um == 0)
		reload_val = TICK_CYCLE_US;
	else
		reload_val = TICK_CYCLE_MS;
	if(end > start)
		diff = end - start;
	else
		diff = reload_val - (start - end) + 1;
	return diff;
}

void tick_initTime(TIME *pTime)
{
	pTime->timeMS = timeTickMs;
	pTime->timeUS = timer_get_counter(TICK_TIMER);
}

uint8_t tick_checkUS(TIME *pTime, uint16_t time)
{
	timeGet = timer_get_counter(TICK_TIMER);
	if(tick_delta_time(0, pTime->timeUS, timeGet) >= time)
	{
		pTime->timeUS = timeGet;
		return 0;
	}
	return 1;
}

uint8_t tick_checkMS(TIME *pTime, uint32_t time)
{
	timeGet = timeTickMs;
	if(tick_delta_time(1, pTime->timeMS, timeGet) >= time)
	{
		pTime->timeMS = timeGet;
		return 0;
	}
	return 1;
}

void tick_delayUS(TIME *pTime, uint16_t us)
{
	pTime->timeUS = timer_get_counter(TICK_TIMER);
	while(tick_checkUS(pTime, us));
}

void tick_delayMS(TIME *pTime, uint16_t ms)
{
	pTime->timeMS = timeTickMs;
	while(tick_checkMS(pTime, ms));
}