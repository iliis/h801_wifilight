#pragma once

#include "Network/NtpClient.h"
#include <SmingCore.h>

class Clock
{
public:

    Clock()
        // Automatically update system clock through NTP every 20 minutes
      : ntp_client("ch.pool.ntp.org", 20*60, NtpTimeResultDelegate(&Clock::on_ntp_update, this))
    {
    }

    void on_ntp_update(NtpClient& ntp, time_t timestamp);
    static void printTime();

private:
    NtpClient ntp_client;

};
