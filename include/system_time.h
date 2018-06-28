#ifndef SYSTEM_TIME_H_INCLUDED
#define SYSTEM_TIME_H_INCLUDED

#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>

/*
 * This module keeps track of time by using RTC timer.
 *
 * But RTC timer drifts and the calibration routine returns varying numbers as
 * well. So just using RTC_value * RTC_us_per_tick can result in quite
 * different numbers, especially if RTC_value is large.
 *
 * Therefore we periodically add the current change of the RTC to our own time counter.
 */

uint64 ICACHE_FLASH_ATTR get_system_time_us();

void   ICACHE_FLASH_ATTR system_time_init();

#endif // SYSTEM_TIME_H_INCLUDED
