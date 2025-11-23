#include "Usart.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

/**
 * @description: 串口1初始化[连接串口测试]
 * @param {unsigned int} baud 设定的波特率
 * @return: {*}
 */
void Usart1_Init(unsigned int baud)
{
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // GPIOA
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // USART1

    // 初始化 GPIO
    GPIO_InitTypeDef gpioInitStruct;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;   // GPIO模式,复用推挽输出模式
    gpioInitStruct.GPIO_Pin = GPIO_Pin_9;         // GPIO引脚,PIN9 引脚
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz; // GPIO速度,50MHz
    GPIO_Init(GPIOA, &gpioInitStruct);

    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // GPIO模式,浮空输入模式
    gpioInitStruct.GPIO_Pin = GPIO_Pin_10;            // GPIO引脚,PIN10 引脚
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;     // GPIO速度,50MHz
    GPIO_Init(GPIOA, &gpioInitStruct);

    USART_InitTypeDef usartInitStruct;
    usartInitStruct.USART_BaudRate = baud;                                      // 波特率
    usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 接收和发送
    usartInitStruct.USART_Parity = USART_Parity_No;                             // 无校验
    usartInitStruct.USART_StopBits = USART_StopBits_1;                          // 1位停止位
    usartInitStruct.USART_WordLength = USART_WordLength_8b;                     // 8位数据位
    USART_Init(USART1, &usartInitStruct);

    USART_Cmd(USART1, ENABLE); // 使能串口

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 使能接收中断

    NVIC_InitTypeDef nvicInitStruct;
    nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;         // 选择配置NVIC的 USART1 线
    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;           // 指定NVIC线路使能
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0; // 指定NVIC线路的抢占优先级为0
    nvicInitStruct.NVIC_IRQChannelSubPriority = 2;        // 指定NVIC线路的响应优先级为2
    NVIC_Init(&nvicInitStruct);
}

/**
 * @description: 串口2初始化[链接ESP8266]
 * @param {unsigned int} baud 设定的波特率
 * @return: {*}
 */
void Usart2_Init(unsigned int baud)
{
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // GPIOA
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // USART2

    // 初始化 GPIO
    GPIO_InitTypeDef gpioInitStruct;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;   // GPIO模式,复用推挽输出模式
    gpioInitStruct.GPIO_Pin = GPIO_Pin_2;         // GPIO引脚,PIN2 引脚
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz; // GPIO速度,50MHz
    GPIO_Init(GPIOA, &gpioInitStruct);

    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // GPIO模式,浮空输入模式
    gpioInitStruct.GPIO_Pin = GPIO_Pin_3;             // GPIO引脚,PIN3 引脚
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;     // GPIO速度,50MHz
    GPIO_Init(GPIOA, &gpioInitStruct);

    USART_InitTypeDef usartInitStruct;
    usartInitStruct.USART_BaudRate = baud;                                      // 波特率
    usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 接收和发送
    usartInitStruct.USART_Parity = USART_Parity_No;                             // 无校验
    usartInitStruct.USART_StopBits = USART_StopBits_1;                          // 1位停止位
    usartInitStruct.USART_WordLength = USART_WordLength_8b;                     // 8位数据位
    USART_Init(USART2, &usartInitStruct);

    USART_Cmd(USART2, ENABLE); // 使能串口

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 使能接收中断

    NVIC_InitTypeDef nvicInitStruct;
    nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;         // 选择配置NVIC的 USART2 线
    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;           // 指定NVIC线路使能
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0; // 指定NVIC线路的抢占优先级为0
    nvicInitStruct.NVIC_IRQChannelSubPriority = 0;        // 指定NVIC线路的响应优先级为0

    NVIC_Init(&nvicInitStruct);
}

/**
 * @description: 串口1和串口2初始化
 * @return: {*}
 */
void Usart_Init()
{
    Usart1_Init(115200); // Debug串口
    Usart2_Init(115200); // ESP8266串口通信
    return;
}

/**
 * @description: 串口数据发送
 * @param {USART_TypeDef} *USARTx 串口组
 * @param {unsigned char} *str 要发送的数据
 * @param {unsigned short} len 数据长度
 * @return: {*}
 */
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
    unsigned short count = 0;
    for (; count < len; count++)
    {
        USART_SendData(USARTx, *str++); // 发送数据
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET); // 等待发送完成
    }
}

/**
 * @description: 格式化打印
 * @param {USART_TypeDef} *USARTx 串口组
 * @param {char} *fmt 不定长参
 * @return: {*}
 */
void UsartPrintf(USART_TypeDef *USARTx, char *fmt, ...)
{
    unsigned char UsartPrintfBuf[296];
    va_list ap;
    unsigned char *pStr = UsartPrintfBuf;
    va_start(ap, fmt);
    vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap); // 格式化
    va_end(ap);
    while (*pStr != 0)
    {
        USART_SendData(USARTx, *pStr++);
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    }
}

/**
 * @description: 串口1收发中断
 * @return: {*}
 */
void USART1_IRQHandler(void)
{
    // 接收中断
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        USART_ClearFlag(USART1, USART_FLAG_RXNE);
    }
}
