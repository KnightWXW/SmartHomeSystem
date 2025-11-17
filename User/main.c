#include <stdio.h>
#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "DHT11.h"
#include "Buzzer.h"
#include "ESP8266.h"
#include "OneNet.h"

char Temp[16];		   // 存储当前温度(整数+小数)的字符串
char Humi[16];		   // 存储当前湿度(整数+小数)的字符串
uint8_t TempInt = 0;   // 当前温度(整数部分)
uint8_t TempDec = 0;   // 当前温度(小数部分)
uint8_t HumiInt = 0;   // 当前湿度(整数部分)
uint8_t HumiDec = 0;   // 当前湿度(小数部分)
uint8_t TempThre = 30; // 温度阈值
uint8_t HumiThre = 70; // 湿度阈值
int16_t KeyNum = 0;	   // 按键键值
char PUBLIS_BUF[256];
const char devPubTopic[] = "$sys/yHtNI9iijM/Test1/thing/property/post";
const char *devSubTopic[] = {"$sys/yHtNI9iijM/Test1/thing/property/set"};
unsigned char *dataPtr = NULL;
uint16_t TimeCount = 0;
uint8_t AlarmFlag = 0;

int main(void)
{
	OLED_Init();	// OLED初始化
	LED_Init();		// LED初始化
	Key_Init();		// Key初始化
	Buzzer_Init();	// 蜂鸣器初始化
	DHT11_Init();	// DHT11初始化
	
	OLED_ShowString(1, 1, "Connecting");
	
	ESP8266_Init(); // ESP8266初始化
	OLED_Clear();
	OLED_ShowString(1, 1, "11");
	// 连接OneNet平台,若失败等待500ms继续尝试
	while (OneNet_DevLink())
	{
		Delay_ms(500);
	}
	OLED_Clear();

	OLED_ShowString(1, 1, "Connect Success");
	Delay_ms(3000);
	OLED_Clear();

	while (1)
	{
		Screen_Show();					   // 屏幕切换
		Screen_Buzzer(GPIOC, GPIO_Pin_13); // 告警
	}
}
