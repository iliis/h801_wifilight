#pragma once

#include "Network/NtpClient.h"
#include <SmingCore.h>

// only instantiate this class once network is connected
class Clock
{
public:

    Clock();

    void on_ntp_update(NtpClient& ntp, time_t timestamp);
    static void printTime();

private:
    NtpClient ntp_client;

};
