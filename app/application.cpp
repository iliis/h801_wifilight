#include "application.hpp"

Application::Application()
  : server(*this)
{
}

void Application::init()
{
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

    WifiEvents.onStationDisconnect(StationDisconnectDelegate(&Application::internetFail, this));
    WifiEvents.onStationGotIP(StationGotIPDelegate(&Application::gotIP, this));
}

void Application::gotIP(IpAddress ip, IpAddress netmask, IpAddress gateway)
{
    LOG(NET, "Successfully connected to WiFi: My IP is %s", ip.toString().c_str());

    LED::set(0,255,0,0,0); // green: connection successfull, waiting for NTP
    server.start();
    app_clock = new Clock();
}

void Application::internetFail(const String& SSID, MacAddress BSSID, WifiDisconnectReason reason)
{
    ERROR(NET, "Failed to connect to %s: %s", SSID.c_str(), WifiEvents.getDisconnectReasonDesc(reason).c_str());

    LED::set(255,0,0,0,0);
}

void Application::setFromHttpPost(HttpRequest& request)
{
    int alarm_enable = request.getPostParameter("alarm_enable").toInt();

    debugf("set config: alarm on: %d\n", alarm_enable);
}

void Application::writeToJson(JsonObject& json)
{
    json["auto_off"] = asctime(&this->auto_off);
    json["foo"] = this->foo;
    json["local_time"] = SystemClock.getSystemTimeString(eTZ_Local);


    json["color"]["red"]    = LED::get(0);
    json["color"]["green"]  = LED::get(1);
    json["color"]["blue"]   = LED::get(2);
    json["color"]["white1"] = LED::get(3);
    json["color"]["white2"] = LED::get(4);
}
