#include "MIDI.h"
#include "UART.h"

void make_noise() {
	uint8_t uart_data;

	if (usart_istheredata()) {
		uart_data = usart_recv(); // receive midi data - note on/off
		
		if (uart_data )
		
		// process midi
		// generate appropriate waveform
		// set freq and/or duty
		set_wave();
	}
}