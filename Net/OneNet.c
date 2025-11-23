#include "stm32f10x.h"
#include "ESP8266.h"
#include "OneNet.h"
#include "Mqttkit.h"
#include "usart.h"
#include "delay.h"
#include "LED.h"
#include <string.h>
#include <stdio.h>
#include "CJSON.h"
#include "Data.h"

/*产品ID*/
#define PROID "Yc4Qwil73t"

// 鉴权Token
#define TOKEN "version=2018-10-31&res=products%2FYc4Qwil73t%2Fdevices%2Fdev1&et=2082712516&method=md5&sign=iy8PjKSgoSLN%2Bhu65d0lwg%3D%3D"

// 设备名称
#define DEVID "dev1"

extern unsigned char esp8266_buf[512];
extern uint8_t AlarmFlag; // 报警状态
extern uint8_t TempInt;	  // 当前温度(整数部分)
extern uint8_t HumiInt;	  // 当前湿度(整数部分)

/**
 * @description: 与onenet创建连接
 * @return: {*}  1-成功	 0-失败
 */
_Bool OneNet_DevLink(void)
{
	// 协议包
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	unsigned char *dataPtr;
	int i = 0;
	_Bool status = 1;
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
							 "PROID: %s,	TOKEN: %s, DEVID:%s\r\n",
				PROID, TOKEN, DEVID);
	if (MQTT_PacketConnect(PROID, TOKEN, DEVID, 256, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len); // 上传平台
		dataPtr = ESP8266_GetIPD(250);						 // 等待平台响应
		if (dataPtr != NULL)
		{
			if (MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch (MQTT_UnPacketConnectAck(dataPtr))
				{
				case 0:
					UsartPrintf(USART_DEBUG, "Tips:CONN SUCCESS\r\n");
					status = 0;
					break;
				case 1:
					UsartPrintf(USART_DEBUG, "WARN:CONN ERROR-Protocol Error\r\n");
					break;
				case 2:
					UsartPrintf(USART_DEBUG, "WARN:CONN ERROR-Invalid Clientid\r\n");
					break;
				case 3:
					UsartPrintf(USART_DEBUG, "WARN:CONN ERROR-Server Failure\r\n");
					break;
				case 4:
					UsartPrintf(USART_DEBUG, "WARN:CONN ERROR-Incorrect Username Or Password\r\n");
					break;
				case 5:
					UsartPrintf(USART_DEBUG, "WARN:CONN ERROR-Invalid Link(Invalid Token)\r\n");
					break;
				default:
					UsartPrintf(USART_DEBUG, "ERROR:CONN ERROR-Unknown Error\r\n");
					break;
				}
			}
		}
		MQTT_DeleteBuffer(&mqttPacket); // 删包
	}
	else
	{
		UsartPrintf(USART_DEBUG, "WARN:MQTT_PacketConnect Failed\r\n");
	}
	return status;
}

/**
 * @description: 订阅
 * @param {char} *topics  订阅的topic
 * @param {unsigned char} topic_cnt  topic个数
 * @return: {*}  SEND_TYPE_OK-成功	SEND_TYPE_SUBSCRIBE-需要重发
 */
void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	// 协议包
	unsigned char i = 0;
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	for (; i < topic_cnt; i++)
	{
		UsartPrintf(USART_DEBUG, "Subscribe Topic: %s\r\n", topics[i]);
	}
	if (MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len); // 向平台发送订阅请求
		MQTT_DeleteBuffer(&mqttPacket);						 // 删包
	}
}

/**
 * @description: 发布消息
 * @param {char} *topic  发布的主题
 * @param {char} *msg 消息内容
 * @return: {*} SEND_TYPE_OK-成功	SEND_TYPE_PUBLISH-需要重送
 */
void OneNet_Publish(const char *topic, const char *msg)
{
	// 协议包
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};
	UsartPrintf(USART_DEBUG, "Publish Topic: %s, Msg: %s\r\n", topic, msg);
	if (MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len); // 向平台发送订阅请求
		MQTT_DeleteBuffer(&mqttPacket);						 // 删包
	}
}

/**
 * @description: 平台返回数据检测
 * @param {unsigned char} *cmd 平台返回的数据
 * @return: {*}
 */
