#pragma once

#include <SmingCore.h>
#include <ArduinoJson.h>
#include <JsonObjectStream.h>
#include <Timezone.h>
#include <Network/Http/HttpRequest.h>

class Configuration
{
public:
    static const size_t JSON_BUF_SIZE = 1024; // should be big enough to hold json of this object
    using JsonBuf = StaticJsonDocument<Configuration::JSON_BUF_SIZE>;

    void setFromHttpPost(HttpRequest& request);
    void writeToJson(JsonObject& json);

    // config values
    tm auto_off = { .tm_min = 30 };
    tm alarm_rampup = { .tm_min = 30 };
    tm alarm_fire = { .tm_min = 0, .tm_hour = 8 };
    int foo = 123;
};
