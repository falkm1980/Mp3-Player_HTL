/*
 * DisplayTest.c
 *
 * Created: 11/10/2019 12:03:48
 * Author : Markus
 */ 

#include "defines.h"
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

void InitUART()
{
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);// | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
	
	UBRR0 = 103;	// 103 = value @16MHz clock for 9600
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

	while(1) {
		SendString("Hallo");
		_delay_ms(2000);	
	}
    //
    //lcd_init(LCD_DISP_ON);
    //lcd_clrscr();
    //lcd_set_contrast(0x00);
    //lcd_gotoxy(4,1);
    //lcd_puts("Normal Size");
    //lcd_charMode(DOUBLESIZE);
    //lcd_gotoxy(0,4);
    //lcd_puts("  Double  \r\n   Size");
    //lcd_charMode(NORMALSIZE);
    //
    //#ifdef GRAPHICMODE
    //lcd_display();
    //#endif
    //for(;;){
	    ////main loop
    //}
    return 0;
}

