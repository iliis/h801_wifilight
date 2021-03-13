#pragma once

#include "Network/NtpClient.h"
#include <SmingCore.h>
#include <optional>

class Clock
{
public:

    void init()
    {
        // Automatically update system clock through NTP every 20 minutes
        ntp_client = std::make_optional<NtpClient>("ch.pool.ntp.org", 20*60, NtpTimeResultDelegate(&Clock::on_ntp_update, this));
    }

    void on_ntp_update(NtpClient& ntp, time_t timestamp);
    static void printTime();

private:
    std::optional<NtpClient> ntp_client;

};
