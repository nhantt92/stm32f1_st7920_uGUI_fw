#include "main.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/iwdg.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/spi.h>
#include "cdcacm.h"
#include "tick.h"
#include "timerTick.h"
#include "clock_rtc.h"
#include "debug.h"
#include "serial.h"
#include "st7920.h"
#include "gui.h"

static void clock_init(void);
static void iwd_init(void);

/* Output for printf */
int _write(int file, char *ptr, int len);
int _write(int file, char *ptr, int len)
{
  int i;
  if(file == STDOUT_FILENO || file == STDERR_FILENO)
  {
    for(i = 0; i < len; i++)
    {
      if(ptr[i] == '\n')
      {
        cdcacm_input('\r');
      }
      cdcacm_input(ptr[i]);
    }
    return i;
  }
  errno = EIO;
  return -1;
}

static void gpio_setup(void)
{
  gpio_set_mode(GPIOD,GPIO_MODE_OUTPUT_50_MHZ,
          GPIO_CNF_OUTPUT_PUSHPULL, GPIO15);
}

LCD_STRUCT lcd1;
UG_GUI gui;

int X2 = 0;
int Y2 = 0;
int X3 = 0;
int Y3 = 0;
float angulo = 0;
int posicao = 0;
int posicaoh = 0;
int temperatura =0;
int min_temp = 500;
int max_temp = -500;

int ScreenWith = 128;
int ScreenWithC = 96;
int ScreenHeight = 64;
int ScreenHeightC = 32;

void desenha_ponteiro(float valor, float rotacao, int radius)
{
  angulo = valor * 2.0 * 3.1415 / rotacao - 1.5707;
  X2 = ScreenWithC + radius * cos(angulo);
  Y2 = ScreenHeightC + radius * sin(angulo);
  UG_DrawLine(ScreenWithC, ScreenHeightC, X2, Y2, 1);
}

void fundo_relogio(void)
{
  UG_DrawCircle(ScreenWithC, ScreenHeightC, 27, 1);
  UG_DrawCircle(ScreenWithC, ScreenHeightC, 1, 1);
  UG_FontSelect(&FONT_6X10);
  UG_PutString(90, 9, "12");
  UG_PutString(114, 25, "3");
  UG_PutString(94, 44, "6");
  UG_PutString(74, 25, "9");
  for(int traco_minuto = 0; traco_minuto < 12; traco_minuto++)
  {
    angulo = traco_minuto / 12.0 * 2 * 3.1415;
    X2 = ScreenWithC + 25 * cos(angulo);
    Y2 = ScreenHeightC + 25 * sin(angulo);
    X3 = ScreenWithC + 25 * cos(angulo);
    Y3 = ScreenHeightC + 25 * sin(angulo);
    UG_DrawLine(X2, Y2, X3, Y3, 1);
  }
}

void tela_1(void)
{ 
  st7920_clear_screen(&lcd1);
  UG_DrawFrame(0, 0, 127, 63, 1);
  fundo_relogio();
  desenha_ponteiro(localTime.hour, 12.0, 10);
  desenha_ponteiro(localTime.min, 60.0, 19);
  desenha_ponteiro(localTime.sec, 60, 21);
}

static void write_pixel(uint16_t x, uint16_t y, uint8_t val)
{
  st7920_draw_pixel(&lcd1, (uint8_t)x, (uint8_t)y, (uint8_t)val);
}

static void window_1_callback(UG_MESSAGE* msg)
{
  if(msg->type == MSG_TYPE_OBJECT)
  {
    if(msg->id == OBJ_TYPE_BUTTON)
    {
      switch(msg->sub_id)
      {
        case BTN_ID_0:
        break;
        case BTN_ID_1:
        break;
        case BTN_ID_2:
        break;
        break;
      }
    }
  }
}

int main(void)
{
  UG_WINDOW window_1;
  UG_BUTTON button_1;
  UG_OBJECT obj_buff_wnd_1[3];
  uint32_t wait = 0xFFFFFF;
  char* str;
  TIME tick, refresh;
  uint8_t x, y, p;
  clock_init();
  tick_init();
  timer_tick_init();
  st7920_init(&lcd1, SPI3, GPIOC, GPIO5, GPIO4, 128, 64);
  RTC_Init();
  cdcacm_init();
  iwd_init();
  tick_initTime(&tick);
  st7920_clear_screen(&lcd1);
  st7920_refresh(&lcd1);
  UG_Init(&gui, write_pixel, 128, 64);
  // UG_FontSelect(&FONT_12X20);
  // UG_WindowCreate(&window_1, obj_buff_wnd_1, 3, window_1_callback);
  // UG_WindowSetTitleTextFont(&window_1, &FONT_6X8);
  // UG_WindowSetTitleText(&window_1, "uGUI Window");
  // UG_WindowSetTitleColor(&window_1, 1);
  // UG_WindowSetTitleTextColor(&window_1, 0);
  // UG_WindowSetBackColor(&window_1, 0);
  // UG_WindowSetForeColor(&window_1, 1);

  // UG_ButtonCreate(&window_1, &button_1, BTN_ID_0, 2, 5, 64, 20);
  // UG_ButtonSetFont(&window_1, BTN_ID_0, &FONT_4X6);
  // UG_ButtonSetText(&window_1, BTN_ID_0, "But");
  // UG_ButtonSetForeColor(&window_1, BTN_ID_0, 1);
  // UG_ButtonSetBackColor(&window_1, BTN_ID_0, 0);
  // UG_ButtonSetAlternateForeColor(&window_1, BTN_ID_0, 1);
  // UG_ButtonSetAlternateBackColor(&window_1, BTN_ID_0, 1);

  // UG_WindowShow(&window_1);
  // UG_Update();

  // UG_ConsoleSetBackcolor(1);
  // UG_ConsoleSetForecolor(0);
  // UG_ConsoleSetArea(0, 0, 127, 63);
  // UG_ConsolePutString(" System initialized!\n");
  // UG_ConsolePutString(" SD-Card mounted!\n");
  // UG_ConsolePutString(" Connected!\n");
  // UG_ConsolePutString(" Start!\n");
  // st7920_draw_bitmap(&lcd1, 0, 0, bmp_logo_ph);
  // st7920_refresh(&lcd1);
  // while(wait--)
  // {
  // }
  // st7920_clear_screen(&lcd1);
  //fundo_relogio();
  //desenha_ponteiro(-5, 60.0, 19);
  st7920_refresh(&lcd1);
  uint8_t cnt = 0;
  uint8_t buff[50];
  while(1)
  {
    if(!tick_checkMS(&tick, 500))
    {
      if(++cnt >100) cnt = 0;
      //memset(buff, 0x00, 10);
      //sprintf(buff, "%02d:%02d:%02d", localTime.hour, localTime.min, localTime.sec);
      //UG_PutString(0, 0, buff);
      tela_1();
    }
    if(!tick_checkMS(&refresh, 500))
    {
      st7920_refresh(&lcd1);
    }
    cdcacm_manage();
    iwdg_reset();
  }
}
static void iwd_init(void)
{
  iwdg_reset();
  iwdg_set_period_ms(5000);
  iwdg_start();
}

static void clock_init(void)
{
  rcc_clock_setup_in_hse_8mhz_out_72mhz();
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_GPIOC);
  rcc_periph_clock_enable(RCC_GPIOD);
  rcc_periph_clock_enable(RCC_AFIO);
  rcc_periph_clock_enable(RCC_TIM1);
  rcc_periph_clock_enable(RCC_TIM3);
  rcc_periph_clock_enable(RCC_SPI1);
  gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON, 0);
}


