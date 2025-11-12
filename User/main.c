#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "DHT11.h"
// #include "ESP8266.h"

uint8_t Temp = 0;	   // 当前温度
uint8_t Humi = 0;	   // 当前湿度
uint8_t TempThre = 30; // 温度阈值
uint8_t HumiThre = 70; // 湿度阈值
int16_t KeyNum = 0;	   // 按键键值

int main(void)
{
	OLED_Init();  // OLED初始化
	Key_Init();	  // Key初始化
	DHT11_Init(); // DHT11初始化
	// ESP8266_Init(); // ESP8266初始化

	while (1)
	{
		Screen_Show(); // 屏幕切换
	}
}
