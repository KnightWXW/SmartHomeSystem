#ifndef DHT11_H
#define DHT11_H

#include "stm32f10x.h"

// 使用寄存器配置PA1引脚
#define DHT11_IO_IN()             \
    {                             \
        GPIOA->CRL &= 0XFFFFFF0F; \
        GPIOA->CRL |= 8 << 1;     \
    }
#define DHT11_IO_OUT()            \
    {                             \
        GPIOA->CRL &= 0XFFFFFF0F; \
        GPIOA->CRL |= 3 << 1;     \
    }

// IO操作函数
#define DHT11_DQ_OUT(X) GPIO_WriteBit(GPIOA, GPIO_Pin_1, X)
#define DHT11_DQ_IN GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)

uint8_t DHT11_Init(void);
uint8_t DHT11_Read_Data(uint8_t *tem, uint8_t *humi);
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Check(void);
void DHT11_Rst(void);

#endif
