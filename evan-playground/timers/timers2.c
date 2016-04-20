#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//function prototype
void initTimer0(void);

// global variable declaration!
//volatile unsigned char myCounter;

int main(void)
{

//    myCounter = 1;
    initTimer0();

    DDRB |= 1<<PB5;

    sei();   //enable global interrupts

    while(1)     //stuck here forever :)
        {
        } 
    return 0;

} // end main

void initTimer0(void)
{
    TCCR1A = 0x00;                          // configure counter wave mode and compare mode
    TCCR1B = (1<<CS11)|(1<<WGM12);          // clock prescale 1/8, wave mode
    OCR1AH = 0x0F;                          // compare reg a = 4096
    OCR1AL = 0xFF;
    OCR1BH = 0x00;                          // compare reg b = 256
    OCR1BL = 0xFF;
    TIMSK1 = (1<<ICIE1)|(1<<OCIE1A)|(1<<OCIE1B); // enable interupts for a and b
    TIFR1 = 0x00;

}

ISR(TIMER1_COMPA_vect) {
    PORTB |= 1<<PB5;
}

ISR(TIMER1_COMPB_vect) {
    PORTB &= ~(1<<PB5);
}
