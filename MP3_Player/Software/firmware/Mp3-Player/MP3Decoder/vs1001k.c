/*
 * vs1001k.c
 *
 * Created: 13.12.2011 17:14:22
 *  Author: Markus
 */ 
#include "vs1001k.h"
#include "../spi.h"
#include <avr/io.h>
#include <util/delay.h>

#define SELECT_MP3()	PORTB &= ~_BV(1)	/* CS = L */
#define	DESELECT_MP3()	PORTB |=  _BV(1)	/* CS = H */

#define WRITE_BYTE	0x02
#define READ_BYTE	0x03

#define DATAPORT_VS1001	PORTD
#define SDATA	PD0
#define DCLK	PD1
#define BSYNC	PD3

//void WriteData(uint8_t address, uint16_t data);

void InitVS1001k()
{
//	SendNulls(32);
	WriteData(0x00, 0x0004);
	
	_delay_ms(50);
	
	WriteData(0x03, 10000);	// set XTAL to 20MHz
	_delay_ms(500);
	
//	SendNulls(32);
}

void SendNulls(uint16_t number)
{
	while(number-- > 0)
	{
		WriteMP3DataByte(0x00);
	}
}

void SoftwareReset()
{
	WriteData(0x00, 0x0004);
	
	_delay_ms(50);
	
	WriteData(0x03, 10000);	// set XTAL to 20MHz
}

void WriteData(uint8_t address, uint16_t data)
{
	SELECT_MP3();
	
	xmit_spi(WRITE_BYTE);
	xmit_spi(address);
	
	uint8_t byte = data >> 8;
	xmit_spi(byte);
	byte = data & 0xFF;
	xmit_spi(byte);
	
	
	DESELECT_MP3();
}

uint16_t ReadData(uint8_t address)
{
	SELECT_MP3();
	
	xmit_spi(READ_BYTE);
	xmit_spi(address);
	
	uint16_t data = 0x00;
	uint8_t byte = 0x00;
	
	byte = rcv_spi();
	data = byte << 8;
	byte = rcv_spi();
	data |= byte;		
	
	DESELECT_MP3();

	return data;
	
}

// *****************************************************************************
void WriteMP3DataByte(uint8_t byte)
{
	uint8_t a;

	for (a=8; a>0; a--){					//das Byte wird Bitweise nacheinander Gesendet MSB First
		
		if (a== 8) {	// set BSYNC
			DATAPORT_VS1001 |= (1<<BSYNC);
		}
					
		if (bit_is_set(byte,(a-1))>0){		//Ist Bit a in Byte gesetzt
			DATAPORT_VS1001 |= (1<<SDATA); 	//Set Output High
		}
		else{
			DATAPORT_VS1001 &= ~(1<<SDATA); 	//Set Output Low
		}
		DATAPORT_VS1001 |= (1<<DCLK); 		//setzt Clock Impuls wieder auf (High)
		DATAPORT_VS1001 &= ~(1<<DCLK);		//erzeugt ein Clock Impuls (LOW)
		
		if (a==8) {	// reset BSYNC
			DATAPORT_VS1001 &= ~(1<<BSYNC);
		}
	}
	

	DATAPORT_VS1001 |= (1<<SDATA);				//setzt Output wieder auf High
}

void StartSineTest(void)
{
	WriteMP3DataByte(0x53);
	WriteMP3DataByte(0xEF);
	WriteMP3DataByte(0x6E);
	WriteMP3DataByte(62);	// 1kHz
	WriteMP3DataByte(0x00);
	WriteMP3DataByte(0x00);
	WriteMP3DataByte(0x00);
	WriteMP3DataByte(0x00);
}

void StopSineTest(void)
{
	WriteMP3DataByte(0x45);
	WriteMP3DataByte(0x78);
	WriteMP3DataByte(0x69);
	WriteMP3DataByte(0x74);
	WriteMP3DataByte(0x00);
	WriteMP3DataByte(0x00);
	WriteMP3DataByte(0x00);
	WriteMP3DataByte(0x00);
}