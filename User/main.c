#include <stdio.h>
#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "Usart.h"
#include "DHT11.h"
#include "Buzzer.h"
#include "ESP8266.h"
#include "OneNet.h"
#include "Data.h"

char Temp[16];															  // 存储当前温度(整数+小数)的字符串
char Humi[16];															  // 存储当前湿度(整数+小数)的字符串
uint8_t TempInt = 0;													  // 当前温度(整数部分)
uint8_t TempDec = 0;													  // 当前温度(小数部分)
uint8_t HumiInt = 0;													  // 当前湿度(整数部分)
uint8_t HumiDec = 0;													  // 当前湿度(小数部分)
uint8_t TempThre = 30;													  // 温度阈值
uint8_t HumiThre = 70;													  // 湿度阈值
int16_t KeyNum = 0;														  // 按键键值
char PUBLIS_BUF[256];													  // 发布信息
const char DevPubTopic[] = "$sys/Yc4Qwil73t/Test1/thing/property/post";	  // 发布主题
const char *DevSubTopic[] = {"$sys/Yc4Qwil73t/Test1/thing/property/set"}; // 订阅主题
unsigned char *DataPtr = NULL;											  // 平台返回数据
uint16_t TimeCount = 0;													  // 计时数
uint8_t AlarmFlag = 0;													  // 报警状态

/**
 * @description: 存储将上传的发布数据
 * @return: {*}
 */
void Data_JsonValue()
{
	uint8_t Temp = TempInt;
	uint8_t Hum = HumiInt;
	memset(PUBLIS_BUF, 0, sizeof(PUBLIS_BUF));
	sprintf(PUBLIS_BUF, "{\"id\":\"123\",\"params\":{\"Temp\":{\"value\":%d},\"Hum\":{\"value\":%d} }}",
			TempInt, HumiInt);
	return;
}

int main(void)
{
	OLED_Init();   // OLED初始化
	LED_Init();	   // LED初始化
	Key_Init();	   // Key初始化
	Buzzer_Init(); // 蜂鸣器初始化
	Usart_Init();  // Usart初始化
	DHT11_Init();  // DHT11初始化

	OLED_ShowString(1, 1, "Connecting");

	ESP8266_Init(); // ESP8266初始化
	OLED_Clear();
	// 连接OneNet平台,若失败等待500ms继续尝试
	while (OneNet_DevLink())
	{
		Delay_ms(500);
	}
	OLED_Clear();

	OLED_ShowString(1, 1, "Connect Success");
	Delay_ms(3000);
	OLED_Clear();
	OneNet_Subscribe(DevSubTopic,1); // OneNet订阅主题
	while (1)
	{
		Screen_Show();	 // 屏幕切换
		Screen_Buzzer(); // 告警
		// 大约每5s进入一次
		TimeCount++;
		if (TimeCount >= 100)
		{
			Data_JsonValue();						 // 上传至全局变量
			OneNet_Publish(DevPubTopic, PUBLIS_BUF); // 全局变量上传至OneNet
			ESP8266_Clear();
			TimeCount = 0;
		}
		DataPtr = ESP8266_GetIPD(2); // 每20m获取平台返回的数据
		if (DataPtr != NULL)
		{
			OneNet_RevPro(DataPtr); // 进行数据监测
		}
	}
}
