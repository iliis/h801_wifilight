#include <SmingCore.h>
#include <HardwareSerial.h>
#include <Platform/WifiEvents.h>
#include <ArduinoJson.h>
#include <JsonObjectStream.h>
#include <Timezone.h>
#include <Network/Http/HttpRequest.h>

#include "util.hpp"
#include "Platform/Station.h"
#include "led.hpp"
#include "clock.hpp"
#include "user_config.hpp"
#include "webserver.hpp"

class Application
{
public:
    Application();

    void init();

    void setFromHttpPost(HttpRequest& request);
    void writeToJson(JsonObject& json);

private:
    void gotIP(IpAddress ip, IpAddress netmask, IpAddress gateway);
    void internetFail(const String& SSID, MacAddress BSSID, WifiDisconnectReason reason);

    Clock* app_clock = nullptr;
    Webserver server;


    static const size_t JSON_BUF_SIZE = 1024; // should be big enough to hold json of this object
    using JsonBuf = StaticJsonDocument<Application::JSON_BUF_SIZE>;

    // config values
    tm auto_off = { .tm_min = 30 };
    tm alarm_rampup = { .tm_min = 30 };
    tm alarm_fire = { .tm_min = 0, .tm_hour = 8 };
    int foo = 123;
};
