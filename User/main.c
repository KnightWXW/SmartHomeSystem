#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "DHT11.h"
#include "ESP8266.h"

int main(void)
{
	OLED_Init();		  // OLED初始化
	Key_Init(GPIO_Pin_3); // Key初始化
	DHT11_Init();		  // DHT11初始化
	ESP8266_Init();		  // ESP8266初始化

	while (1)
	{
	}
}
