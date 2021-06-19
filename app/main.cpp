#include <SmingCore.h>
#include <HardwareSerial.h>
#include <Platform/WifiEvents.h>

#include "util.hpp"
#include "Platform/Station.h"
#include "led.hpp"
#include "clock.hpp"
#include "user_config.hpp"
#include "webserver.hpp"

Configuration config;
Clock* app_clock = nullptr;
Webserver server(config);

void gotIP(IpAddress ip, IpAddress netmask, IpAddress gateway)
{
    LOG(NET, "Successfully connected to WiFi: My IP is %s", ip.toString().c_str());

    LED::set(0,255,0,0,0); // green: connection successfull, waiting for NTP
    server.start();
    app_clock = new Clock();
}

void internetFail(const String& SSID, MacAddress BSSID, WifiDisconnectReason reason)
{
    ERROR(NET, "Failed to connect to %s: %s", SSID.c_str(), WifiEvents.getDisconnectReasonDesc(reason).c_str());

    LED::set(255,0,0,0,0);
}

void init()
{
    // this does NOT work:
    // see https://github.com/SmingHub/Sming/issues/2208
    //Serial.begin(115200, SERIAL_8N1, SERIAL_FULL, 1); // 115200 by default
    //Serial.setTx(2); // use GPIO2 for TX <- nope :(

    Serial.begin(SERIAL_BAUD_RATE, SERIAL_8N1, SERIAL_FULL, 2);
    Serial.systemDebugOutput(true); // Enable debug output to serial
    //Serial.write("FooBar!\n");

    LOG(APP, "initializing...");
    LOG(APP, "mounting SPIFFS");

    // enable filesystem
    if (spiffs_mount()) {
        LOG(APP, "successfully mounted SPIFFS filessytem");
    } else {
        ERROR(APP, "Failed to mount SPIFFS filesystem");
    }

    LED::set(255,155,0,0,0); // orange: waiting for wifi
    //LED::set(0,0,255,0,0);

    LOG(APP, "enabling WiFi");

    WifiStation.enable(true);
    WifiStation.config(CFG_SSID, CFG_SSID_PASSWORD);

    WifiEvents.onStationDisconnect(internetFail);
    WifiEvents.onStationGotIP(gotIP);
}
