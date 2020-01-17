/*
 * PowerOnOff.c
 *
 * Created: 11/15/2019 1:21:44 PM
 * Author : markus
 */ 
#define F_CPU 16000000ul

#include <avr/io.h>
#include "avr/delay.h"

int main(void)
{
	DDRD = (1<< PD5) | (1 << PD6);
	PORTD = 0x00;
	
	PORTD |= (1 << PD4); // Pull up Res. PD4
	

	PORTD |= (1 << PD5); // switch permanent on
	

	for (;;)
	{	
		int low = 0;
	
		/* Replace with your application code */
		while (1)
		{
			low = ((PIND & (1 << PD4)) == 0);

			_delay_ms(1000);
			int tmp = ((PIND & (1 << PD4)) == 0);
			
			if (low > 0 && tmp > 0)
			{
				PORTD &= ~(1<<PD5); // switch off
				while(1) {}
			}
		}		
	}

}

