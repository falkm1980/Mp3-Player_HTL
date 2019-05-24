/*
 * vs1001k.h
 *
 * Created: 13.12.2011 17:14:09
 *  Author: Markus
 */ 


#ifndef VS1001K_H_
#define VS1001K_H_
#include <avr/io.h>

extern void InitVS1001k(void);

extern void StartSineTest(void);
extern void StopSineTest(void);
extern void WriteData(uint8_t address, uint16_t data);
extern uint16_t ReadData(uint8_t address);
extern void WriteMP3DataByte(uint8_t byte);
extern void SoftwareReset();

extern void SendNulls(uint16_t number);

#endif /* VS1001K_H_ */