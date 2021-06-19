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


setup (sming)
-------------

Download and install [Sming](https://sming.readthedocs.io/en/latest/getting-started/index.html) framework:

    git clone https://github.com/SmingHub/Sming /opt/sming
    source /opt/sming/Tools/install.sh 8266 # or 'all'
    export SMING_HOME=/opt/sming/Sming
    source $SMING_HOME/../Tools/export.sh

In this project's folder:

    make


setup (deprecated)
-----

- solder some pinheaders onto serial pins and a jumper onto the other two bootloader selection pins
  consult https://eryk.io/2015/10/esp8266-based-wifi-rgb-controller-h801/ for details

//- install Espressif SDK via https://github.com/pfalcon/esp-open-sdk

//- update paths in Makefile

- create app/user_config.hpp with

    #define CFG_SSID            "your SSID here"
    #define CFG_SSID_PASSWORD   "your password here"

- build and flash

   make flash



run on host
-----------

    # create a virtual network device
    ./setup_host_networking.sh

    # remove any old configs
    make dist-clean
    make config-clean

    # build framework
    make SMING_ARCH=Host -j8

    # enable SPIFFS
    make hwconfig HWCONFIG=spiffs

    # fake-flash it
    make flash

    # run it
    make run



TODO
====

- list of WiFi APs
- Webserver
- higher PWM resolution
- add physical touch buttons (for on/off, FAD, red night-light, ...)
- http://www.instructables.com/id/FADing-Fall-Asleep-Device/
- add build info to help or welcome message
