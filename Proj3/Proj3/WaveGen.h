#pragma once
#include <stdint.h>

// Used for the attack
extern volatile uint8_t env_mod;

enum FG_STATE {
	SQUARE,
	SINE,
	SAWTOOTH,
	TRIANGLE
};

//void initTimer1(void);
void set_wave(void);
void change_wave(enum FG_STATE state);
void next_wave(void);
void prev_wave(void);
void initTimer1(void);

void timer1_on(void);
void timer1_off(void);

void set_DAC_data(uint16_t data);
void clear_DAC(void);
void Initialize_SPI_Master(void);
void Transmit_SPI_Master(void);