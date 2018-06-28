#include "system_time.h"

////////////////////////////////////////////////////////////////////////////////
// data

uint32 last_RTC_value = UINT32_MAX;

uint64 accumulated_system_time_us = 0;

////////////////////////////////////////////////////////////////////////////////

static os_timer_t update_timer;

void update_system_time(void *arg)
{
    uint32_t rtc    = system_get_rtc_time();
    uint32_t rtc_us = system_rtc_clock_cali_proc(); // us per RTC cycle

    int64_t rtc_delta = ((int64_t) rtc) - last_RTC_value;
    last_RTC_value = rtc;

    // apply calibration result to convert ticks to microseconds
    rtc_delta *= rtc_us >> 12;

    if (rtc_delta < 0) {
        os_printf("[SYST] WARN: RTC is running backwards: delta = %lld\n", rtc_delta);
        rtc_delta = 0;
    }

    // accumulate time
    accumulated_system_time_us += rtc_delta;

    //os_printf("[SYST] updating system time by %lld uS. It is now %llu uS since startup.\n", rtc_delta, accumulated_system_time_us);

    // Q: should we handle overflows?
    // A: Nah, 2^64 uS equals about half a million years. I'll buy you a beer if you ever run into this.
}

////////////////////////////////////////////////////////////////////////////////

uint64 ICACHE_FLASH_ATTR get_system_time_us()
{
    update_system_time(NULL);
    return accumulated_system_time_us;
}

////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR system_time_init()
{
    last_RTC_value = system_get_rtc_time();
    accumulated_system_time_us = 0;

    update_system_time(NULL);

    // update system time every second
    os_timer_setfn(&update_timer, (os_timer_func_t *)update_system_time, NULL);
    os_timer_arm(&update_timer, 1000, 1);
}

////////////////////////////////////////////////////////////////////////////////
