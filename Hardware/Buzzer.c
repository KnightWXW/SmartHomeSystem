#include "stm32f10x.h"

/*
    蜂鸣器的触发方式为 低电平触发
*/

/**
 * @description: 蜂鸣器初始化
 * @param {uint16_t} GPIO_Pin
 * @return: {*}
 */
void Buzzer_Init(uint16_t GPIO_Pin)
{
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // 初始化 GPIO
    GPIO_InitTypeDef gpioStructure;
    gpioStructure.GPIO_Pin = GPIO_Pin;           // GPIO引脚,GPIO_Pin引脚
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz; // GPIO速度,50MHz
    gpioStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // GPIO模式,推挽输出模式
    GPIO_Init(GPIOC, &gpioStructure);
}

/**
 * @description: Buzzer开始鸣叫
 * @param {uint16_t} GPIO_Pin
 * @return: {*}
 */
void Buzzer_ON(GPIO_TypeDef *GPIO, uint16_t GPIO_Pin)
{
    GPIO_WriteBit(GPIO, GPIO_Pin, Bit_RESET);
}

/**
 * @description: Buzzer停止鸣叫
 * @param {uint16_t} GPIO_Pin
 * @return: {*}
 */
void Buzzer_OFF(GPIO_TypeDef *GPIO, uint16_t GPIO_Pin)
{
    GPIO_WriteBit(GPIO, GPIO_Pin, Bit_SET);
}