/*
 * SerialTest.c
 *
 * Created: 22/10/2019 19:54:10
 * Author : Markus
 */ 
 #define F_CPU 16000000ul

#include <avr/io.h>
#include <util/delay.h>

void InitUART()
{
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);// | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
	
	UBRR0 = 103;	// Bluetooth module has 9600 baud, 103 = value @16MHz clock for 9600
}

void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) )
	;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void SendString(uint8_t* pData)
{
	uint8_t len = strlen(pData);
	if (len < 1)
	return;
	
	char c = 0x00;
	
	while(c = *pData++)
	USART_Transmit(c);
	
	USART_Transmit('\r\n');
}


int main(void)
{
	InitUART();

    /* Replace with your application code */
    while (1) 
    {
		SendString("Hallo Welt");
		_delay_ms(2000);
    }
}

