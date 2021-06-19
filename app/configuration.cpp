#include "configuration.hpp"

void Configuration::setFromHttpPost(HttpRequest& request)
{
    int alarm_enable = request.getPostParameter("alarm_enable").toInt();

    debugf("set config: alarm on: %d\n", alarm_enable);
}

void Configuration::writeToJson(JsonObject& json)
{
    json["auto_off"] = asctime(&this->auto_off);
    json["foo"] = this->foo;
    json["local_time"] = SystemClock.getSystemTimeString(eTZ_Local);
}
