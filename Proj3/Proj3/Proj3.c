#define F_CPU 16000000 			// define internal CLK speed

#include "wave_gen.h"
#include "UART.h"
#include "MIDI.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void)
{	
	DDRC |= (1 << PC7);
	usart_init(31250, F_CPU);
	//initTimer1(); // timer for square
	initTimer3(); // timer for sine/sawtooth
	set_wave();
	
	//timer3_off();
	//timer3_on();
	sei();

	Initialize_SPI_Master();
	_delay_ms(50);
	while (1) {
//		timer3_off();
		make_noise();
	}
	return 0;
}  // end main