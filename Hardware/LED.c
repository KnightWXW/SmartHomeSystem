#include "stm32f10x.h"

LED_INFO g_Led_Info = {0};

/**
 * @description: LED初始化
 * @param {uint16_t} GPIO_Pin
 * @return: {*}
 */
void LED_Init(uint16_t GPIO_Pin)
{
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 初始化 GPIO
    GPIO_InitTypeDef gpioStructure;
    gpioStructure.GPIO_Pin = GPIO_Pin;           // GPIO引脚,PIN 引脚
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz; // GPIO速度,50MHz
    gpioStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // GPIO模式,推挽输出模式
    GPIO_Init(GPIOA, &gpioStructure);
}

/**
 * @description: LED 灭灯
 * @param {GPIO_TypeDef*} GPIO
 * @param {uint16_t} GPIO_Pin
 * @return: {*}
 */
void LED_OFF(GPIO_TypeDef *GPIO, uint16_t GPIO_Pin)
{
    // LED 亮 为低电平触发,灯灭时需要引脚输出高电平
    GPIO_SetBits(GPIO, GPIO_Pin);
    g_Led_Info.LedStatus = 0;
}

/**
 * @description: LED 亮灯
 * @param {GPIO_TypeDef*} GPIO
 * @param {uint16_t} GPIO_Pin
 * @return: {*}
 */
void LED_ON(GPIO_TypeDef *GPIO, uint16_t GPIO_Pin)
{
    // LED 亮 为低电平触发,灯亮时需要引脚输出低电平
    GPIO_ResetBits(GPIO, GPIO_Pin);
    g_Led_Info.LedStatus = 1;
}

/**
 * @description: LED 开关灯
 * @param {GPIO_TypeDef} *GPIO
 * @param {uint16_t} GPIO_Pin
 * @return: {*}
 */
void LED_Turn(GPIO_TypeDef *GPIO, uint16_t GPIO_Pin)
{
    if (GPIO_ReadOutputDataBit(GPIO, GPIO_Pin) == 1)
    {
        LED_ON(GPIO, GPIO_Pin);
        g_Led_Info.LedStatus = 1;
    }
    else
    {
        LED_OFF(GPIO, GPIO_Pin);
        g_Led_Info.LedStatus = 0;
    }
}