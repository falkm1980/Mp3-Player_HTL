/*
 * spi.c
 *
 * Created: 12.12.2011 21:47:15
 *  Author: Markus
 */ 
#include "spi.h"
#include <util/delay.h>
#include <avr/io.h>

void init_spi (void)
{

	DDRB = (1<<PB1) | (1<<PB2) | (1<<PB3) |(1<<PB5);	// this is for the MP3-Chip
	// PB0 -> NC
	// PB1 -> CS SD-Card (Out)
	// PB2 -> XCS (Out)
	// PB3 -> MOSI (Out)
	// PB4 -> MISO (In)
	// PB5 -> SCK (Out)

	DDRD |= (1<<PD4);	// CS Card
	
	PORTB = ~((1<<PB4));
	
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); //Enable SPI, SPI in Master Mode
	
	SPSR = 0x01; // fosz/2
}

void xmit_spi (BYTE byte)
{
	SPDR = byte;
	
	while(!(SPSR & (1<<SPIF)))
	{
	}	
}

BYTE rcv_spi (void)
{
	char Byte = 0;
	SPDR = 0xff;
	
	while(!(SPSR & (1<<SPIF)))
	{
	}
	
	Byte = SPDR;
	
	return Byte;
}

void dly_100us(void)
{
	_delay_us(1);
}