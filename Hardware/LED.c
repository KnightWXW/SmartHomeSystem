#include "stm32f10x.h"
#include "LED.h"

LED_INFO g_Led_Info = {0};

/**
 * @description: LED初始化
 * @return: {*}
 */
void LED_Init(void)
{
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // 初始化 GPIO
    GPIO_InitTypeDef gpioStructure;
    gpioStructure.GPIO_Pin = GPIO_Pin_14;        // GPIO引脚,PIN14 引脚
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz; // GPIO速度,50MHz
    gpioStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // GPIO模式,推挽输出模式
    GPIO_Init(GPIOC, &gpioStructure);
}

/**
 * @description: LED 灭灯
 * @return: {*}
 */
void LED_OFF(void)
{
    // LED 亮 为低电平触发,灯灭时需要引脚输出高电平
    GPIO_SetBits(GPIOC, GPIO_Pin_14);
    g_Led_Info.LedStatus = 0;
}

/**
 * @description: LED 亮灯
 * @return: {*}
 */
void LED_ON(void)
{
    // LED 亮 为低电平触发,灯亮时需要引脚输出低电平
    GPIO_ResetBits(GPIOC, GPIO_Pin_14);
    g_Led_Info.LedStatus = 1;
}

/**
 * @description: LED 开关灯
 * @return: {*}
 */
void LED_Turn(void)
{
    if (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_14) == 1)
    {
        LED_ON();
        g_Led_Info.LedStatus = 1;
    }
    else
    {
        LED_OFF();
        g_Led_Info.LedStatus = 0;
    }
}