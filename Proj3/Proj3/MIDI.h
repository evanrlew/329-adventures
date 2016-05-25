#pragma once

#include <stdint.h>

extern volatile uint32_t duty;
extern volatile uint32_t freq;
extern volatile uint8_t velocity_scale;
extern volatile uint8_t env_mod;

void make_noise(void);
void change_velocity_scale(uint8_t velocity);