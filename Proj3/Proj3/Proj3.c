#define F_CPU 16000000 			// define internal CLK speed

#include "wave_gen.h"
#include "UART.h"
#include "MIDI.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void)
{	
	usart_init(31250, F_CPU);
	initTimer1(); // timer for sine/sawtooth
	change_wave(TRIANGLE);
	set_wave();
		
	sei();

	Initialize_SPI_Master();
	while (1) {
		make_noise();
	}
	return 0;
}  // end main