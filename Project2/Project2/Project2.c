#define F_CPU 16000000 			//define internal CLK speed
#define MOSI 2  				// PB pin 2
#define SCK  1  				// PB pin 1
#define SS   0  				// PB pin 0

#define TIMER1_PRESCALE 64
#define NUM_SAW_POINTS 64
#define NUM_SINE_POINTS 64

#define WAVE_TYPE_BTN 0
#define FREQ_VAL_BTN 1
#define DUTY_CYCLE_BTN 4

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

enum SPI_XFER_STATE {
	XFER_FINISHED,
	MSB_SENT,
	LSB_SENT
};

enum FG_STATE {
	SQUARE,
	SAWTOOTH,
	SINE
};
 
static uint16_t SINE_VALS[NUM_SINE_POINTS] = {2048,2249,2448,2643,2832,3013,3186,3347,3496,3631,3751,3854,3940,4008,4057,4086,4095,4086,4057,4008,3940,3854,3751,3631,3496,3347,3186,3013,2832,2643,2448,2249,2048,1847,1648,1453,1264,1083,910,749,600,465,345,242,156,88,39,10,0,10,39,88,156,242,345,465,600,749,910,1083,1264,1453,1648,1847};

volatile enum SPI_XFER_STATE spi_state = XFER_FINISHED;
volatile uint8_t spi_msb = 0;
volatile uint8_t spi_lsb = 0;

volatile uint32_t duty = 10;
volatile uint32_t freq = 100;

volatile enum FG_STATE fg_state = SAWTOOTH;

void initTimer1(void);
void set_wave(void);
void initTimer3(void);

void timer1_on(void);
void timer1_off(void);
void timer3_on(void);
void timer3_off(void);

void change_wave_type(void);
void change_frequency(void);
void change_duty(void);

void set_DAC_data(uint16_t data);
void Initialize_SPI_Master(void);
void Transmit_SPI_Master(void);

void initButtons(void);
void pollButtons(void);

int main(void)
{	
	initButtons();
	initTimer1(); // timer for square
	initTimer3(); // timer for sine/sawtooth
	set_wave();
	
	timer3_on();
	sei();
	
	DDRB = 1<<MOSI | 1<<SCK | 1<<SS;	// make MOSI, SCK and SS outputs
	Initialize_SPI_Master();
	
	while(1) {
		pollButtons();
	}
	return 0;
}  // end main

void initButtons(void) {
	DDRD &= ~(1<<WAVE_TYPE_BTN | 1<<FREQ_VAL_BTN);
	PORTD |= 1<<WAVE_TYPE_BTN | 1<<FREQ_VAL_BTN;
	DDRF &= ~(1<<DUTY_CYCLE_BTN);
	PORTF |= 1<<DUTY_CYCLE_BTN;
}

void pollButtons(void) {
	int i;
	
	if (!(PIND & 1<<WAVE_TYPE_BTN)) { 
		change_wave_type();
		_delay_ms(100);
	}
	if (!(PINF & 1<<DUTY_CYCLE_BTN)) {
		change_duty();
		_delay_ms(100);
	}
	if (!(PIND & 1<<FREQ_VAL_BTN)) {
		change_frequency();
		_delay_ms(100);
	}
}

void change_wave_type() {

	if (fg_state == SQUARE) {
		timer1_off();
		fg_state = SAWTOOTH;
		timer3_on();
	}		
	else if (fg_state == SAWTOOTH) {
		fg_state = SINE;
	}
	else if (fg_state == SINE) {
		timer3_off();
		fg_state = SQUARE;
		timer1_on();
	} else {
		fg_state = SINE;
	}
	set_wave();
}

void change_frequency(void) {
	if (freq == 500) {
		freq = 100;
	}
	else {
		freq += 100;
	}
	set_wave();
}

void change_duty(void) {
	if (duty == 90) {
		duty = 10;
	}
	else {
		duty += 10;
	}
	set_wave();
}

