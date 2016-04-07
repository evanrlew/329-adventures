#include <avr/io.h>    	// registers locations and some other things
#define F_CPU 16000000UL  	// tells compiler the freq of your processor
#include <util/delay.h>    // software delay functions

void lcd_wr_cmd(uint8_t input) {
	PORTB = 0b00000100; // E = 1
	PORTD = input;
	_delay_us(1);
	PORTB = 0b00000000; // E = 0
}

void lcd_wr_data(uint8_t input) {
	PORTB = 0b00000001; // RS = 1
	_delay_us(1);
	
	PORTD = input;
	_delay_us(1);
	
	PORTB = 0b00000101; // RS = 1, E = 1
	_delay_us(1);
	
	//PORTB = 0b00000001; // RS = 1, E = 0
	//_delay_us(1);
	
	PORTB = 0b00000000; // RS = 0, E = 0
	_delay_us(1);
}

void lcd_write_string(char* inputString) {
	while(*inputString) {
		switch(*inputString) {
			case 'A': lcd_wr_data(0b01000001); break;
			case 'B': lcd_wr_data(0b01000010); break;
			case 'C': lcd_wr_data(0b01000011); break;
			case 'D': lcd_wr_data(0b01000100); break;
			case 'E': lcd_wr_data(0b01000101); break;
			case 'H': lcd_wr_data(0b01001000); break;
			case 'L': lcd_wr_data(0b01001100); break;
			case 'O': lcd_wr_data(0b01001111); break;
			case '!': lcd_wr_data(0b00100001); break;			
	
			default: lcd_wr_data(0b1111111); break;
		}
		_delay_us(100); // Delay after each write
		inputString++;
	}
}

int main(void) {
	DDRD = 0xFF;
	DDRB = 0b00000111;
	
	_delay_ms(40);

	lcd_wr_cmd(0b00111000);
	_delay_us(50);
	
	lcd_wr_cmd(0b00001111); // turn on display
	_delay_us(50);
	
	lcd_wr_cmd(0b00000001); // clear display
	_delay_ms(5);
	
	lcd_wr_cmd(0b00000110); // set entry mode
	_delay_ms(1);
	
	//lcd_wr_cmd(0b10000000);
	//lcd_wr_cmd(0b00011100); // move the cursor thing
		
	lcd_wr_cmd(0b10000000);
	_delay_us(100);
	
//	lcd_wr_data(0x41);
	
	lcd_write_string("HELLO!");
	while(2);

	return 1;
}
