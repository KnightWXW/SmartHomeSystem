#include "stm32f10x.h"
#include "ESP8266.h"
#include "delay.h"
#include "Usart.h"
#include <string.h>
#include <stdio.h>

#define ESP8266_WIFI_INFO "AT+CWJAP=\"OPPO\",\"147258369\"\r\n"
#define ESP8266_ONENET_INFO	 "AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"

unsigned char esp8266Buf[512];	  // ESP8266缓存数组
unsigned short esp8266Cnt = 0;	  // ESP8266当前计数
unsigned short esp8266CntPre = 0; // ESP8266之前计数

/**
 * @description: ESP8266清空缓存
 * @return: {*}
 */
void ESP8266_Clear(void)
{
	memset(esp8266Buf, 0, sizeof(esp8266Buf));
	esp8266Cnt = 0;
}

/**
 * @description: ESP8266等待接收完成
 * @return: {*}
 * 	REV_OK:	   接收完成
 * 	REV_WAIT:  接收超时未完成
 */
_Bool ESP8266_WaitRecive(void)
{
	// 如果接收计数为0,则说明未处于接收数据中,直接跳出返回
	if (esp8266Cnt == 0)
	{
		return REV_WAIT;
	}
	// 如果上一次的值和这次相同，则说明接收完毕
	if (esp8266Cnt == esp8266CntPre)
	{
		esp8266Cnt = 0; // 清0接收计数
		return REV_OK;	// 返回接收完成标志
	}
	esp8266CntPre = esp8266Cnt; // 置为相同
	return REV_WAIT;			// 返回接收未完成标志
}

/**
 * @description: ESP8266发送命令
 * @param {char} *cmd 命令
 * @param {char} *res 需要检查的返回指令
 * @return: {*} 0:成功	1:失败
 */
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	unsigned char timeOut = 200;
	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	while (timeOut--)
	{
		if (ESP8266_WaitRecive() == REV_OK) // 如果收到数据
		{
			if (strstr((const char *)esp8266Buf, res) != NULL) // 如果检索到关键词
			{
				ESP8266_Clear(); // 清空缓存
				return 0;
			}
		}
		Delay_ms(10);
	}
	return 1;
}

/**
 * @description: ESP8266发送数据
 * @param {unsigned char} *data 数据内容
 * @param {unsigned short} len  数据长度
 * @return: {*}
 */
void ESP8266_SendData(unsigned char *data, unsigned short len)
{
	char cmdBuf[32];
	ESP8266_Clear();						   // 清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len); // 发送命令
	if (!ESP8266_SendCmd(cmdBuf, ">"))		   // 收到‘>’时可以发送数据
	{
		Usart_SendString(USART2, data, len); // 发送设备连接请求数据
	}
}

/**
 * @description: ESP8266获取平台返回的数据
 * @param {unsigned short} timeOut 等待的时间(乘10ms)
 * @return: {*} 平台返回的原始数据
 * 		不同网络设备返回的格式不同,需要去调试
 * 		如ESP8266的返回格式为"+IPD,x:yyy"
 * 			x代表数据长度，yyy是数据内容
 */
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{
	char *ptrIPD = NULL;
	do
	{
		if (ESP8266_WaitRecive() == REV_OK) // 如果接收完成
		{
			ptrIPD = strstr((char *)esp8266Buf, "IPD,"); // 搜索“IPD”头
			if (ptrIPD == NULL)							 // 如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				// UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':'); // 找到':'
				if (ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
				{
					return NULL;
				}
			}
		}
		Delay_ms(5); // 延时等待
	} while (timeOut--);
	return NULL; // 超时还未找到，返回空指针
}

/**
 * @description: ESP8266初始化
 * @return: {*}
 */
void ESP8266_Init(void)
{
	ESP8266_Clear();
	UsartPrintf(USART_DEBUG, "1. AT\r\n");
	while (ESP8266_SendCmd("AT\r\n", "OK"))
	{
	};
	Delay_ms(500);
	UsartPrintf(USART_DEBUG, "2. CWMODE\r\n");
	while (ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
	{
	};
	Delay_ms(500);
	UsartPrintf(USART_DEBUG, "3. AT+CWDHCP\r\n");
	while (ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
	{
		
	};
	Delay_ms(500);
	UsartPrintf(USART_DEBUG, "4. CWJAP\r\n");
	while (ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
	{
		
	};
	Delay_ms(500);
	UsartPrintf(USART_DEBUG, "5. CIPSTART\r\n");
	while (ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
	{
		
	};
	Delay_ms(500);
	UsartPrintf(USART_DEBUG, "6. ESP8266 Init OK\r\n");
}

/**
 * @description: USART2收发中断函数
 * @return: {*}
 */
void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) // 接收中断
	{
		if (esp8266Cnt >= sizeof(esp8266Buf))
		{
			esp8266Cnt = 0; // 防止串口被刷爆
		}
		esp8266Buf[esp8266Cnt++] = USART2->DR;
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
}
