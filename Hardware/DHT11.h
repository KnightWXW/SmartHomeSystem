#ifndef _DHT11_H_
#define _DHT11_H_

#include "stm32f10x.h"

void DHT11_IO_IN(void);
void DHT11_IO_OUT(void);
uint8_t DHT11_Init(void);
uint8_t DHT11_Read_Data(uint8_t *TempInt, uint8_t *TempDec, uint8_t *HumiInt, uint8_t *HumiDec);
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Check(void);
void DHT11_Reset(void);

#endif
