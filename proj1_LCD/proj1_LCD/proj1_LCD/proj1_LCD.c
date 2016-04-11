#include <avr/io.h>    	// registers locations and some other things
#define F_CPU 16000000UL  	// tells compiler the freq of your processor
#include <util/delay.h>    // software delay functions
#include <avr/interrupt.h>

void lcd_wr_cmd(uint8_t input) {
	PORTB |= 0b00000100; // E = 1
	PORTD = input;
	_delay_us(1);
	PORTB &= ~(1 << 2); // E = 0
}

void lcd_wr_data(uint8_t input) {
	PORTB |= 0b00000001; // RS = 1
	_delay_us(1);
	
	PORTD = input;
	_delay_us(1);
	
	PORTB |= 0b00000101; // RS = 1, E = 1
	_delay_us(1);
	
	//PORTB = 0b00000001; // RS = 1, E = 0
	//_delay_us(1);
	
	PORTB &= ~(5); // RS = 0, E = 0
	_delay_us(100); // Delay after each write
}

void lcd_write_string(char* inputString) {
	while(*inputString) {
		lcd_wr_data(*inputString);
		inputString++;
	}
}

int main(void) {
	DDRD = 0xFF;
	DDRB = 0b00000111;
	
	_delay_ms(40);

	lcd_wr_cmd(0b00111000);
	_delay_us(50);
	
	lcd_wr_cmd(0b00001100); // turn on display
	_delay_us(50);
	
	lcd_wr_cmd(0b00000001); // clear display
	_delay_ms(5);
	
	lcd_wr_cmd(0b00000110); // set entry mode
	_delay_ms(1);
			
	lcd_write_string("Hello World!");

	while (2) {
		if (PINB ^ 1 << PB3) {
			lcd_wr_cmd(0b00000001); // clear display
			_delay_ms(5);
			
			lcd_write_string("dA king");
		}
	}
	
	return 1;
}
