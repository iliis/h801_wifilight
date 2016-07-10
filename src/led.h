#ifndef LED_H_EFBRKS25
#define LED_H_EFBRKS25

#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>
#include <pwm.h>

#include "pinmux.h"

void ICACHE_FLASH_ATTR LED_set(uint8_t r, uint8_t g, uint8_t b, uint8_t w1, uint8_t w2);
void ICACHE_FLASH_ATTR LED_set_arr(uint8_t rgbww[5]);

void ICACHE_FLASH_ATTR LED_init();

#endif /* end of include guard: LED_H_EFBRKS25 */
