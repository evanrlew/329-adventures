/* 
 * Contains the main that initializes all the peripherals and 
 * the main loop that monitors the gesture sensor
 */

#define F_CPU 16000000 			// define internal CLK speed

#include "WaveGen.h"
#include "UART.h"
#include "MIDI.h"
#include "ProxSensor.h"
#include "EnvGen.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

extern volatile MIDI_XFER_STATE midi_xfer_state;

int main(void)
{	
	DDRF |= (1<<PF1);
	usart_init(31250, F_CPU);
	init_sensor();
	initTimer1(); // timer for sine/sawtooth
	initTimer3(); // Envelope timer
	
	set_wave();
	Initialize_SPI_Master();
	sei();

	while (1) {
		if (midi_xfer_state == MIDI_WAITING) {
			monitor_sensor();
		}			
	}
	return 0;
}  // end main