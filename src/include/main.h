#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/iwdg.h>
#include <libopencm3/stm32/timer.h>

#define SIM_ENABLE() gpio_clear(GPIOA, GPIO8)
#define SIM_DISABLE() gpio_set(GPIOA, GPIO8)

#endif