#ifndef ALARM_H_YALGCBSB
#define ALARM_H_YALGCBSB

#include <user_interface.h>
#include <espconn.h>
#include <errno.h>
#include <osapi.h>

#include "animation_presets.h"
#include "helpers.h"
#include "ntp.h"

void ICACHE_FLASH_ATTR alarm_server_init(unsigned int port);

#endif /* end of include guard: ALARM_H_YALGCBSB */
