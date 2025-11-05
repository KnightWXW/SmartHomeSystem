#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Key.h"

int main(void)
{
	OLED_Init();		  // OLED初始化
	Key_Init(GPIO_Pin_0); // Key初始化

	while (1)
	{
	}
}