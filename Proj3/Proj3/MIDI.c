#include "MIDI.h"
#include "UART.h"
#include "wave_gen.h"

#define OFF_CMD 0x80
#define ON_CMD 0x90
#define CHANNEL_MASK 0xF
#define CHANNEL_NUM 0

static uint16_t MIDI_TO_FREQ[] = {28,29,31,33,35,37,39,41,44,46,49,52,55,58,62,65,69,73,78,82,87,92,98,104,110,117,123,131,139,147,156,165,175,185,196,208,220,233,247,262,277,294,311,330,349,370,392,415,440,466,494,523,554,587,622,659,698,740,784,831,880,932,988,1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,1865,1976,2093,2217,2349,2489,2637,2794,2960,3136,3322,3520,3729,3951,4186};

void make_noise() {
	uint8_t uart_data;
	uint8_t key;
	uint8_t velocity;

	if (usart_istheredata()) {
		uart_data = usart_recv(); // receive midi data - note on/off
		
		if ((uart_data & ON_CMD) && (uart_data & CHANNEL_MASK == CHANNEL_NUM)) {
			timer3_on();
			key = usart_recv();
				
			key = key < 21? 0: key - 21;
			
			freq = MIDI_TO_FREQ[key];
			set_wave();
			
			velocity = usart_recv();	
		}
		else if ((uart_data & OFF_CMD) && (uart_data & CHANNEL_MASK == CHANNEL_NUM)) {
			timer3_off();
			usart_recv();
			usart_recv();
		}
		else {
			usart_recv();
			usart_recv();
		}
		
		 
		// process midi
		// generate appropriate waveform
		// set freq and/or duty
		//freq = 5000;
		
		//set_wave();
	}
}