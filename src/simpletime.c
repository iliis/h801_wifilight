#include "simpletime.h"

////////////////////////////////////////////////////////////////////////////////
// functions to convert to and from system time

// leap year calulator expects year argument as years offset from 1970
#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0

void ICACHE_FLASH_ATTR simple_localtime(time_t timeInput, tmElements_t *tm)
{
    // break the given time_t into time components
    // this is a more compact version of the C library localtime function
    // note that year is offset from 1970 !!!

    uint8_t year;
    uint8_t month, monthLength;
    uint32_t time;
    unsigned long days;

    time = (uint32_t)timeInput;
    tm->Second = time % 60;
    time /= 60; // now it is minutes
    tm->Minute = time % 60;
    time /= 60; // now it is hours
    tm->Hour = time % 24;
    time /= 24; // now it is days
    tm->Wday = ((time + 4) % 7) + 1;  // Sunday is day 1

    year = 0;
    days = 0;
    while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
        year++;
    }
    tm->Year = year; // year is offset from 1970 

    days -= LEAP_YEAR(year) ? 366 : 365;
    time -= days; // now it is days in this year, starting at 0

    days=0;
    month=0;
    monthLength=0;
    for (month=0; month<12; month++) {
        if (month==1) { // february
            if (LEAP_YEAR(year)) {
                monthLength=29;
            } else {
                monthLength=28;
            }
        } else {
            monthLength = monthDays[month];
        }

        if (time >= monthLength) {
            time -= monthLength;
        } else {
            break;
        }
    }
    tm->Month = month + 1;  // jan is month 1
    tm->Day = time + 1;     // day of month
}

////////////////////////////////////////////////////////////////////////////////
