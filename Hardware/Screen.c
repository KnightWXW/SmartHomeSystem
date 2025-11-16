#include <stdio.h>
#include "stm32f10x.h" // Device header
#include "Key.h"
#include "OLED.h"
#include "DHT11.h"
#include "Buzzer.h"

extern char Temp[16];    // 存储当前温度(整数+小数)的字符串
extern char Humi[16];    // 存储当前湿度(整数+小数)的字符串
extern uint8_t TempInt;  // 当前温度(整数部分)
extern uint8_t TempDec;  // 当前温度(小数部分)
extern uint8_t HumiInt;  // 当前湿度(整数部分)
extern uint8_t HumiDec;  // 当前湿度(小数部分)
extern uint8_t TempThre; // 温度阈值
extern uint8_t HumiThre; // 湿度阈值
extern int16_t KeyNum;   // 按键键值

typedef enum
{
    SCREEN_MAIN,
    SCREEN_SET_TEMP,
    SCREEN_SET_HUM,
    SCREEN_TOTAL_NUM,
} Screen_Enum;

Screen_Enum CurCNT = SCREEN_MAIN; // 屏幕计数值

/**
 * @description: 显示主菜单屏幕
 * @return: {*}
 */
void Screen_ShowMain(void)
{
    OLED_ShowString(1, 1, "System Menu");
    OLED_ShowString(2, 1, "Temp:");
    OLED_ShowString(3, 1, "Humi:");

    if (DHT11_Read_Data(&TempInt, &TempDec, &HumiInt, &HumiDec) == 0)
    {
        sprintf(Temp, "%d.%d", TempInt, TempDec);
        sprintf(Humi, "%d.%d", HumiInt, HumiDec);
        OLED_ShowString(2, 6, Temp);
        OLED_ShowString(3, 6, Humi);
    }
    return;
}

/**
 * @description: 显示设置温度值的屏幕
 * @return: {*}
 */
void Screen_ShowSetTemp(void)
{
    OLED_ShowString(1, 1, "Set Temp Value");
    OLED_ShowString(2, 1, "Thre:");
    if (KeyNum == 2)
    {
        TempThre--;
    }
    else if (KeyNum == 3)
    {
        TempThre++;
    }
    OLED_ShowNum(2, 6, TempThre, 3);
    return;
}

/**
 * @description: 显示设置湿度值的屏幕
 * @return: {*}
 */
void Screen_ShowSetHumi(void)
{
    OLED_ShowString(1, 1, "Set Humi Value");
    OLED_ShowString(2, 1, "Thre:");
    if (KeyNum == 2)
    {
        HumiThre--;
    }
    else if (KeyNum == 3)
    {
        HumiThre++;
    }
    OLED_ShowNum(2, 6, HumiThre, 3);
    return;
}

/**
 * @description: 屏幕切换
 * @return: {*}
 */
void Screen_Show(void)
{
    KeyNum = Key_GetNum(0);
    if (KeyNum == 1)
    {
        CurCNT++;
        CurCNT = (Screen_Enum)(CurCNT % SCREEN_TOTAL_NUM);
        OLED_Clear();
    }

    switch (CurCNT)
    {
    case SCREEN_MAIN:
        Screen_ShowMain();
        break;
    case SCREEN_SET_TEMP:
        Screen_ShowSetTemp();
        break;
    case SCREEN_SET_HUM:
        Screen_ShowSetHumi();
        break;
    default:
        OLED_ShowString(1, 1, "SHOW ERROR!");
        break;
    }
    return;
}

/**
 * @description: 蜂鸣器警报
 * @return: {*}
 */
void Screen_Buzzer()
{
    if ((TempInt > TempThre) || (HumiInt > HumiThre))
    {
        Buzzer_ON();
    }
    else
    {
        Buzzer_OFF();
    }
    return;
}
