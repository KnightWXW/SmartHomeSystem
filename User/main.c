#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Key.h"
#include "DHT11.h"

int main(void)
{
	OLED_Init();		  // OLED初始化
	Key_Init(GPIO_Pin_0); // Key初始化
	DHT11_Init();		  // DHT11初始化

	while (1)
	{
		OLED_ShowString(1, 0, "DHT11 init OK   ");
		Delay_ms(100);
	}
	OLED_ShowString(3, 0, "Hardware init OK    ");
}
