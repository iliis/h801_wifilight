# Uncomment the line below if you want to use Espressif's PWM library.
#ENABLE_CUSTOM_PWM=0

#SMING_ARCH=Esp8266

#COM_SPEED=74480 # same as bootloader
COM_SPEED=115200
COM_SPEED_ESPTOOL=576000

# only build code in this folder
COMPONENT_APPCODE=app

# for my h801 module:
#DISABLE_SPIFFS = 1
SPI_SIZE=1M
#SPI_SIZE=512K
#SPIFF_SIZE=131072
SPIFF_SIZE=0x10000
SPIFF_FILES=files

# move file system closer to beginning, so it actually fits inside 1M
# otherwise the build system tries to put it at 0x100000, i.e. exactly at the end of the flash
# no idea why this is the default behaviour O.o
# see https://github.com/SmingHub/Sming/issues/2209
RBOOT_SPIFFS_0=0xE0000


# set DTR and RTS high (= deassert = 0), so chip boots normally
TERMINAL=python3 -m serial.tools.miniterm --encoding ascii /dev/ttyUSB0 115200 --dtr 0 --rts 0


ARDUINO_LIBRARIES=Timezone
