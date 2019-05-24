/*
 * spi.h
 *
 * Created: 12.12.2011 21:47:23
 *  Author: Markus
 */ 


#ifndef SPI_H_
#define SPI_H_

#include "integer.h"

extern void init_spi (void);
extern void xmit_spi (BYTE);
extern BYTE rcv_spi (void);
extern void dly_100us(void);


#endif /* SPI_H_ */