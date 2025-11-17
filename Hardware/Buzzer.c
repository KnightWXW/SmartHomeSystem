#include "stm32f10x.h"

/*
    蜂鸣器的触发方式为 低电平触发
*/

/**
 * @description: 蜂鸣器初始化
 * @return: {*}
 */
void Buzzer_Init(void)
{
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // 初始化 GPIO
    GPIO_InitTypeDef gpioStructure;
    gpioStructure.GPIO_Pin = GPIO_Pin_13;        // GPIO引脚,GPIO_Pin_13引脚
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz; // GPIO速度,50MHz
    gpioStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // GPIO模式,推挽输出模式
    GPIO_Init(GPIOC, &gpioStructure);

    GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET); // 初始为不鸣叫
}

/**
 * @description: Buzzer开始鸣叫
 * @return: {*}
 */
void Buzzer_ON(void)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
}

/**
 * @description: Buzzer停止鸣叫
 * @return: {*}
 */
void Buzzer_OFF(void)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
}