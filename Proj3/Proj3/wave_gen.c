#include <avr/io.h>
#include <avr/interrupt.h>

#include "wave_gen.h"

#define F_CPU 16000000

#define MOSI 2  				// PB pin 2
#define SCK  1  				// PB pin 1
#define SS   0  				// PB pin 0

#define TIMER1_PRESCALE 64
#define NUM_SAW_POINTS 16
#define NUM_SINE_POINTS 16
#define NUM_TRI_POINTS 16

static uint16_t SINE_VALS[NUM_SINE_POINTS] = {2048, 2832, 3496, 3940, 4095, 3940, 3496, 2832, 2048, 1264, 600, 156, 0, 156, 600, 1264};

volatile enum SPI_XFER_STATE spi_state = XFER_FINISHED;
volatile uint8_t spi_msb = 0;
volatile uint8_t spi_lsb = 0;

volatile uint32_t duty = 10;
volatile uint32_t freq = 1000;
volatile uint32_t velocity_scale = 1;

volatile enum FG_STATE fg_state = SAWTOOTH;

/*
void initTimer1(void)
{
	TCCR1A = 0x00;                          // configure counter wave mode and compare mode
	TCCR1B = (1<<WGM12);								// clock off initially
	TIMSK1 = (1<<ICIE1)|(1<<OCIE1A)|(1<<OCIE1B); // enable interrupts for a and b
	TIFR1 = 0x00;
}*/

void initTimer3(void)
{
	TCCR1A = 0x00;                // configure counter wave mode and compare mode
	TCCR1B = (1<<WGM12);          // clock off , wave mode
	TIMSK1 = (1<<OCIE1A); // enable interrupts for a
	TIFR1 = 0x00;
}
/*
void timer1_on(void) {
	TCCR1B |= (1<<CS11)|(1<<CS10); // clock prescale 1/64
	TIMSK1 = (1<<ICIE1)|(1<<OCIE1A)|(1<<OCIE1B);
}

void timer1_off(void) {
	TCCR1B &= ~(1 << CS10 | 1 << CS11 | 1 << CS12);
	TIMSK1 = 0;
}*/

void timer3_on(void) {
	TCCR1B |= (1<<CS10); // no prescale on the clock
	TIMSK1 = (1<<OCIE1A);
	TIFR1 = 0x00;
}	
	
void timer3_off(void) {
	TCCR1B &= ~(1 << CS10);
	TIMSK1 = 0;
}

void set_wave(void) {
//	uint32_t sq_ticks_per_period = F_CPU / TIMER1_PRESCALE / freq;
//	uint32_t ticks_per_duty = sq_ticks_per_period * duty / 100;
	
	uint16_t saw_ticks_per_step;
	uint16_t sin_ticks_per_step;
	uint16_t tri_ticks_per_step;
	/*
	// calculate and set frequency
	OCR1AH = sq_ticks_per_period >> 8 & 0xFF;
	OCR1AL = sq_ticks_per_period & 0xFF;
	
	// calculate and set duty
	OCR1BH = ticks_per_duty >> 8 & 0xFF;
	OCR1BL = ticks_per_duty & 0xFF;*/

	if (fg_state == SAWTOOTH) {
		saw_ticks_per_step = F_CPU / freq / NUM_SAW_POINTS;
		OCR1AH = saw_ticks_per_step >> 8;
		OCR1AL = saw_ticks_per_step & 0xFF;
	}	
	if (fg_state == SINE) {
		sin_ticks_per_step = F_CPU / freq / NUM_SINE_POINTS;
		OCR1AH = sin_ticks_per_step >> 8;
		OCR1AL = sin_ticks_per_step & 0xFF;		
	}
	else if (fg_state == TRIANGLE) {
		tri_ticks_per_step = F_CPU / freq / NUM_TRI_POINTS;
		OCR1AH = sin_ticks_per_step >> 8;
		OCR1AL = sin_ticks_per_step & 0xFF;
	}
}

void set_DAC_data(uint16_t data) {
	spi_msb = (data >> 8 & 0xF) | 0x70;
	spi_lsb = data & 0xFF;
}

void Initialize_SPI_Master(void)
{
	DDRB = 1<<MOSI | 1<<SCK | 1<<SS; // make MOSI, SCK and SS outputs
		
	SPCR = (1<<SPIE) | 		//No interrupts
	(1<<SPE) | 				//SPI enabled
	(0<<DORD) | 			//send MSB first
	(1<<MSTR) | 			//master
	(0<<CPOL) | 			//clock idles low
	(0<<CPHA) | 			//sample falling clock edge
	(0<<SPR1) | (0<<SPR0) ; //clock speed
	
	SPSR = (0<<SPIF) | 		//SPI interrupt flag
	(0<<WCOL) | 			//Write collision flag
	(1<<SPI2X) ; 			//Doubles SPI clock
	PORTB = 1 << SS;  		// make sure SS is high
}

void Transmit_SPI_Master(void) {
	
	if (spi_state == XFER_FINISHED) {
		spi_state = MSB_SENT;
		PORTB &= ~(1 << SS); 		  //Assert slave select (active low) 		
		SPDR = spi_msb;
	}
	else if (spi_state == MSB_SENT) {
		spi_state = LSB_SENT;
		SPDR = spi_lsb;
	}
	else {
		spi_state = XFER_FINISHED;
		PORTB |= 1 << SS;
	}
}


ISR(SPI_STC_vect) {
	Transmit_SPI_Master();
}
/*
ISR(TIMER1_COMPA_vect) {
	set_DAC_data(0xFFF);
	Transmit_SPI_Master();
}

ISR(TIMER1_COMPB_vect) {
	set_DAC_data(0);
	Transmit_SPI_Master();
*/

ISR(TIMER1_COMPA_vect) {
	uint16_t dac_val = 0;

	if (fg_state == SAWTOOTH) {	
		static uint16_t saw_val = 0;

		if (saw_val == 0xFFF) {
			saw_val = 0;
		}			
		else {
			saw_val += 4096 / NUM_SAW_POINTS;
		}			

		if (saw_val >= 0x1000) { // dac can only take in 0xFFF and lower
			saw_val = 0xFFF;
		}			
		
		dac_val = saw_val;
	}
	else if (fg_state == SINE) {
		static uint16_t sine_cnt = 0;
		
		if (sine_cnt == NUM_SINE_POINTS) {
			sine_cnt = 0;
		}

		dac_val = SINE_VALS[sine_cnt];
		sine_cnt ++;
	}
	else if (fg_state == TRIANGLE) {
		static int direction = 1;
		static uint16_t tri_val = 0;
						
		tri_val += direction * 2 * 4096 / NUM_TRI_POINTS;
		
		if (tri_val >= 4096) {
			tri_val = 0xFFF;
			direction = -1;
		} 
		else if (tri_val <  2 * 4096 / NUM_TRI_POINTS - 1) {
			tri_val = 0;
			direction = 1;
		}
		
		dac_val = tri_val;
	}

	dac_val = dac_val >> velocity_scale;
	set_DAC_data(dac_val);
	Transmit_SPI_Master();
}

void clear_DAC(void) {
	set_DAC_data(2048);
	Transmit_SPI_Master();
}
