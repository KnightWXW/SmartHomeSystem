#include "Usart.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

/**
 * @description: 串口1初始化
 * @param {unsigned int} baud 设定的波特率
 * @return: {*}
 */
void Usart1_Init(unsigned int baud)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    // PA9	TXD
    GPIO_InitTypeDef gpioInitStruct;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);

    // PA10	RXD
    USART_InitTypeDef usartInitStruct;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_10;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);

    NVIC_InitTypeDef nvicInitStruct;
    usartInitStruct.USART_BaudRate = baud;
    usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 接收和发送
    usartInitStruct.USART_Parity = USART_Parity_No;                             // 无校验
    usartInitStruct.USART_StopBits = USART_StopBits_1;                          // 1位停止位
    usartInitStruct.USART_WordLength = USART_WordLength_8b;                     // 8位数据位
    USART_Init(USART1, &usartInitStruct);

    USART_Cmd(USART1, ENABLE); // 使能串口

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 使能接收中断

    nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;
    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInitStruct.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&nvicInitStruct);
}

/**
 * @description: 串口2初始化
 * @param {unsigned int} baud 设定的波特率
 * @return: {*}
 */
void Usart2_Init(unsigned int baud)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // PA2	TXD
    GPIO_InitTypeDef gpioInitStruct;
    gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);

    // PA3	RXD
    gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
    gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInitStruct);

    USART_InitTypeDef usartInitStruct;
    usartInitStruct.USART_BaudRate = baud;
    usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 接收和发送
    usartInitStruct.USART_Parity = USART_Parity_No;                             // 无校验
    usartInitStruct.USART_StopBits = USART_StopBits_1;                          // 1位停止位
    usartInitStruct.USART_WordLength = USART_WordLength_8b;                     // 8位数据位
    USART_Init(USART2, &usartInitStruct);

    USART_Cmd(USART2, ENABLE); // 使能串口

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 使能接收中断

    NVIC_InitTypeDef nvicInitStruct;
    nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
    nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
    nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvicInitStruct);
}

/**
 * @description: 串口1和串口2初始化
 * @return: {*}
 */
void Usart_Init()
{
    Usart1_Init(115200); // debug串口
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
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
            ; // 等待发送完成
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
        while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET)
            ;
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
