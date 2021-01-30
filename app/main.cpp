#include <SmingCore.h>
#include "led.hpp"

void init()
{
    Serial.begin(74880); //SERIAL_BAUD_RATE); // 115200 by default
    Serial.systemDebugOutput(true); // Enable debug output to serial

    debugf("Hallo Welt");

    LED::set(1,0,0,0,0);

    Serial.write("asdfsafsafsafsadfsa;fjsa;ifjs;fijsaf;idsafj\n");
    Serial.write("nananananananananananananananananananananananananananananana\n");
}
