#include "DHT11.h"
#include "delay.h"

#define DHT_PIN GPIO_Pin_1

/**
 * @description: DHT11输出引脚初始化
 * @return: {*}
 */
void DHT11_IO_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT_PIN;            // GPIO引脚:DHT_PIN 引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // GPIO模式:推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // GPIO速度:50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
 * @description: DHT11输入引脚初始化
 * @return: {*}
 */
void DHT11_IO_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DHT_PIN;            // GPIO引脚:DHT_PIN 引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;     // GPIO模式:上拉输入模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // GPIO速度:50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
 * @description: 复位DHT11,并发送初始信号[输出模式]
 * @return: {*}
 */
void DHT11_Reset(void)
{
    DHT11_IO_OUT();                                 // 输出引脚初始化
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, (BitAction)0); // 拉低引脚20ms(至少18ms)
    Delay_ms(20);
    GPIO_WriteBit(GPIOA, GPIO_Pin_1, (BitAction)1); // 松开引脚30μs(20-40μs)
    Delay_us(30);
}

/**
 * @description: 等待DHT11的回应[输入模式]
 * @return: {*}
 *      1: 未检测到DHT11存在
 *      0: 检测到DHT11存在
 */
uint8_t DHT11_Check(void)
{
    uint8_t retry = 0;
    DHT11_IO_IN(); // 输入引脚初始化
    // 拉低40~80us
    while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) && retry < 100)
    {
        retry++;
        Delay_us(1);
    }
    if (retry >= 100)
    {
        return 1;
    }
    else
    {
        retry = 0;
    }
    // 拉低后会再次拉高40~80us
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) && retry < 100)
    {
        retry++;
        Delay_us(1);
    }
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
    // 等待变为低电平
    while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) && retry < 100)
    {
        retry++;
        Delay_us(1);
    }
    retry = 0;
    // 等待变为高电平
    while (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) && retry < 100)
    {
        retry++;
        Delay_us(1);
    }
    // 读取1位数据
    Delay_us(40);
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1))
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
uint8_t DHT11_Read_Data(uint8_t *TempInt, uint8_t *TempDec, uint8_t *HumiInt, uint8_t *HumiDec)
{
    uint8_t buf[5];
    DHT11_Reset(); // DHT复位,发出起始信号
    if (DHT11_Check() == 0)
    {
        for (uint8_t i = 0; i < 5; i++) // 读取40位数据
        {
            buf[i] = DHT11_Read_Byte();
        }
        // 校验和 == [8bit湿度整数数据 + 8bit湿度小数数据
        //           + 8bi温度整数数据 + 8bit温度小数数据] 所得结果的末尾8位
        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *HumiInt = buf[0];
            *HumiDec = buf[1];
            *TempInt = buf[2];
            *TempDec = buf[3];
        }
    }
    else
    {
        return 1;
    }
    return 0;
}

/**
 * @description: DHT11初始化
 *      初始化DHT11的IO口 & 检测DHT11的存在
 * @return: {*} 0[存在]/1[不存在]
 */
uint8_t DHT11_Init(void)
{
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // GPIOA
    DHT11_IO_OUT();
    GPIO_SetBits(GPIOA, DHT_PIN); // 引脚初始化输出高电平

    DHT11_Reset();        // 复位DHT11,发送起始信号
    return DHT11_Check(); // 等待DHT11的回应
}
