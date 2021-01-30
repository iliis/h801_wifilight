# Uncomment the line below if you want to use Espressif's PWM library.
#ENABLE_CUSTOM_PWM=0

#SMING_ARCH=Esp8266

# only build code in this folder
COMPONENT_APPCODE=app

# for my h801 module:
#DISABLE_SPIFFS = 1
SPI_SIZE=1M
#SPI_SIZE=512K
#SPIFF_SIZE=131072
SPIFF_SIZE=0x15000


# move file system closer to beginning, so it actually fits inside 1M
# otherwise the build system tries to put it at 0x100000, i.e. exactly at the end of the flash
# no idea why this is the default behaviour O.o
RBOOT_SPIFFS_0=0x80000
