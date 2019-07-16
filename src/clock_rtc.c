#include "clock_rtc.h"
#include <libopencm3/stm32/f1/bkp.h>
#include <time.h>

DATE_TIME localTime;

void RTC_Init(void)
{
	//rcc_set_rtc_clock_source(RCC_LSE); // select clock source from Crytal 32.768Khz
	rtc_auto_awake(RCC_LSE, 0x7FFF); // select Crytal 32.768Khz & prescale 32767, mode will not reset RTC when MCU reset
	nvic_enable_irq(NVIC_RTC_IRQ);
	nvic_set_priority(NVIC_RTC_IRQ,1);
	rtc_interrupt_enable(RTC_SEC); // Enable the RTC interrupt SEC
	//rtc_interrupt_enable(RTC_ALR); //Enable the RTC interrupt Alarm
	pwr_disable_backup_domain_write_protect();
	if(BKP_DR1 != RTC_HOLDER)
	{
		BKP_DR1 = RTC_HOLDER;
		localTime.year = 2019;
		localTime.month = 7;
		localTime.mday = 5;
		localTime.hour = 11-7;
		localTime.min = 14;
		localTime.sec = 0;
		RTC_Sync(&localTime, +7);
	}
	pwr_enable_backup_domain_write_protect();
}

void RTC_Now(DATE_TIME *dt)
{
	rtc_interrupt_disable(RTC_SEC); 
	*dt = localTime;
	rtc_interrupt_enable(RTC_SEC); 
}

uint32_t RTC_GetUnixTimestamp(DATE_TIME *dt)
{
	struct tm t;
	time_t t_of_day;
	t.tm_year = dt->year - 1900;
	t.tm_mon = dt->month - 1; 				// Month, 0 - jan
	t.tm_mday = dt->mday; 					// Day of the month
	t.tm_hour = dt->hour;
	t.tm_min = dt->min;
	t.tm_sec = dt->sec;
	t.tm_isdst = -1; 						// Is DST on? 1 = yes, 0 = no, -1 = unknown
	t_of_day = mktime(&t);
	return (uint32_t)t_of_day; 
}

void RTC_Sync(DATE_TIME *dt, uint8_t timeZone)
{
	uint32_t tt;
	uint32_t number;
	rtc_interrupt_disable(RTC_SEC); 
	tt = RTC_GetUnixTimestamp(dt);
	tt += timeZone * 60 * 60;
	rtc_set_counter_val((uint32_t)tt);
	rtc_interrupt_enable(RTC_SEC); 
}

void RTC_CalcTime(DATE_TIME *dt, uint32_t unixTime)
{
	struct tm *gt;
	time_t epoch;
	epoch = unixTime;
	gt = localtime(&epoch);			
	dt->sec = gt->tm_sec;
	dt->min = gt->tm_min;
	dt->hour = gt->tm_hour;
	dt->mday = gt->tm_mday;
	dt->wday = gt->tm_wday + 1;				// tm_wday 0 - 6 (0 = sunday)
	dt->month = gt->tm_mon + 1;				// tm_mon 0 - 11 (0 = Jan)
	dt->year = gt->tm_year + 1900;		    // tm_year = current year - 1900
}

void rtc_isr(void)
{
	volatile uint32_t number;
	/* The interrupt flag isn't cleared by hardware, we have to do it. */
	rtc_clear_flag(RTC_SEC);
	number = rtc_get_counter_val();
	RTC_CalcTime(&localTime, number); 
}

// add number of secs to a DATE_TIME struct
int8_t TIME_AddSec(DATE_TIME *t, int32_t sec)
{
	if(t == NULL) return -1;
	if(sec == 0) return 0;	
	TIME_FromSec(t, TIME_GetSec(t) + sec);	
	return 0;
}

// find the number of sec since 00:00 hours, Jan 1, 1970 UTC
int32_t TIME_GetSec(DATE_TIME *t)
{
	struct tm tx;
	if(t == NULL) return -1;	
	tx.tm_sec = t->sec;
	tx.tm_min = t->min;
	tx.tm_hour = t->hour;
	tx.tm_mday = t->mday;
	tx.tm_mday = t->mday;
	tx.tm_wday = t->wday - 1;
	tx.tm_mon = t->month - 1;
	tx.tm_year = t->year - 1900;
	tx.tm_isdst = 0;
	return mktime(&tx);
}

int8_t TIME_FromSec(DATE_TIME *t, int32_t sec)
{
	struct tm *gt;
	if(t == NULL) return -1;
	gt = localtime((const time_t*)&sec);
	t->sec = gt->tm_sec;
	t->min = gt->tm_min;
	t->hour = gt->tm_hour;
	t->mday = gt->tm_mday;
	t->wday = gt->tm_wday + 1;			// tm_wday 0 - 6 (0 = sunday)
	t->month = gt->tm_mon + 1;			// tm_mon 0 - 11 (0 = Jan)
	t->year = gt->tm_year + 1900;		// tm_year = current year - 1900
	return 0;
}