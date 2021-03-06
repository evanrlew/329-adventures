/* 
 * Processes any incoming MIDI data and translates it
 * into a analog waveform
 */

#include "MIDI.h"
#include "UART.h"
#include "WaveGen.h"
#include "EnvGen.h"
#include <avr/interrupt.h>

#include <avr/io.h>

#define OFF_CMD 0x80
#define ON_CMD 0x90
#define CMD_MASK 0xF0
#define CHANNEL_MASK 0xF
#define CHANNEL_NUM 0

volatile MIDI_XFER_STATE midi_xfer_state = MIDI_WAITING;

volatile uint8_t midi_cmd;
volatile uint8_t midi_note;
volatile uint8_t midi_vel;


static uint16_t MIDI_TO_FREQ[] = {28,29,31,33,35,37,39,41,44,46,49,52,55,58,62,65,69,73,78,82,87,92,98,104,110,117,123,131,139,147,156,165,175,185,196,208,220,233,247,262,277,294,311,330,349,370,392,415,440,466,494,523,554,587,622,659,698,740,784,831,880,932,988,1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,1865,1976,2093,2217,2349,2489,2637,2794,2960,3136,3322,3520,3729,3951,4186};

void make_noise() {
	uint8_t uart_data;
	uint8_t key;
	uint8_t velocity;

	if ((midi_cmd & CMD_MASK) == ON_CMD) {
		key = midi_note;
		key = key < 33? 0 : (key - 33);
		freq = MIDI_TO_FREQ[key];
			
		velocity = midi_vel;
		change_velocity_scale(velocity);
			
		set_wave();			
		start_atk_timer(100);
		timer1_on();
	}
	else if ((midi_cmd & CMD_MASK) == OFF_CMD) {
		timer1_off();
		clear_DAC();
	}
	else {
		timer1_off();
		clear_DAC();
	}
}

void change_velocity_scale(uint8_t velocity) {
	velocity_scale = 6 - velocity / 24;
}

/* 
 * Handles reciving all three command bytes
 */
ISR(USART1_RX_vect) {
	if (midi_xfer_state == MIDI_WAITING) {
		midi_cmd = usart_recv();
		midi_xfer_state = MIDI_CMD_RCVD;
	} 
	else if (midi_xfer_state == MIDI_CMD_RCVD) {
		midi_note = usart_recv();
		midi_xfer_state = MIDI_NOTE_RCVD;
	}
	else if (midi_xfer_state == MIDI_NOTE_RCVD) {
		midi_vel = usart_recv();
		midi_xfer_state = MIDI_WAITING;
		make_noise();
	}
	else {
		midi_xfer_state = MIDI_WAITING;
	}

}