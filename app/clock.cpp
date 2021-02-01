#include <Timezone.h>

#include "clock.hpp"
#include "led.hpp"

// https://de.wikipedia.org/wiki/Sommerzeit
const TimeChangeRule dst_start = {"MESZ", Last, Sun, Mar, 2, 2*60}; // UTC+2
const TimeChangeRule std_start = {"MEZ",  Last, Sun, Oct, 3, 1*60}; // UTC+1
Timezone local_TZ(dst_start, std_start);

void Clock::on_ntp_update(NtpClient& ntp, time_t timestamp)
{
    static bool first_update = true;

    /*
     * Update the system clock and calculate the correct time offset,
     * accounting for time zone and daylight savings.
     */
    auto localTime = local_TZ.toLocal(timestamp);
    SystemClock.setTime(timestamp, eTZ_UTC);
    SystemClock.setTimeZoneOffset(localTime - timestamp);

    printTime();

    if (first_update)
    {
        first_update = false;
        LED::set(0,0,0,0,0);
    }
}

void Clock::printTime()
{
    Serial.print("Local Time: ");
    Serial.print(SystemClock.getSystemTimeString(eTZ_Local));
    Serial.print(" ");
    Serial.print("UTC Time:   ");
    Serial.println(SystemClock.getSystemTimeString(eTZ_UTC));
}
