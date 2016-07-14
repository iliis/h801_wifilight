#include "animation_presets.h"

////////////////////////////////////////////////////////////////////////////////

Animation* ICACHE_FLASH_ATTR mkanim_sunset(unsigned int total_duration)
{
    Animation* a = new_animation(10, total_duration*1000/10);

    set_animation_frame(a, 0,    0,   0,   0,    0,   0);
    set_animation_frame(a, 1,    0,   0,  20,    0,   0);
    set_animation_frame(a, 2,   20,   0,  30,    0,   0);
    set_animation_frame(a, 3,   53,  53,  75,    0,   0);
    set_animation_frame(a, 4,   78,  70, 102,    0,   0);
    set_animation_frame(a, 5,  130,  90,  75,    0,   0);
    set_animation_frame(a, 6,  255, 137,   5,    0,   0);
    set_animation_frame(a, 7,  255, 255, 221,    0,   0);
    set_animation_frame(a, 8,  255, 255, 255,    0,   0);
    set_animation_frame(a, 9,  255, 255, 255,  255, 255);

    return a;
}

////////////////////////////////////////////////////////////////////////////////
