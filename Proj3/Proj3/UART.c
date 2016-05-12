#include <stdlib.h>					// Standard C library
#include <avr/io.h>					// Input-output ports, special registers

#define F_CPU 16000000
#include <util/delay.h>

#define BAUD_PRESCALE 103
void usart_init(uint16_t baudin, uint32_t clk_speedin)
{
	uint32_t ubrr = (clk_speedin/16UL)/baudin-1;
	UBRR1H = (unsigned char)(ubrr>>8) & 0xF;
	UBRR1L = (unsigned char)ubrr;
	/*UBRR0H = (BAUD_PRESCALE>>8);
	UBRR0L = BAUD_PRESCALE;*/
	/* Enable receiver and transmitter */
	UCSR1B = (1<<RXEN1)|(1<<TXEN1);
	/* Set frame format: 8data, 1stop bit */
	UCSR1C = (1<<USBS1)|(3<<UCSZ10);
	UCSR1A &= ~(1<<U2X1);
}

/*the send function will put 8bits on the trans line. */
void usart_send( uint8_t data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1)) );
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

/* the receive data function. Note that this a blocking call
Therefore you may not get control back after this is called 
until a much later time. It may be helpfull to use the 
istheredata() function to check before calling this function
	@return 8bit data packet from sender
*/
uint8_t  usart_recv(void)
{
	/* Wait for data to be received */
	while ( !(UCSR1A & (1<<RXC1)) )
	;
	/* Get and return received data from buffer */
	return UDR1;
}

/* function check to see if there is data to be received
	@return true is there is data ready to be read */
uint8_t  usart_istheredata(void)
{
	 return (UCSR1A & (1<<RXC1));
}