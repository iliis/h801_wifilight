#ifndef LED_SERVER_H_YXUFJWHT
#define LED_SERVER_H_YXUFJWHT

#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>
#include <user_interface.h>
#include <espconn.h>

#include "led.h"

void ICACHE_FLASH_ATTR LED_server_start(unsigned int local_port);

#endif /* end of include guard: LED_SERVER_H_YXUFJWHT */
