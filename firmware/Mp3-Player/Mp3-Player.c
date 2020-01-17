/*
* Mp3_Player.c
*
* Created: 11.12.2011 21:18:14
*  Author: Markus
*/
#define F_CPU 16000000UL
#include <avr/io.h>
#include "lcd.h"
#include <util/delay.h>
#include "MMC/pff.h"
#include "MP3Decoder/vs1011.h"

//#define KEY_BACK	PD0
//#define KEY_PLAY	PD1
#define KEY_BACK	PD2
#define KEY_PLAY	PD4
#define KEY_FORWARD	PD3

#define BACK			1
#define PLAY			2
#define FORWARD			3
#define VOLUME_MINUS	4
#define VOLUME_PLUS		5

uint8_t iBackDown = 0;
uint8_t iPlayDown = 0;
uint8_t iForwardDown = 0;
uint8_t iLastValue = 0xFF;
uint8_t iVolumePressed = 0;

void InitKeyboard()
{
	DDRD = ~((1<<KEY_PLAY) | (1<<KEY_BACK) | (1<<KEY_FORWARD));
	// the CS for the card is also in PORTD but this PIN is initialized in spi.c->init_spi
	// therefore it is important that InitKeyboard is called before init_spi
}

//void InitUART()
//{
	//UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	//UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
	//
	//UBRR0 = 25;
//}

void InitUART()
{
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);// | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
	
	UBRR0 = 51;	// Bluetooth module has 19200 baud, 51 = value @16MHz clock for 19200
}

/*
* tries to read a byte from the serial port (bluetooth)
*/
//uint8_t ReadSerialByte()
//{
	//uint8_t rcv = 0x00;
	//
	//while((UCSR0A & (1 << RXC0)) == 0x00)
	//{
	//}
	//
	//rcv = UDR0;
	//return rcv;
//}

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

uint8_t ReadSerialByte()
{
	uint8_t rcv = 0x00;
	
	if ((UCSR0A & (1 << RXC0)) != 0x00)
	{
		rcv = UDR0;
		switch(rcv)
		{
			case 0x01:
			case 0x31:
				return 1;
			
			case 0x02:
			case 0x32:
				return 2;
				
			case 0x03:
			case 0x33:
				return 3;
				
			default:
				return rcv;
		}
	}
	else
		return 0x00;
}

uint8_t GetKey()
{
	//uint8_t serialData = 0x00;
	//serialData = ReadSerialByte();
	//if (serialData != 0x00)
		//return serialData;
	
	uint8_t portD = PIND;
	if (portD == iLastValue)
		return 0;
	
	iLastValue = portD;
	// PD3, S4
	if (iPlayDown && (portD & 0x10))
	{
		iPlayDown = 0;
		if (iVolumePressed != 0)
		{
			iVolumePressed = 0;
			return 0;
		}
		
		return 2;
	}

	// check Volume
	if (iPlayDown)
	{
		// PD2, S2
		if ((portD & 0x08) == 0x00)
		{
			iVolumePressed = 1;
			return 5;
		}
		// PD4, S3
		if ((portD & 0x04) == 0x00)
		{
			iVolumePressed = 1;
			return 4;
		}
	}

	// PD2, S2
	if (iBackDown && (portD & 0x04))
	{
		iBackDown = 0;
		return 1;
	}	

	// PD4, S3
	if (iForwardDown && (portD & 0x08))
	{
		iForwardDown = 0;
		return 3;
	}
	
	
	// PD2, S2
	if ((portD & 0x04) == 0x00)
	{
		if (!iBackDown)
		{
			iBackDown = 1;
			return 0;			
		}
	}
	// PD3, S4
	if ((portD & 0x10) == 0x00)
	{
		if (!iPlayDown)
		{
			iPlayDown = 1;
			return 0;
		}
	}
	//PD4, S3
	if ((portD & 0x08) == 0x00)
	{
		if (!iForwardDown)
		{
			iForwardDown = 1;
			return 0;
		}
	}
		
	return 0;
}


uint8_t GetNumberFiles()
{
	DIR dir;
	char* path = "";
	BYTE res = 0;
	res = pf_opendir(&dir, path);
	uint8_t numFiles = 0;
	
	if (res == FR_OK)
	{
		while (1)
		{
			FILINFO fno;
			res = pf_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0)
			{
				break;
			}
			numFiles++;
		}
	}

	return numFiles;
}

