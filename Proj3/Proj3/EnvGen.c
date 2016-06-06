/* 
 * Creates the timer that generates the waveform envelope
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#define ATK_STEPS 6

volatile uint8_t env_mod = 0;

void initTimer3(void) {
	TCCR3A = 0;
	TCCR3B = 1 << WGM32;
	TIMSK3 = 1 << OCIE3A;
	TIFR3 = 0;
}

void start_atk_timer(uint16_t atk_time) {
	uint32_t ticks_per_step = atk_time / ATK_STEPS * 16;
	
	env_mod = ATK_STEPS;
	
	TCCR3B |= (1<<CS30 | 1<<CS32);
	TIMSK3 = 1<<OCIE3A;
	TIFR3 = 0;
	OCR3AH = ticks_per_step >> 8;
	OCR3AL = ticks_per_step & 0xFF;
}

void atk_timer_off(void) {
	TIMSK3 = 0;
}

ISR(TIMER3_COMPA_vect) {
	env_mod--;
	if (!env_mod) {
		atk_timer_off();
	}
}