#ifndef DHT11_H
#define DHT11_H

#include "stm32f10x.h"

void DHT11_IO_IN(void);
void DHT11_IO_OUT(void);
uint8_t DHT11_Init(void);
uint8_t DHT11_Read_Data(uint8_t *tem, uint8_t *humi);
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Check(void);
void DHT11_Reset(void);

#endif