uint8_t PlayFile(uint8_t numFile2Play)
{
	BYTE res = 0;

	DIR dir;
	char* path = "";
	res = pf_opendir(&dir, path);
	FILINFO fno;
	
	if (res == FR_OK)
	{
		uint8_t index = 0;
		do
		{
			res = pf_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0)
			{
				break;
			}
			
			index++;
		} while(index < numFile2Play);
	}
	else
	{
		return 0;
	}
	
	SoftwareReset();	// resets the MP3-Chip
	
	SendString(fno.fname);

	res = pf_open(fno.fname);
	if (res)
	{
		StartSineTest();
		_delay_ms(2000);
		return 0;
	}

	const WORD sizeBlock = 512;
	BYTE buff[sizeBlock];
	WORD br=0;
	uint8_t serialData = 0x00;
	uint16_t powerOffRepeat = 0;
	
	while(1)
	{
		res = pf_read(buff, sizeBlock, &br);    // Read data to the buff[]
		if (res != 0)
		{
			break;
		}
		else
		{
			
			if (br == 0)
			{
				break;
			}
			
			uint16_t bytesWritten = 0;
			do
			{
				uint16_t bytes2Write = 0;
				if ((br - bytesWritten) > 32)
				bytes2Write = 32;
				else
				bytes2Write = br - bytesWritten;
				
				for(int i=0; i<32; i++)
				{
					WriteMP3DataByte(buff[bytesWritten+i]);
				}
				
				bytesWritten += bytes2Write;
				
				loop_until_bit_is_set(PINC, PC1);
				
				switch(GetKey())
				{
					case BACK:
						_delay_ms(100);
						return 1;
					case PLAY:
						_delay_ms(100);
						return 2;
					case FORWARD:
						_delay_ms(100);
						return 3;
					case VOLUME_MINUS:
						VolumeMinus();
						_delay_ms(50);
						break;
					case VOLUME_PLUS:
						VolumePlus();
						_delay_ms(50);
						break;
					default:
						if ((PIND & 0x04) == 0)
						{
							powerOffRepeat++;
							if (powerOffRepeat > 1000)
							{
								while((PIND & 0x04) == 0);
								PORTD = ~(1<<PD5);
							}
						}
						else
						{
							powerOffRepeat = 0;
						}
						
				}
			} while (bytesWritten < br);
			
			if (br < sizeBlock)
			{
				_delay_ms(100);
				return 0;
			}
		}
	}
	
	return 0;
}



int main(void)
{
	_delay_ms(10);

	DDRD = 0x22; // PD5 is the output for the power switch
	PORTD = 0xFF;	// all pins high + enable the pull up resistors for the 3 Keys (PD0..PD2)

	
	DDRC = 0b00101001;
	// PC0 -> XDCS (Out)
	// PC1 -> DREQ (In)
	// PC2 -> 
	// PC3 -> XRESET (Out)
	// PC4 -> SDA (??)
	// PC5 -> SCL (??)
	// PC6 -> 
	// PC7 ->

	PORTC = 0xFF;
	_delay_ms(50);
	
	char cText[128] = {0};

//	loop_until_bit_is_set(PIND, PD0);
	
	InitKeyboard();
//	PORTD |= 0x07;	// enable the pull up resistors for the 3 Keys
	
	InitUART();
	SendString("UART running....");
	//while (1) {
		//SendString("Hallo Welt");
		//_delay_ms(2000);
	//}
	
	BYTE res = 0;
	FATFS fs;			/* File system object */
	FRESULT fResult = -1;
	


	res = disk_initialize(); // this also init's the SPI-Interface


	_delay_ms(10);
	HardwareReset();
	SendString("Hardware reset...");


	InitVS1011e();
	SendString("InitVS1011e...");

	_delay_ms(50);

	//while(1)
	//{
		//StartSineTest();
		//_delay_ms(5000);
		//StopSineTest();
		//_delay_ms(2000);
	//}

	fResult = pf_mount(&fs);

	sprintf(cText, "pf_mount: %d",fResult);

	SendString(cText);
	memset(cText, 0, 128);

	if (fResult == 0)
	{
	PORTD = 0xFF;
		uint8_t numFiles = GetNumberFiles();
		uint8_t file2Play = 1;

		sprintf(cText, "Files: %d",numFiles);

		SendString(cText);
		memset(cText, 0, 128);
	
		// in Main function
		while(1)
		{
			uint8_t ret = PlayFile(file2Play);
			switch(ret)
			{
				case BACK:
					file2Play--;
					if (file2Play < 1)
					file2Play = numFiles;
					sprintf(cText, "Back: %d",file2Play);
					SendString(cText);
					memset(cText, 0, 128);
					break;
				case PLAY:
					SendString("Play 1");

					_delay_ms(500);
					//if ((PIND & 0x10) == 0 ) // switch MP3-Player off
					//{
						//PORTD = ~(1<<PD5);
					//}
					while(GetKey() != PLAY)
					{
					}
					_delay_ms(100);
					SendString("Play 2");
					break;
				case FORWARD:
					file2Play++;
					if (file2Play > numFiles)
						file2Play = 1;
					sprintf(cText, "Forward: %d",file2Play);
					SendString(cText);
					memset(cText, 0, 128);
					break;
				//case VOLUME_MINUS:
					//VolumeMinus();
					//break;
				//case VOLUME_PLUS:
					//VolumePlus();
					//break;
				default:
					file2Play++;
					if (file2Play > numFiles)
					file2Play = 1;
					sprintf(cText, "Default: %d",file2Play);
					SendString(cText);
					memset(cText, 0, 128);
					break;
			}
		}
	}
	else
	{
		pf_mount(0);

		while(1)
		{
		}
	}
	
	_delay_ms(2000);
	
	while(1)
	{
	}
}