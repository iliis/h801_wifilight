WiFiLi
======

Espressif based firmware for cheap H801 RGBWW-LED-controllers.

Inspired by https://eryk.io/2015/10/esp8266-based-wifi-rgb-controller-h801/

Wiring with those red FT1232 clones:

    TX - TX
    RX - RX


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
