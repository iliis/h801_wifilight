# Uncomment the line below if you want to use Espressif's PWM library.
#ENABLE_CUSTOM_PWM=0

#SMING_ARCH=Esp8266

# use flash file system (to provide files for webserver)
HWCONFIG=hardware_config

#COM_SPEED=74480 # same as bootloader
COM_SPEED=115200
COM_SPEED_ESPTOOL=576000

# only build code in this folder
COMPONENT_APPCODE=app

# for my h801 module:
SPIFF_FILES=files


# set DTR and RTS high (= deassert = 0), so chip boots normally
# exit miniterm with Ctrl-]
TERMINAL=python3 -m serial.tools.miniterm --encoding ascii /dev/ttyUSB0 115200 --dtr 0 --rts 0


ARDUINO_LIBRARIES=Timezone ArduinoJson6
