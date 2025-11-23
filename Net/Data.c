#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

extern char Temp[16];	 // 存储当前温度(整数+小数)的字符串
extern char Humi[16];	 // 存储当前湿度(整数+小数)的字符串
extern uint8_t TempInt;	 // 当前温度(整数部分)
extern uint8_t HumiInt;	 // 当前湿度(整数部分)
extern uint8_t TempThre; // 温度阈值
extern uint8_t HumiThre; // 湿度阈值
extern int16_t KeyNum;	 // 按键键值

char PUBLIS_BUF[256];
const char devPubTopic[] = "$sys/Yc4Qwil73t/dev1/thing/property/post";
const char *DevSubTopic[] = {"$sys/Yc4Qwil73t/dev1/thing/property/set"};

void Data_JsonValue()
{
	uint8_t Temp = TempInt;
	uint8_t Hum = HumiInt;
	memset(PUBLIS_BUF, 0, sizeof(PUBLIS_BUF));
	sprintf(PUBLIS_BUF, "{\"id\":\"123\",\"params\":{\"Temp\":{\"value\":%d},\"Hum\":{\"value\":%d} }}",
			TempInt, HumiInt);
	return;
}