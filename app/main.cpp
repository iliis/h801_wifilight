#include <SmingCore.h>

#include "application.hpp"

Application* app = nullptr;

void init()
{
    // this does NOT work:
    // see https://github.com/SmingHub/Sming/issues/2208
    //Serial.begin(115200, SERIAL_8N1, SERIAL_FULL, 1); // 115200 by default
    //Serial.setTx(2); // use GPIO2 for TX <- nope :(

    Serial.begin(SERIAL_BAUD_RATE, SERIAL_8N1, SERIAL_FULL, 2);
    Serial.systemDebugOutput(true); // Enable debug output to serial
    //Serial.write("FooBar!\n");

    app = new Application();
    app->init();
}
