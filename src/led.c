#include "led.h"

////////////////////////////////////////////////////////////////////////////////
/* 10bit
uint32_t gamma_table[256] = {
       0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
       0,    1,    1,    1,    1,    1,    1,    1,    1,    2,    2,    2,    2,    2,    3,    3, 
       3,    3,    4,    4,    4,    5,    5,    5,    6,    6,    7,    7,    7,    8,    8,    9, 
      10,   10,   11,   11,   12,   13,   13,   14,   15,   15,   16,   17,   18,   19,   20,   20, 
      21,   22,   23,   24,   25,   26,   27,   29,   30,   31,   32,   33,   35,   36,   37,   38, 
      40,   41,   43,   44,   46,   47,   49,   50,   52,   54,   55,   57,   59,   61,   63,   64, 
      66,   68,   70,   72,   74,   77,   79,   81,   83,   85,   88,   90,   92,   95,   97,  100, 
     102,  105,  107,  110,  113,  115,  118,  121,  124,  127,  130,  133,  136,  139,  142,  145, 
     149,  152,  155,  158,  162,  165,  169,  172,  176,  180,  183,  187,  191,  195,  199,  203, 
     207,  211,  215,  219,  223,  227,  232,  236,  240,  245,  249,  254,  258,  263,  268,  273, 
     277,  282,  287,  292,  297,  302,  308,  313,  318,  323,  329,  334,  340,  345,  351,  357, 
     362,  368,  374,  380,  386,  392,  398,  404,  410,  417,  423,  429,  436,  442,  449,  455, 
     462,  469,  476,  483,  490,  497,  504,  511,  518,  525,  533,  540,  548,  555,  563,  571, 
     578,  586,  594,  602,  610,  618,  626,  634,  643,  651,  660,  668,  677,  685,  694,  703, 
     712,  721,  730,  739,  748,  757,  766,  776,  785,  795,  804,  814,  824,  833,  843,  853, 
     863,  873,  884,  894,  904,  915,  925,  936,  946,  957,  968,  979,  990, 1001, 1012, 1023, 
};
*/

// Gamma brightness lookup table <https://victornpb.github.io/gamma-table-generator>
// gamma = 2.00 steps = 256 range = 0-16383
const uint32_t gamma_table[256] = {
     0,   0,   1,   2,   4,   6,   9,  12,  16,  20,  25,  30,  36,  43,  49,  57,
    64,  73,  82,  91, 101, 111, 122, 133, 145, 157, 170, 184, 198, 212, 227, 242,
   258, 274, 291, 309, 327, 345, 364, 383, 403, 424, 444, 466, 488, 510, 533, 557,
   580, 605, 630, 655, 681, 708, 735, 762, 790, 819, 848, 877, 907, 938, 968,1000,
  1032,1064,1097,1131,1165,1200,1235,1270,1306,1343,1380,1417,1455,1494,1533,1572,
  1612,1653,1694,1736,1778,1820,1863,1907,1951,1996,2041,2086,2132,2179,2226,2274,
  2322,2371,2420,2469,2519,2570,2621,2673,2725,2778,2831,2885,2939,2993,3049,3104,
  3160,3217,3274,3332,3390,3449,3508,3568,3628,3689,3750,3812,3874,3937,4000,4064,
  4128,4193,4258,4324,4390,4457,4524,4592,4660,4729,4798,4868,4938,5009,5080,5152,
  5224,5297,5371,5444,5519,5594,5669,5745,5821,5898,5975,6053,6131,6210,6290,6370,
  6450,6531,6612,6694,6776,6859,6943,7027,7111,7196,7281,7367,7454,7541,7628,7716,
  7804,7893,7983,8073,8163,8254,8346,8438,8530,8623,8716,8810,8905,9000,9095,9191,
  9288,9385,9482,9580,9679,9778,9877,9977,10078,10179,10281,10383,10485,10588,10692,10796,
  10900,11005,11111,11217,11324,11431,11538,11646,11755,11864,11974,12084,12194,12305,12417,12529,
  12642,12755,12869,12983,13097,13212,13328,13444,13561,13678,13796,13914,14033,14152,14271,14392,
  14512,14633,14755,14877,15000,15123,15247,15371,15496,15621,15747,15873,16000,16127,16255,16383,
};

////////////////////////////////////////////////////////////////////////////////

// RGB FET
#define redPIN      15 //12
#define greenPIN    13 //15
#define bluePIN     12 //13

// W FET
#define w1PIN       14
#define w2PIN       4

// onbaord green LED D1
#define LEDPIN      5
// onbaord red LED D2
#define LED2PIN     1


#define PWM_CONFIG(pinnr)   { PIN_MUX(pinnr), PIN_FUNC(pinnr), pinnr }

// PWM configuration
uint32 io_info[][3] = { PWM_CONFIG(redPIN), // RED
                        PWM_CONFIG(greenPIN), // GREEN
                        PWM_CONFIG(bluePIN), // BLUE
                        PWM_CONFIG(w1PIN), // WHITE1
                        PWM_CONFIG(w2PIN), // WHITE2
                      };

////////////////////////////////////////////////////////////////////////////////

#define PWM_CHANNEL_COUNT   5
#define PWM_PERIOD          16383

////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR LED_set(uint8_t r, uint8_t g, uint8_t b, uint8_t w1, uint8_t w2)
{
    uint8_t rgbww[5] = {r,g,b,w1,w2};
    LED_set_arr(rgbww);
}

void ICACHE_FLASH_ATTR LED_set_arr(uint8_t rgbww[5])
{
    uint8_t channel = 0;
    for (; channel < PWM_CHANNEL_COUNT; channel++) {

        // TODO: if output = 0, disable PWM and just use GPIO

        // convert to raw PWM duty cycle
        pwm_set_duty(gamma_table[rgbww[channel]], channel);
        //os_printf("[LED] setting channel %d to %ld\n", channel, gamma_table[rgbww[channel]]);
    }
    pwm_start();

    /*
    os_printf("[LED] set arr: %d %d %d - %d %d\n",
            rgbww[0], rgbww[1], rgbww[2], rgbww[3], rgbww[4]);
            */
}

////////////////////////////////////////////////////////////////////////////////

void ICACHE_FLASH_ATTR LED_init()
{
    /*
    gpio_output_set(redPIN,     0, redPIN,      0); // output, high-active
    gpio_output_set(greenPIN,   0, greenPIN,    0); // output, high-active
    gpio_output_set(bluePIN,    0, bluePIN,     0); // output, high-active
    gpio_output_set(w1PIN,      0, w1PIN,       0); // output, high-active
    gpio_output_set(w2PIN,      0, w2PIN,       0); // output, high-active
    gpio_output_set(LEDPIN,     0, LEDPIN,      0); // output, high-active
    gpio_output_set(LED2PIN,    0, LED2PIN,     0); // output, high-active
    */

    uint32 pwm_duty_init[PWM_CHANNEL_COUNT] = { 0,0,0,0,0 }; // default: OFF

    os_printf("[PWM] initializing...\n");
    //set_pwm_debug_en(1); // output system messages from PWM library
    /*
    int i = 0;
    for (; i < PWM_CHANNEL_COUNT; i++) {
        os_printf("[PWM] io_info[%d] = %lx, %ld, %ld\n",
                i, io_info[i][0], io_info[i][1], io_info[i][2]);
    }
    */

    // initialize pin functions for PWM output
    pwm_init(PWM_PERIOD, pwm_duty_init, PWM_CHANNEL_COUNT, io_info);
    pwm_start();
}

////////////////////////////////////////////////////////////////////////////////
