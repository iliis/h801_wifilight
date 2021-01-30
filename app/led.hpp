#pragma once

#include <array>
#include <inttypes.h>

namespace LED
{

uint16_t gamma(uint8_t input, float gamma = 2.8);

void set(uint8_t r, uint8_t g, uint8_t b, uint8_t w1, uint8_t w2);
void set(const std::array<uint8_t, 5>& rgbww);

} // namespace LED
