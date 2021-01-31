#pragma once

#include "Network/NtpClient.h"
#include <SmingCore.h>

class Clock
{
public:

    // Automatically update system clock through NTP every 30 seconds
    // TODO: when deployed, we don't need to and shouldn't update this fast, increase to 10min+
    Clock()
      : ntp_client("ch.pool.ntp.org", 30, NtpTimeResultDelegate(&Clock::on_ntp_update, this))
    {
    }

    void on_ntp_update(NtpClient& ntp, time_t timestamp);
    static void printTime();

private:
    NtpClient ntp_client;

};
