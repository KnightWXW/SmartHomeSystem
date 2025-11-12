#ifndef _BUZZER_H_
#define _BUZZER_H_

void Buzzer_Init(uint16_t GPIO_Pin);
void Buzzer_ON(GPIO_TypeDef *GPIO, uint16_t GPIO_Pin);
void Buzzer_OFF(GPIO_TypeDef *GPIO, uint16_t GPIO_Pin);

#endif