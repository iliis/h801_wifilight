#include "animation.h"

////////////////////////////////////////////////////////////////////////////////

Animation* ICACHE_FLASH_ATTR new_animation(size_t length, unsigned long int frame_duration)
{
    Animation* anim = (Animation*) os_malloc(sizeof(Animation));

    if (!anim) {
        os_printf("[Anim] ERROR: cannot allocate Animation struct (%d bytes).\n",
                sizeof(Animation));
        return NULL;
    }


    anim->data = (uint8_t*) os_malloc(length*FRAME_SIZE);

    // init MAGENTA
    os_memset(anim->data, 0, length*FRAME_SIZE);
    size_t i = 0;
    for (; i < length; i++) {
        anim->data[i*FRAME_SIZE + 0] = 255;
        anim->data[i*FRAME_SIZE + 2] = 255;
    }

    anim->length = length;
    anim->frame_duration = frame_duration;

    anim->current_pos = 0;
    anim->running = 0;

    if (!anim->data) {
        os_printf("[Anim] ERROR: cannot allocate %d bytes for animation data.\n",
                length*FRAME_SIZE);
        os_free(anim);
        return NULL;
    }

    return anim;
}

////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR delete_animation(Animation* anim)
{
    os_free(anim->data);
    os_free(anim);
}

////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR set_animation_frame(Animation* anim, size_t frame, uint8_t r, uint8_t g, uint8_t b, uint8_t w1, uint8_t w2)
{
    if (anim) {
        anim->data[frame * FRAME_SIZE + 0] = r;
        anim->data[frame * FRAME_SIZE + 1] = g;
        anim->data[frame * FRAME_SIZE + 2] = b;
        anim->data[frame * FRAME_SIZE + 3] = w1;
        anim->data[frame * FRAME_SIZE + 4] = w2;
    }
}

////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR tick_animation(Animation* anim, unsigned long int deltaT)
{
    if (anim->running) {
        anim->current_pos += deltaT;
        //os_printf("[ANIM] exec dT = %ld -> %ld\n", deltaT, anim->current_pos);

        if (anim->current_pos > (anim->length-1)*anim->frame_duration) {
            anim->current_pos = (anim->length-1)*anim->frame_duration;
            anim->running = 0;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

// interpolates from a to b
uint8_t interp(uint8_t a, uint8_t b, int cur, int max)
{
    // return a * (1-(cur/max)) + b * (cur/max);
    return a + ((int)b-(int)a)*cur/max;
}

////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR exec_animation(Animation* anim)
{
    if (anim->current_pos == (anim->length-1)*anim->frame_duration) {
        // we're at the end of the animation
        LED_set_arr(&anim->data[(anim->length-1)*FRAME_SIZE]);
    } else {
        // somewhere in the middle, interpolate
        unsigned int frame_idx = anim->current_pos / anim->frame_duration;

        // get old and new color to interpolate between
        uint8_t* frame_a = &anim->data[ frame_idx    * FRAME_SIZE];
        uint8_t* frame_b = &anim->data[(frame_idx+1) * FRAME_SIZE];

        // calculate interpolated color
        uint8_t interp_frame[FRAME_SIZE];
        size_t i = 0;
        for (; i<FRAME_SIZE; i++) {
            interp_frame[i] = interp(frame_a[i], frame_b[i],
                    anim->current_pos - frame_idx * anim->frame_duration,
                    anim->frame_duration);
        }

        // actually set this color
        LED_set_arr(interp_frame);

        /*
        os_printf("[ANIM] exec: %d %d %d - %d %d\n",
                interp_frame[0],
                interp_frame[1],
                interp_frame[2],
                interp_frame[3],
                interp_frame[4]);
                */
    }

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// global state
Animation* current_animation = NULL;
bool cur_anim_autodelete = 0;

int64_t last_timestamp = 0;

////////////////////////////////////////////////////////////////////////////////

static volatile os_timer_t animation_timer;

void animation_timer_func(void * unused) {
    if (current_animation && current_animation->running) {

        uint64_t t = system_get_rtc_time();

        uint32 rtc_us = system_rtc_clock_cali_proc(); // us per RTC cycle

        uint64 localt = (uint32) (t - last_timestamp); // uint32 so it handles overflows correctly
        localt *= rtc_us >> 12;
        localt /= 1000; // convert to miliseconds


        // TODO: calculate deltaT
        exec_animation(current_animation);
        tick_animation(current_animation, localt);

        last_timestamp = t;

    } else {
        if (current_animation)
            exec_animation(current_animation); // make sure last frame is set

        os_printf("[ANIM] timer: stop\n");
        stop_current_animation();
    }
}


////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR start_animation(Animation* anim, bool autodelete)
{
    stop_current_animation();

    if (anim) {
        current_animation = anim;
        current_animation->current_pos = 0;
        current_animation->running = 1;
        cur_anim_autodelete = autodelete;

        last_timestamp = system_get_rtc_time();

        //Setup timer
        os_timer_setfn(&animation_timer, (os_timer_func_t *)animation_timer_func, NULL);

        //Arm the timer
        os_timer_arm(&animation_timer, 10, 1); // execute all 10 ms
    }
}

////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR stop_current_animation()
{
    //Disarm timer
    os_timer_disarm(&animation_timer);
    if (current_animation) {
        if (cur_anim_autodelete) {
            delete_animation(current_animation);
        }
        current_animation = NULL;
    }

    os_printf("[ANIM] stop\n");
}

////////////////////////////////////////////////////////////////////////////////
