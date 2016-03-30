#define F_CPU 16000000UL  	// tells compiler the freq of your processor
#include <avr/io.h>    	// registers locations and some other things
#include <util/delay.h>    // software delay functions

int main(void){
	DDRB |= (1<<PB5);        		//  digital pin13/PORTB5 = output

	while(1){            		// infinite loop
		PORTB |= (1<<PB5);    	// write a 1 to B5
		_delay_ms(1000);    	//Wait 1 second
		PORTB &= ~(1<<PB5);        //Turn led off
		_delay_ms(1000);    	//Wait another second
	}
	
	return 1;
}
