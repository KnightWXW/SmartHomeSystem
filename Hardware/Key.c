#include "stm32f10x.h"
#include "Delay.h"

/*
    按键释放时 为 高电平，此时电平为 GPIO 的电平
    按键按下时 为 低电平，此时电平为 GND  的电平
*/

/**
 * @description: Key 初始化
 * @return: {*}
 */
void Key_Init(void)
{
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 初始化 GPIO
    GPIO_InitTypeDef gpioStructure;
    gpioStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_6; // GPIO引脚,PIN 引脚
    gpioStructure.GPIO_Speed = GPIO_Speed_50MHz;                   // GPIO速度,50MHz
    gpioStructure.GPIO_Mode = GPIO_Mode_IPU;                       // GPIO模式,上拉输入模式
    GPIO_Init(GPIOA, &gpioStructure);
}

/**
 * @description: 获取按键的键码值
 * @param {uint8_t} KeyMode 按键连续
 *   0: 不支持连续按键
 *   1: 支持连续按键
 * @return: {*}
 */
uint16_t Key_GetNum(uint8_t KeyMode)
{
    static uint16_t keyUp = 1; // 按键按下释放标志位 0[释放] 1[按下]
    // 支持连续按键,连续按下时手动将标志位置1
    if (KeyMode == 1)
    {
        keyUp = 1;
    }
   
    if (keyUp == 1 && (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0 || GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0 || GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0))
    {
        Delay_ms(20); // 延时消抖
        keyUp = 0;
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)
        {
            Delay_ms(20); // 延时消抖
            while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0);   // 等待按键松手
            Delay_ms(20); // 延时消抖
            return 1;
        }
        else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
        {
            Delay_ms(20); // 延时消抖
            while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0);   // 等待按键松手
            Delay_ms(20); // 延时消抖
            return 2;
        }
        else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0)
        {
            Delay_ms(20); // 延时消抖
            while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0);   // 等待按键松手
            Delay_ms(20); // 延时消抖
            return 3;
        }
    }
    // 不支持连续按键
    else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 1 && GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 1 && GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 1)
    {
        keyUp = 1;
    }
    return 0;
}
