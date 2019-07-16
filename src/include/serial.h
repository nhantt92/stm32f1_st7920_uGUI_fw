#ifndef __SERIAL_H_
#define __SERIAL_H_

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/iwdg.h>
#include "ringbuf.h"
#include "timerTick.h"
#include "tick.h"

#define BUFF_SIZE 512

void serial_init(void);
void serial_puts(uint8_t *str);
void serial_putstring(const uint8_t *str);
uint16_t serial_get(uint8_t *buff, uint16_t len);

#endif /*__SERIAL_H__*/