#pragma once

#include <stdint.h>

extern volatile uint32_t duty;
extern volatile uint32_t freq;

void make_noise(void);