#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//function prototype
void initTimer0(void);

// global variable declaration!
volatile unsigned char myCounter;

int main(void)
{

    myCounter = 1;
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
    TCCR0A = 0x00;  // timer overflow mode
    TCCR0B = 0x05;  // timer clk = system clk / 1024
    TIFR0 = 0x01;   // clear previous timer overflow
    TIMSK0 = 0x01;  // timer overflow interrupt enabled
}


ISR(TIMER0_OVF_vect)
{
    if (myCounter == 7){
        myCounter = 0;
PORTB |= 1<<PB5;
}
    else{
        myCounter++;
    PORTB &= ~(1<<PB5);
}
}   

