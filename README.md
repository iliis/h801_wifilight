WiFiLi
======

Espressif based firmware for cheap H801 RGBWW-LED-controllers.

Inspired by https://eryk.io/2015/10/esp8266-based-wifi-rgb-controller-h801/

Wiring with those red FT1232 clones:

    TX - TX
    RX - RX
    GND - GND

Baud rate: 74880 (same as bootloader)

Use 'make term' to run miniterm.


setup
-----

- solder some pinheaders onto serial pins and a jumper onto the other two bootloader selection pins
  consult https://eryk.io/2015/10/esp8266-based-wifi-rgb-controller-h801/ for details

- install Espressif SDK via https://github.com/pfalcon/esp-open-sdk (takes ~25 minutes to build!)

- update paths in Makefile (or add SDK to $PATH), adjust /dev/ttyUSBx path

- create src/user_config.h with

        #define SSID            "your SSID here"
        #define SSID_PASSWORD   "your password here"

- build and flash

        make flash


usage
-----

telnet $IP 1


TODO
====

- list of WiFi APs
- Webserver
- higher PWM resolution
- add physical touch buttons (for on/off, FAD, red night-light, ...)
- http://www.instructables.com/id/FADing-Fall-Asleep-Device/
- add build info to help or welcome message
