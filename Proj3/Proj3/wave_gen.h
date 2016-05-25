#pragma once
#include <stdint.h>

extern volatile uint8_t env_mod;

enum SPI_XFER_STATE {
	XFER_FINISHED,
	MSB_SENT,
	LSB_SENT
};

enum FG_STATE {
	SQUARE,
	SAWTOOTH,
	SINE,
	TRIANGLE
};

//void initTimer1(void);
void set_wave(void);
void change_wave(enum FG_STATE state);
void initTimer1(void);

//void timer1_on(void);
//void timer1_off(void);
void timer1_on(void);
void timer1_off(void);

void set_DAC_data(uint16_t data);
void clear_DAC(void);
void Initialize_SPI_Master(void);
void Transmit_SPI_Master(void);