void initTimer1(void)
{
	TCCR1A = 0x00;                          // configure counter wave mode and compare mode
	TCCR1B = (1<<WGM12);								// clock off initially
	//OCR1AH = 0x01;                          // compare reg a = 500
	//OCR1AL = 0xF4;
	//OCR1BH = 0x00;                          // compare reg b = 125
	//OCR1BL = 0x7D;	
	TIMSK1 = (1<<ICIE1)|(1<<OCIE1A)|(1<<OCIE1B); // enable interupts for a and b
	TIFR1 = 0x00;
}

void initTimer3(void)
{
	TCCR3A = 0x00;                          // configure counter wave mode and compare mode
	TCCR3B = (1<<WGM32);          // clock off , wave mode
	TIMSK3 = (1<<ICIE3)|(1<<OCIE3A); // enable interupts for a and b
	TIFR3 = 0x00;
}

void timer1_on(void) {
	TCCR1B |= (1<<CS11)|(1<<CS10); // clock prescale 1/64, wave mode
	TIMSK1 = (1<<ICIE1)|(1<<OCIE1A)|(1<<OCIE1B);
}

void timer1_off(void) {
	TCCR1B &= ~(1 << CS10 | 1 << CS11 | 1 << CS12);
	TIMSK1 = 0;
}

void timer3_on(void) {
	TCCR3B |= (1<<CS30);          // clock no prescale
	TIMSK3 = (1<<ICIE3)|(1<<OCIE3A);
}	
	
void timer3_off(void) {
	TCCR3B &= ~(1 << CS30);
	TIMSK3 = 0;
}

void set_wave(void) {
	uint32_t sq_ticks_per_period = F_CPU / TIMER1_PRESCALE / freq;
	uint32_t ticks_per_duty = sq_ticks_per_period * duty / 100;
	
	uint16_t saw_ticks_per_step;
	uint16_t sin_ticks_per_step;
	
	// calculate and set frequency
	OCR1AH = sq_ticks_per_period >> 8 & 0xFF;                          // compare reg a
	OCR1AL = sq_ticks_per_period & 0xFF;
	
	// calculate and set duty
	OCR1BH = ticks_per_duty >> 8 & 0xFF;                          // compare reg b
	OCR1BL = ticks_per_duty & 0xFF;

	if (fg_state == SAWTOOTH) {
		saw_ticks_per_step = F_CPU / freq / NUM_SAW_POINTS;
		OCR3AH = saw_ticks_per_step >> 8;
		OCR3AL = saw_ticks_per_step & 0xFF;
	}	
	else if (fg_state == SINE) {
		sin_ticks_per_step = F_CPU / freq / NUM_SINE_POINTS;
		OCR3AH = sin_ticks_per_step >> 8;
		OCR3AL = sin_ticks_per_step & 0xFF;		
	}
}

void set_DAC_data(uint16_t data) {
	spi_msb = (data >> 8 & 0xF) | 0x70;
	spi_lsb = data & 0xFF;
}

void Initialize_SPI_Master(void)
{
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
	cli();
	Transmit_SPI_Master();
	sei();
}

ISR(TIMER1_COMPA_vect) {
	set_DAC_data(0xFFF);
	Transmit_SPI_Master();
}

ISR(TIMER1_COMPB_vect) {
	set_DAC_data(0);
	Transmit_SPI_Master();
}

ISR(TIMER3_COMPA_vect) {
	uint16_t dac_val = 0;
	
	if (fg_state == SAWTOOTH) {	
		static uint16_t saw_val = 0;
		
		if (saw_val == 0xFFF)
			saw_val = 0;
		else
			saw_val += 4096 / NUM_SAW_POINTS;
	
		if (saw_val >= 0x1000) // dac can only take in 0xFFF and lower
			saw_val = 0xFFF;
		
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
				
	set_DAC_data(dac_val);
	Transmit_SPI_Master();
}
