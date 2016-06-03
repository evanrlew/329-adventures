#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000

#include "WaveGen.h"
#include "EnvGen.h"
#define MOSI 2  				// PB pin 2
#define SCK  1  				// PB pin 1
#define SS   0  				// PB pin 0

#define TIMER1_PRESCALE 64
#define NUM_SAW_POINTS 16
#define NUM_SINE_POINTS 16
#define NUM_TRI_POINTS 16

static uint16_t SINE_VALS[NUM_SINE_POINTS] = {2048, 2832, 3496, 3940, 4095, 3940, 3496, 2832, 2048, 1264, 600, 156, 0, 156, 600, 1264};
static uint16_t TRI_VALS[NUM_TRI_POINTS] = {0, 512, 1024, 1536, 2048, 2560, 3072, 3584, 4095, 3584, 3072, 2560, 2049, 1536, 1025, 512};

volatile enum SPI_XFER_STATE spi_state = XFER_FINISHED;
volatile uint8_t spi_msb = 0;
volatile uint8_t spi_lsb = 0;

volatile uint32_t duty = 25;
volatile uint32_t freq = 1000;
volatile uint32_t velocity_scale = 1;

volatile enum FG_STATE fg_state = SQUARE;

void initTimer1(void)
{
	TCCR1A = 0;                // configure counter wave mode and compare mode
	TCCR1B = (1<<WGM12) | (1<<CS10);    // clock on, no prescale , wave mode
	TIMSK1 = 0; // disable interrupts for timer1 
	TIFR1 = 0;
}

void timer1_on( void )
{
	TIMSK1 = 1<<OCIE1A;

	if (fg_state == SQUARE) 
		TIMSK1 |= (1<<OCIE1B);

	TIFR1 = 0x00;
}	
	
void timer1_off( void )
{
	TIMSK1 = 0;
}

void change_wave(enum FG_STATE state) {
	fg_state = state;
	
	if (state == SQUARE) {
		TCCR1B |= (1<<CS11) | (1<<CS10);
		
		// need OCB for duty cycle
		TIMSK1 |= 1<<OCIE1B;
	}
	else {
		// CS10 bit is still on
		TCCR1B &= ~(1<<CS11);
		
		// turn off output compare B
		TIMSK1 &= ~(1<<OCIE1B);
	}	
}

void next_wave(void) {
	if (fg_state == TRIANGLE) {
		fg_state = SQUARE;
	}		
	else { 
		fg_state++;
	}		
	change_wave(fg_state);
}

void prev_wave(void) {
	if (fg_state == SQUARE) {
		fg_state = TRIANGLE;
	}
	else {
		fg_state--;
	}
	change_wave(fg_state);
}

void set_wave(void) {
	uint32_t sq_ticks_per_period = F_CPU / TIMER1_PRESCALE / freq;
	uint32_t ticks_per_duty = sq_ticks_per_period * duty / 100;
	
	uint16_t saw_ticks_per_step;
	uint16_t sin_ticks_per_step;
	uint16_t tri_ticks_per_step;

	if (fg_state == SQUARE) {	
		// calculate and set frequency
		OCR1AH = sq_ticks_per_period >> 8 & 0xFF;
		OCR1AL = sq_ticks_per_period & 0xFF;
	
		// calculate and set duty
		OCR1BH = ticks_per_duty >> 8 & 0xFF;
		OCR1BL = ticks_per_duty & 0xFF;
	}	
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
	if (fg_state == TRIANGLE) {
		tri_ticks_per_step = F_CPU / freq / NUM_TRI_POINTS;
		OCR1AH = tri_ticks_per_step >> 8;
		OCR1AL = tri_ticks_per_step & 0xFF;
	}
}

void set_DAC_data(uint16_t data) {
	spi_msb = (data >> 8 & 0xF) | 0x70;
	spi_lsb = data & 0xFF;
}

void Initialize_SPI_Master(void)
{
	DDRB = 1<<MOSI | 1<<SCK | 1<<SS; // make MOSI, SCK and SS outputs
		
	SPCR = (0<<SPIE) | 		//interrupts enabled
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

	PORTB &= ~(1 << SS); 			//Assert slave select (active low) 		
	SPDR = spi_msb;
	while (!(SPSR & (1 << SPIF)));
	SPDR = spi_lsb;
	while (!(SPSR & (1 << SPIF)));
	PORTB |= 1 << SS;
}


ISR(TIMER1_COMPA_vect) {
	uint16_t dac_val = 0;

	if (fg_state == SQUARE) {
		dac_val = 0xFFF;
	}
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
		sine_cnt++;
	}
	else if (fg_state == TRIANGLE) {
		static uint16_t tri_cnt = 0;
		
		if (tri_cnt == NUM_TRI_POINTS) {
			tri_cnt = 0;
		}
		
		dac_val = TRI_VALS[tri_cnt];
		tri_cnt++;
	}

	dac_val = dac_val / (velocity_scale + env_mod);
	set_DAC_data(dac_val);
	Transmit_SPI_Master();
}

ISR(TIMER1_COMPB_vect) {
	set_DAC_data(0);
	Transmit_SPI_Master();
}

void clear_DAC(void) {
	set_DAC_data(0);
	Transmit_SPI_Master();
}