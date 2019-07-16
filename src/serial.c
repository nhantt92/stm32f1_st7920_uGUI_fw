#include "serial.h"
#include "debug.h"

uint8_t serialBuf[BUFF_SIZE];
uint8_t serialGet;
uint8_t serialFlag;
uint32_t serialTick;
ringbuf ringSerial;

static void usart_setup(void);

static void usart_setup(void)
{
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_USART1);

	nvic_enable_irq(NVIC_USART1_IRQ);

	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
          GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
          GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);

	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART1, USART_MODE_TX_RX);

	USART_CR1(USART1) |= USART_CR1_RXNEIE;

	usart_enable(USART1);
}

void serial_init(void)
{
  ringbuf_init(&ringSerial, serialBuf, sizeof(serialBuf));
	usart_setup();
	serialFlag = 0;
}

void usart1_isr(void)
{
  uint8_t data;
  /* Check if we were called because of RXNE. */
  if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) &&
      ((USART_SR(USART1) & USART_SR_RXNE) != 0)) {
    /* Retrieve the data from the peripheral. */
    data = usart_recv(USART1);
    ringbuf_put(&ringSerial, data);
    serialTick = tick_get();
    serialFlag = 1;
  }
}

void serial_puts(uint8_t *str)
{
  uint16_t index = 0;
  while(str[index] != 0)
  {
    usart_send_blocking(USART1, str[index]);
    index++;
  }
}

void serial_putstring(const uint8_t *str)
{
  uint16_t index = 0;
  while(str[index] != 0)
  {
    usart_send_blocking(USART1, str[index]);
    index++;
  }
}

uint16_t serial_get(uint8_t *buff, uint16_t len)
{
  uint16_t getcnt = 0;
  if(((tick_get() - serialTick) > 1000) && (serialFlag == 1))
  {
    serialFlag = 2;
    while((ringbuf_get(&ringSerial, &serialGet)) && (getcnt < len))
    {
      buff[getcnt++] = serialGet;
    }
    serialFlag = 0;
  }
  return getcnt;
}