#define F_CPU 16000000 			// define internal CLK speed

#include "WaveGen.h"
#include "UART.h"
#include "MIDI.h"
#include "ProxSensor.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void)
{	
	DDRF |= (1<<PF1);
	usart_init(31250, F_CPU);
	init_sensor();
	initTimer1(); // timer for sine/sawtooth
	initTimer3();
	change_wave(SINE);
	set_wave();
	sei();

	Initialize_SPI_Master();
	while (1) {
		make_noise();
		monitor_sensor();
	}
	return 0;
}  // end main