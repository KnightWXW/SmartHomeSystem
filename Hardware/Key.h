#ifndef _KEY_H_
#define _KEY_H_

void Key_Init(uint16_t GPIO_Pin);
uint16_t Key_GetNum(GPIO_TypeDef *GPIO, uint16_t GPIO_Pin);

#endif