void OneNet_RevPro(unsigned char *cmd)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // 协议包
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	short result = 0;
	cJSON *json, *params_json, *led_json, *Alarm_json, *Temp_json, *Humi_json;
	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	type = MQTT_UnPacketRecv(cmd);
	switch (type)
	{
	case MQTT_PKT_CMD:														  // 命令下发
		result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len); // 解出topic和消息体
		if (result == 0)
		{
			UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
			if (MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0) // 命令回复组包
			{
				UsartPrintf(USART_DEBUG, "Tips:	Send CmdResp\r\n");

				ESP8266_SendData(mqttPacket._data, mqttPacket._len); // 回复命令
				MQTT_DeleteBuffer(&mqttPacket);						 // 删包
			}
		}
		break;
	case MQTT_PKT_PUBLISH: // 接收的Publish消息
		result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
		if (result == 0)
		{
			UsartPrintf(USART_DEBUG, "topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
						cmdid_topic, topic_len, req_payload, req_len);
			// 对数据包req_payload进行JSON格式解析
			json = cJSON_Parse(req_payload);
			params_json = cJSON_GetObjectItem(json, "params");
			led_json = cJSON_GetObjectItem(params_json, "LED");
			Alarm_json = cJSON_GetObjectItem(params_json, "Alarm");
			Temp_json = cJSON_GetObjectItem(params_json, "temp");
			Humi_json = cJSON_GetObjectItem(params_json, "hum");
			if (led_json != NULL)
			{
				if (led_json->type == cJSON_True)
				{
					LED_ON();
				}
				else
				{
					LED_OFF();
				}
			}
			if (Alarm_json != NULL)
			{
				if (Alarm_json->type == cJSON_True)
				{
					AlarmFlag = 1;
					UsartPrintf(USART_DEBUG, "AlarmFlag = 1\r\n");
				}
				else
				{
					AlarmFlag = 0;
					UsartPrintf(USART_DEBUG, "AlarmFlag = 0\r\n");
				}
			}
			if (Temp_json != NULL)
			{
				TempInt = Temp_json->valueint;
			}
			if (Humi_json != NULL)
			{
				HumiInt = Humi_json->valueint;
			}
			cJSON_Delete(json);
		}
		break;
	case MQTT_PKT_PUBACK: // 发送Publish消息，平台回复的Ack
		if (MQTT_UnPacketPublishAck(cmd) == 0)
		{
			UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
		}
		break;
	case MQTT_PKT_PUBREC: // 发送Publish消息，平台回复的Rec，设备需回复Rel消息
		if (MQTT_UnPacketPublishRec(cmd) == 0)
		{
			UsartPrintf(USART_DEBUG, "Tips:	Rev PublishRec\r\n");
			if (MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
			{
				UsartPrintf(USART_DEBUG, "Tips:	Send PublishRel\r\n");
				ESP8266_SendData(mqttPacket._data, mqttPacket._len);
				MQTT_DeleteBuffer(&mqttPacket);
			}
		}
		break;
	case MQTT_PKT_PUBREL: // 收到Publish消息，设备回复Rec后，平台回复的Rel，设备需再回复Comp
		if (MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
		{
			UsartPrintf(USART_DEBUG, "Tips:	Rev PublishRel\r\n");
			if (MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
			{
				UsartPrintf(USART_DEBUG, "Tips:	Send PublishComp\r\n");
				ESP8266_SendData(mqttPacket._data, mqttPacket._len);
				MQTT_DeleteBuffer(&mqttPacket);
			}
		}
		break;
	case MQTT_PKT_PUBCOMP: // 发送Publish消息，平台返回Rec，设备回复Rel，平台再返回的Comp
		if (MQTT_UnPacketPublishComp(cmd) == 0)
		{
			UsartPrintf(USART_DEBUG, "Tips:	Rev PublishComp\r\n");
		}
		break;
	case MQTT_PKT_SUBACK: // 发送Subscribe消息的Ack
		if (MQTT_UnPacketSubscribe(cmd) == 0)
		{
			UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe OK\r\n");
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Tips:	MQTT Subscribe Err\r\n");
		}
		break;
	case MQTT_PKT_UNSUBACK: // 发送UnSubscribe消息的Ack
		if (MQTT_UnPacketUnSubscribe(cmd) == 0)
		{
			UsartPrintf(USART_DEBUG, "Tips:	MQTT UnSubscribe OK\r\n");
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Tips:	MQTT UnSubscribe Err\r\n");
		}
		break;
	default:
		result = -1;
		break;
	}
	ESP8266_Clear(); // 清空缓存
	if (result == -1)
	{
		return;
	}
	if (type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}
}
