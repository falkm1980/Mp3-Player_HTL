/*
 * vs1001k.c
 *
 * Created: 13.12.2011 17:14:22
 *  Author: Markus
 */ 
#include "vs1011.h"
#include "../spi.h"
#include <avr/io.h>
#include <util/delay.h>

#define SELECT_MP3()	PORTB &= ~_BV(2)	/* CS = L */
#define	DESELECT_MP3()	PORTB |=  _BV(2)	/* CS = H */

#define SELECT_MP3_DATA()	PORTB &= ~_BV(0)	/* CS = L */
#define	DESELECT_MP3_DATA()	PORTB |=  _BV(0)	/* CS = H */

#define SELECT_SPI_DEBUG()	PORTD &= ~_BV(3)	/* CS = L */
#define	DESELECT_SPI_DEBUG()	PORTD |= _BV(3)	/* CS = H */

#define WRITE_BYTE	0x02
#define READ_BYTE	0x03

#define XTAL_VALUE	0x9800	// value for the register for a XTAL=12.288MHz
//#define XTAL_VALUE	0x30D4	// value for the register for a XTAL=25MHZ (12500 dec = 30D4 hex)
#define VS1002_NATIVE_MODE	0x0800
#define SOFT_RESET	0x0004
#define SM_TESTS	0x0020
#define VOLUME_REGISTER	0x0B

//#define DATAPORT_VS1001	PORTD
//#define SDATA	PD0
//#define DCLK	PD1
//#define BSYNC	PD3
//
//void WriteData(uint8_t address, uint16_t data);
uint8_t iVolume = 0x00;
const uint8_t iVolumeStep = 2;

void InitVS1011e()
{
	loop_until_bit_is_set(PINB, PB1);
	
	WriteCommand(0x00, VS1002_NATIVE_MODE | SM_TESTS); // enable VS1002 native SPI mode
	_delay_ms(1);
	
	WriteCommand(0x03, XTAL_VALUE);	// set XTAL to 12.288MHz
	
	_delay_ms(1);
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
	WriteCommand(0x00, 0x0004);
	
	_delay_ms(50);
	
	InitVS1011e();
	
//	WriteCommand(0x03, XTAL_VALUE);	// set XTAL to 12.288MHz
}

/// After a Hardware reset you have to init the MP3-Chip again
void HardwareReset()
{
	PORTC &= ~_BV(3); // Pin low
	_delay_ms(100);
	DESELECT_MP3();
	DESELECT_MP3_DATA();
	PORTC |=  _BV(3); // Pin high
	_delay_ms(100);	// just for security
}

void WriteCommand(uint8_t address, uint16_t data)
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

uint16_t ReadCommand(uint8_t address)
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
	SELECT_MP3_DATA();
	
	xmit_spi(byte);
	
	DESELECT_MP3_DATA();
	
	
	/// this is the old style for the VS1001k where no dedicated CS for the dataport was available
	//////////////////////////////////////////////////////////////////////////
	//uint8_t a;
//
	//for (a=8; a>0; a--){					//das Byte wird Bitweise nacheinander Gesendet MSB First
		//
		//if (a== 8) {	// set BSYNC
			//DATAPORT_VS1001 |= (1<<BSYNC);
		//}
					//
		//if (bit_is_set(byte,(a-1))>0){		//Ist Bit a in Byte gesetzt
			//DATAPORT_VS1001 |= (1<<SDATA); 	//Set Output High
		//}
		//else{
			//DATAPORT_VS1001 &= ~(1<<SDATA); 	//Set Output Low
		//}
		//DATAPORT_VS1001 |= (1<<DCLK); 		//setzt Clock Impuls wieder auf (High)
		//DATAPORT_VS1001 &= ~(1<<DCLK);		//erzeugt ein Clock Impuls (LOW)
		//
		//if (a==8) {	// reset BSYNC
			//DATAPORT_VS1001 &= ~(1<<BSYNC);
		//}
	//}
	//
//
	//DATAPORT_VS1001 |= (1<<SDATA);				//setzt Output wieder auf High
}

void StartSineTest(void)
{
//	HardwareReset();
	
	//uint16_t data = SM_TESTS | VS1002_NATIVE_MODE;
	//WriteCommand(0x00, data);
	//WriteCommand(0x03, XTAL_VALUE);
	//
//	_delay_ms(5);

//	while(1)
	{
//	SELECT_MP3_DATA();	
	WriteMP3DataByte(0x53);
	WriteMP3DataByte(0xEF);
	WriteMP3DataByte(0x6E);
	WriteMP3DataByte(0x44);	// 1kHz
	WriteMP3DataByte(0x00);
	WriteMP3DataByte(0x00);
	WriteMP3DataByte(0x00);
	WriteMP3DataByte(0x00);
	
//	DESELECT_MP3_DATA();
	}
}

void StopSineTest(void)
{
//	SELECT_MP3_DATA();
	WriteMP3DataByte(0x45);
	WriteMP3DataByte(0x78);
	WriteMP3DataByte(0x69);
	WriteMP3DataByte(0x74);
	WriteMP3DataByte(0x00);
	WriteMP3DataByte(0x00);
	WriteMP3DataByte(0x00);
	WriteMP3DataByte(0x00);
//	DESELECT_MP3_DATA();
}

void WriteDebugByte(uint8_t b1, uint8_t b2)
{
	SELECT_SPI_DEBUG();
	
	xmit_spi(b1);
	xmit_spi(b2);
	
	DESELECT_SPI_DEBUG();
	
}

void VolumePlus()
{
	uint8_t newValue = iVolume - iVolumeStep;
	if (newValue > iVolume) // üerblauf bei subtraktion
	{
		newValue = 0x00;
	}
	iVolume = newValue;
	
	uint16_t volume = 0x00;
	volume = (newValue << 8) | newValue;
	
	WriteCommand(VOLUME_REGISTER, volume);
}

void VolumeMinus()
{
	uint8_t newValue = iVolume + iVolumeStep;
	if (newValue < iVolume) // üerblauf bei subtraktion
	{
		newValue = 0xFE;
	}
	
	if (newValue > 0xFE)
		newValue = 0xFE;

	iVolume = newValue;
	
	uint16_t volume = 0x00;
	volume = (newValue << 8) | newValue;
	
	WriteCommand(VOLUME_REGISTER, volume);	
}
