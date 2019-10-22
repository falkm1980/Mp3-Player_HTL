/*
 * GPIOTest.c
 *
 * Created: 22/10/2019 20:00:00
 * Author : Markus
 */ 

 #define F_CPU 16000000ul

#include <avr/io.h>
#include <avr/delay.h>

int main(void)
{
	DDRD |= (1 << PD5) | (1 << PD6) | (1 << PD7);
	 
    /* Replace with your application code */
    while (1) 
    {
		if (PIND & 0x10)
			PORTD = 0xE0;
		else
			PORTD = 0x00;
    }
}

