#ifndef ANIMATION_H_MXBYNMNW
#define ANIMATION_H_MXBYNMNW

#include <mem.h>
#include <ets_sys.h>

#include "led.h"
#include "system_time.h"

////////////////////////////////////////////////////////////////////////////////

#define FRAME_SIZE 5    // 5 bytes per keyframe (RGBWW)

////////////////////////////////////////////////////////////////////////////////

typedef struct {
    uint8_t * data; // FRAME_SIZE*length bytes
    size_t length;

    unsigned long int frame_duration; // in milliseconds
    unsigned long int current_pos; // between 0 and length*duration
    bool running; // set to false by exec() when animation has completed
} Animation;

////////////////////////////////////////////////////////////////////////////////

Animation* ICACHE_FLASH_ATTR new_animation(size_t length, unsigned long int frame_duration);
void ICACHE_FLASH_ATTR delete_animation(Animation* anim);

void ICACHE_FLASH_ATTR set_animation_frame(Animation* anim, size_t frame, uint8_t r, uint8_t g, uint8_t b, uint8_t w1, uint8_t w2);

// deltaT: number of milliseconds since last tick
void ICACHE_FLASH_ATTR tick_animation(Animation* anim, unsigned long int deltaT);

void ICACHE_FLASH_ATTR exec_animation(Animation* anim);

////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR start_animation(Animation* anim, bool autodelete);
void ICACHE_FLASH_ATTR stop_current_animation();

////////////////////////////////////////////////////////////////////////////////

#endif /* end of include guard: ANIMATION_H_MXBYNMNW */
