#define F_CPU 16000000 			//define internal CLK speed
#define MOSI 2  				// PB pin 2
#define SCK  1  				// PB pin 1
#define SS   0  				// PB pin 0

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

enum SPI_XFER_STATE {
	XFER_FINISHED,
	MSB_SENT,
	LSB_SENT
};

volatile enum SPI_XFER_STATE SPI_STATE = XFER_FINISHED;
volatile uint8_t SPI_MSB = 0;
volatile uint8_t SPI_LSB = 0;

void Initialize_SPI_Master(void);
void Transmit_SPI_Master();

int main(void)
{
	int count = 0;
	int val = 0;
	int incr = 0x100 / 16;
	
	sei();
	
	DDRB = 1<<MOSI | 1<<SCK | 1<<SS;	// make MOSI, SCK and SS outputs
	Initialize_SPI_Master();
	while(1)
	{
		for (count = 0; count < 16; count++) {
			val += incr;
			SPI_MSB = (val >> 8 & 0xF) | 0x70;
			SPI_LSB = val & 0xFF;
			Transmit_SPI_Master();
			_delay_ms(6);
		}
		
		for (count = 0; count < 16; count ++) {
			val -= incr;
			SPI_MSB = (val >> 8 & 0xF) | 0x70;
			SPI_LSB = val & 0xFF;
			Transmit_SPI_Master();
			_delay_ms(6);
		}
	}  // end while
	return 0;
}  // end main

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
	(0<<SPI2X) ; 			//Doubles SPI clock
	PORTB = 1 << SS;  		// make sure SS is high
}

void Transmit_SPI_Master(void) {
	
	if (SPI_STATE == XFER_FINISHED) {
		SPI_STATE = MSB_SENT;
		PORTB &= ~(1 << SS); 		  //Assert slave select (active low) 		
		SPDR = SPI_MSB;
	}
	else if (SPI_STATE == MSB_SENT) {
		SPI_STATE = LSB_SENT;
		SPDR = SPI_LSB;
	}
	else {
		SPI_STATE = XFER_FINISHED;
		PORTB |= 1 << SS;
	}
}

ISR(SPI_STC_vect) {
	cli();
	Transmit_SPI_Master();
	sei();
}