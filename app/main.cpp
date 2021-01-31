#include <SmingCore.h>
#include <HardwareSerial.h>
#include <Platform/WifiEvents.h>

#include "Platform/Station.h"
#include "led.hpp"
#include "clock.hpp"
#include "user_config.hpp"


enum ApplicationState
{
    BOOTING,
    CONNECTING,
    WAITING_FOR_NTP,
    READY,
    FAILED
} app_state = BOOTING;



/*
void onTimeReceived(NtpClient& ntp, time_t timestamp)
{
    if (app_state != READY)
    {
        debugf("Successfully received time update");
        app_state = READY;
        LED::set(0,0,0,0,0);
    }
}
*/

void gotIP(IpAddress ip, IpAddress netmask, IpAddress gateway)
{
    debugf("Successfully connected to WiFi: My IP is %s", ip.toString().c_str());

    app_state = WAITING_FOR_NTP;
    LED::set(0,255,0,0,0); // green: connection successfull, waiting for NTP
}

void internetFail(const String& SSID, MacAddress BSSID, WifiDisconnectReason reason)
{
    debugf("Failed to connect to %s: %s", SSID.c_str(), WifiEvents.getDisconnectReasonDesc(reason).c_str());

    app_state = FAILED;
    LED::set(255,0,0,0,0);
}


Clock app_clock;

void init()
{
    // this does NOT work:
    // see https://github.com/SmingHub/Sming/issues/2208
    //Serial.begin(115200, SERIAL_8N1, SERIAL_FULL, 1); // 115200 by default
    //Serial.setTx(2); // use GPIO2 for TX <- nope :(

    Serial.begin(SERIAL_BAUD_RATE, SERIAL_8N1, SERIAL_FULL, 2);
    Serial.systemDebugOutput(true); // Enable debug output to serial
    //Serial.write("FooBar!\n");

    debugf("Hallo Welt");

    LED::set(255,155,0,0,0); // orange: waiting for wifi
    //LED::set(0,0,255,0,0);

    WifiStation.enable(true);
    WifiStation.config(CFG_SSID, CFG_SSID_PASSWORD);

    WifiEvents.onStationDisconnect(internetFail);
    WifiEvents.onStationGotIP(gotIP);

    app_state = CONNECTING;
}
