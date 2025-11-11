#include "stm32f10x.h" // Device header
#include "OLED.h"

void OLED_ShowScreenOfMain(void)
{
}

void OLED_ShowScreenOfSetTemp(void)
{
}

void OLED_ShowScreenOfSetHum(void)
{
}

void OLED_ShowScreen(uint8_t num)
{
    switch (num)
    {
    case SCREEN_MAIN:
        OLED_Clear();
        OLED_ShowScreenOfMain();
        break;
    case SCREEN_SET_TEMP:
        OLED_Clear();
        OLED_ShowScreenOfSetTemp(void);
        break;
    case SCREEN_SET_HUM:
        OLED_Clear();
        OLED_ShowScreenOfSetHum(void);
        break;
    default:
        OLED_Clear();
        OLED_ShowString(1, 1, "OLED SCREEN SHOW ERROR!");
        break;
    }
}