#pragma once
#include <stdint.h>

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
void initTimer3(void);

//void timer1_on(void);
//void timer1_off(void);
void timer3_on(void);
void timer3_off(void);

void set_DAC_data(uint16_t data);
void clear_DAC(void);
void Initialize_SPI_Master(void);
void Transmit_SPI_Master(void);