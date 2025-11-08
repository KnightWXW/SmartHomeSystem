#include "DHT11.h"
#include "delay.h"

#define DHT_PIN GPIO_Pin_1

/**
 * @description: 复位DHT11
 * @return: {*}
 */
void DHT11_Reset(void)
{
    DHT11_IO_OUT();  // SET OUTPUT
    DHT11_DQ_OUT(0); // 拉低DQ
    Delay_ms(20);    // 拉低至少18ms
    DHT11_DQ_OUT(1); // DQ=1
    Delay_us(30);     // 主机拉高20~40us
}

/**
 * @description: 等待DHT11的回应
 * @return: {*}
 *      1: 未检测到DHT11存在
 *      0: 检测到DHT11存在
 */
uint8_t DHT11_Check(void)
{
    uint8_t retry = 0;
    DHT11_IO_IN();                     // SET INPUT
    while (DHT11_DQ_IN && retry < 100) // DHT11会拉低40~80us
    {
        retry++;
        Delay_us(1);
    };
    if (retry >= 100)
    {
        return 1;
    }
    else
    {
        retry = 0;
    }
    while (!DHT11_DQ_IN && retry < 100) // DHT11拉低后会再次拉高40~80us
    {
        retry++;
        Delay_us(1);
    };
    if (retry >= 100)
    {
        return 1;
    }
    return 0;
}

/**
 * @description: 从DHT11读取一个数据位
 * @return: {*}
 */
uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry = 0;
    while (DHT11_DQ_IN && retry < 100) // 等待变为低电平
    {
        retry++;
        Delay_us(1);
    }
    retry = 0;
    while (!DHT11_DQ_IN && retry < 100) // 等待变高电平
    {
        retry++;
        Delay_us(1);
    }
    Delay_us(40); // 等待40us
    if (DHT11_DQ_IN)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @description: 从DHT11读取一个字节
 * @return: {*}
 */
uint8_t DHT11_Read_Byte(void)
{
    uint8_t readByte = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        readByte <<= 1;
        readByte |= DHT11_Read_Bit();
    }
    return readByte;
}

/**
 * @description: 从DHT11读取一次数据
 * @param {uint8_t} *temp: 温度值(范围:0~50°)
 * @param {uint8_t} *humi: 湿度值(范围:20%~90%)
 * @return: {*} 0[正常]/1[读取失败]
 */
uint8_t DHT11_Read_Data(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;
    DHT11_Reset();
    if (DHT11_Check() == 0)
    {
        for (i = 0; i < 5; i++) // 读取40位数据
        {
            buf[i] = DHT11_Read_Byte();
        }
        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi = buf[0];
            *temp = buf[2];
        }
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * @description: DHT11初始化
 *      初始化DHT11的IO口 & 检测DHT11的存在
 * @return: {*} 0[存在]/1[不存在]
 */
uint8_t DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能PG端口时钟
    GPIO_InitStructure.GPIO_Pin = DHT_PIN;                // PG11端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化IO口
    GPIO_SetBits(GPIOA, DHT_PIN);          // PG11 输出高

    DHT11_Reset();          // 复位DHT11
    return DHT11_Check(); // 等待DHT11的回应
}
