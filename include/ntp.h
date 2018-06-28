#ifndef NTP_H_INCLUDED
#define NTP_H_INCLUDED

#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>
#include <user_interface.h>
#include <espconn.h>
#include <ip_addr.h>
#include <limits.h>

#include "simpletime.h"
#include "system_time.h"

// query NTP server (i.e. force update)
void ICACHE_FLASH_ATTR sendNTPpacket();

// returns NULL if not initialized
time_t * ICACHE_FLASH_ATTR getLocalTime();

void ICACHE_FLASH_ATTR NTPinit();

#endif // NTP_H_INCLUDED
