/*
 * Processes data on the gesture sensor and interprets
 * left and right swipes.
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdlib.h>					// Standard C library

#include "ProxSensor.h"
#include "WaveGen.h"

void init_sensor() {
	DDRF = 0x3;
		
	TWBR = 0x12; // 100kHz SCL
	TWSR = 0x01; // Prescalar of 4
		
	DDRD |= (1<<PD0) | (1<<PD1);
	PORTD |= (1<<PD0) | (1<<PD1);
}

void monitor_sensor() {
	uint8_t data;
	ProxStatus status;
	GestureType gesture;

	PORTF ^= 1;
	_delay_ms(50);
	status = data_available();
	
	if (status == GEST_AVAIL) {
		_delay_ms(50);
		gesture = readGesture();
		if (gesture == RIGHT_SWIPE) {
			next_wave();
		}
		else if (gesture == LEFT_SWIPE) {
			prev_wave();
		}
	}	
}

//writes data value to register with address regAdd
//return data value written on success, -1 on failure
void write_prox_sensor(uint8_t regAdd, uint8_t data) {

	// send start condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	// wait to see of start condition has been transmitted
	while( !(TWCR & (1<<TWINT)) );

	// check if status is no "start"
	if( (TWSR & 0xF8) != TW_START ){
		exit(-1); // error has occured
	}

	// slave address to write to
	TWDR = (0x10 << 1) | TW_WRITE;
	TWCR = (1<<TWINT) | (1<<TWEN);

	while( !(TWCR & (1<<TWINT)) );

	if( (TWSR & 0xF8) != TW_MT_SLA_ACK ) {
		exit(-1);
	}

	// register to write to
	TWDR = regAdd;
	TWCR = (1<<TWINT) | (1<<TWEN);

	while( !(TWCR & (1<<TWINT)) );

	if( (TWSR & 0xF8) != TW_MT_DATA_ACK ){
		exit(-1);
	}


	// write data to register
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);

	while( !(TWCR & (1<<TWINT)) );

	if( (TWSR & 0xF8) != TW_MT_DATA_ACK ) {
		exit(-1);
	}

	// transmit STOP
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

//reads data value from register with address regAdd
//return data value read on success, -1 on failure
int read_prox_sensor(uint8_t regAdd) {
	int data;

	// send start condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	// wait to see of start condition has been transmitted
	while( !(TWCR & (1<<TWINT)) );

	// check if status is no "start"
	if( TW_STATUS != TW_START ) {
		return 0; // error has occured
	}

	// slave address to write to
	TWDR = (0x10<<1) | TW_WRITE;

	// send device address
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wait to see of start condition has been transmitted
	while( !(TWCR & (1<<TWINT)) );

	// check for aknowledgement
	if( TW_STATUS != TW_MT_SLA_ACK ){
		return 0;
	}

	// set device register address
	TWDR = regAdd;

	//send device register address
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wait to see of start condition has been transmitted
	while( !(TWCR & (1<<TWINT)) );

	// check for aknowledgement
	if( TW_STATUS != TW_MT_DATA_ACK ){
		return 0;
	}

	// repeated start
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	// wait to see of start condition has been transmitted
	while( !(TWCR & (1<<TWINT)) );

	// check if status is no "repeated start"
	if( TW_STATUS != TW_REP_START ){
		return 0;
	}

	// slave address to read from
	TWDR = (0x10<<1) | TW_READ;

	// send address
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wait for address to be sent
	while( !(TWCR & (1<<TWINT)) );

	// check for ack
	if( TW_STATUS != TW_MR_SLA_ACK ){
		return 0;
	}

	TWCR = (1<<TWINT) | (1<<TWEN);

	// wait for data to be sent
	while( !(TWCR & (1<<TWINT)) );

	// read data
	data = TWDR;

	// transmit STOP
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	
	return data;
}

/*
 * returns 1 if gesture is ready to be read. 0 otherwise.
 */
ProxStatus data_available()
{
    uint8_t status;
    
    /* Read STATUS register and extract SWP bit */
    if ( !(status = read_prox_sensor(ZX_STATUS))) {
        return 0;
    }
	
	// checks the gesture flag of the status register
    if (status & 0b00000100) { 
        return GEST_AVAIL;
    }
	
	// checking hover and hover move
	if (status & 0b00001000) {
		return HOVER_AVAIL;
	}

	if (status & 0b00010000) {
		return HOVER_MV_AVAIL;
	}

	// checks the position flag of the status register
	if (status & 0b00000001) {
		return POS_AVAIL;
	}
	
    return NONE;
}


/*
 * returns a number corresponding to  a gesture. 0xFF on error.
 */
GestureType readGesture()
{
    uint8_t gesture;
    
    /* Read GESTURE register and return the value */
    if ( !(gesture = read_prox_sensor(ZX_GESTURE)) ) {
        return NO_GESTURE;
    }

    switch ( gesture ) {
        case RIGHT_SWIPE:
            return RIGHT_SWIPE;
        case LEFT_SWIPE:
            return LEFT_SWIPE;
        case UP_SWIPE:
            return UP_SWIPE;
        default:
            return NO_GESTURE;
    }
}