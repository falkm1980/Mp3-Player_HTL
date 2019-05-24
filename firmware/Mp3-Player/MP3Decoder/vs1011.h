/*
 * vs1001k.h
 *
 * Created: 13.12.2011 17:14:09
 *  Author: Markus
 */ 


#ifndef VS1011E_H_
#define VS1011E_H_
#include <avr/io.h>

extern void InitVS1011e(void);

extern void StartSineTest(void);
extern void StopSineTest(void);
extern void WriteData(uint8_t address, uint16_t data);
extern uint16_t ReadData(uint8_t address);
extern void WriteMP3DataByte(uint8_t byte);
extern void SoftwareReset();
extern void HardwareReset();

extern void SendNulls(uint16_t number);

extern void WriteDebugByte(uint8_t b1, uint8_t b2);

extern void VolumePlus();
extern void VolumeMinus();

#endif /* VS1011E_H_ */