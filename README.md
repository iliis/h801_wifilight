WiFiLi
======

Espressif based firmware for cheap H801 RGBWW-LED-controllers.

Inspired by https://eryk.io/2015/10/esp8266-based-wifi-rgb-controller-h801/

Wiring with those red FT1232 clones:

    TX - TX
    RX - RX
    GND

optional, but recommended for convenience:

    RTS - RESET (needs some soldering on H801, is active low)
    DTR - GPIO0 (instead of jumper, has pullup to 3.3 -> boot normally)

might need 'sudo stty -F /dev/ttyUSB0 -hup'?
see https://github.com/espressif/esptool/wiki/ESP8266-Boot-Mode-Selection
hup -> assert both RTS and DTR -> reset into bootloader

bootloader: DTR low
run: DTR high

Warning: This is inverted in software! (pyserial at least)
--rts 1 will set RTS to LOW! I.e. reset the chip. This works nicely:

    python3 -m serial.tools.miniterm --encoding ascii /dev/ttyUSB0 115200 --dtr 0 --rts 0


H801 uses GPIO2 for UART TX, but Sming defaults to 1. So ensure UART is properly configured at startup:

    Serial.begin(115200, SERIAL_8N1, SERIAL_FULL, 2);

Setting afterwards somehow does NOT work:

    Serial.begin(115200)
    Serial.setTx(2);


setup
-----

- solder some pinheaders onto serial pins and a jumper onto the other two bootloader selection pins
  consult https://eryk.io/2015/10/esp8266-based-wifi-rgb-controller-h801/ for details

- install Espressif SDK via https://github.com/pfalcon/esp-open-sdk

- update paths in Makefile

- create src/user_config.h with

        #define SSID            "your SSID here"
        #define SSID_PASSWORD   "your password here"

- build and flash

        make flash




TODO
====

- list of WiFi APs
- Webserver
- higher PWM resolution
- add physical touch buttons (for on/off, FAD, red night-light, ...)
- http://www.instructables.com/id/FADing-Fall-Asleep-Device/
- add build info to help or welcome message
