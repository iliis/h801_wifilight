#include "led.hpp"

/*
 * based on:
 * File: Esp SDK Hardware PWM demo
 * Original Author: https://github.com/hrsavla
 *
 * This HardwarePWM library enables Sming framework user to use ESP SDK PWM API
 * Period of PWM is fixed to 1000us / Frequency = 1khz
 * Duty at 100% = 22222. Duty at 0% = 0
 * You can use function setPeriod() to change frequency/period.
 * Calculate the Duty as per the formulae give in ESP8266 SDK
 * Duty = (Period *1000)/45
 *
 * PWM can be generated on upto 8 pins (ie All pins except pin 16)
 * Created on August 17, 2015, 2:27 PM
 *
 * See also ESP8266 Technical Reference, Chapter 12:
 * http://espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf
 */

#include <SmingCore.h>
#include <HardwarePWM.h>

#include <math.h>

namespace LED
{

// RGB FET
#define redPIN      15
#define greenPIN    13
#define bluePIN     12

// W FET
#define w1PIN       14
#define w2PIN       4

// onbaord green LED D1
#define LEDPIN      5
// onbaord red LED D2
#define LED2PIN     1

uint8_t pins[5] = {redPIN, greenPIN, bluePIN, w1PIN, w2PIN}; // List of pins that you want to connect to pwm
HardwarePWM HW_pwm(pins, 5);

 // remember state so we can query it later
uint8_t state[5] = { 0 };

int MAX_PWM_DUTYCYCLE = HW_pwm.getMaxDuty();

uint16_t gamma(uint8_t input, float gamma)
{
    return pow(input / 255.0f, gamma) * MAX_PWM_DUTYCYCLE + 0.5;
}

void set(uint8_t r, uint8_t g, uint8_t b, uint8_t w1, uint8_t w2)
{
    set({r, g, b, w1, w2});
}

void set(const std::array<uint8_t, 5>& rgbww)
{
    for (int i = 0; i < sizeof(pins); i++) {
        HW_pwm.analogWrite(pins[i], gamma(rgbww[i]));
        state[i] = rgbww[i];
        //HW_pwm.analogWrite(pins[i], rgbww[i]);
    }

    //debugf("pwm max duty: %d", MAX_PWM_DUTYCYCLE);
    //debugf("gamma(%d) = %d", rgbww[0], gamma(rgbww[0]));
}

uint8_t get(int idx)
{
    if (idx >= 0 and idx < 5)
        return state[idx];
    else
        return 0;
}

} // namespace LED
