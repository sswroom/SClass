#include "Stdafx.h"
#include "Core/Core.h"
#include "Net/MQTTConn.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebSocketClient.h"
#include "Sync/Thread.h"

#include <stdio.h>

Bool disconnected;
void __stdcall OnDisconnect(void *userObj)
{
	disconnected = true;
	printf("Disconnected\r\n");
}

void __stdcall OnMessage(void *userObj, Text::CString topic, const UInt8 *buff, UOSInt buffSize)
{
	printf("Data from topic: %s\r\n", topic.v);
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Net::OSSocketFactory sockf(true);
	Net::WebSocketClient *cli;
//	NEW_CLASS(cli, Net::WebSocketClient(&sockf, 0, CSTR("192.168.1.24"), 9001, CSTR("/mqtt"), CSTR_NULL, Net::WebSocketClient::Protocol::MQTT));
//	NEW_CLASS(cli, Net::WebSocketClient(&sockf, 0, CSTR("ws.vi-server.org"), 80, CSTR("/mirror"), CSTR("http://127.0.0.1"), Net::WebSocketClient::Protocol::Chat));
	NEW_CLASS(cli, Net::WebSocketClient(&sockf, 0, CSTR("test.mosquitto.org"), 8080, CSTR("/mqtt"), CSTR_NULL, Net::WebSocketClient::Protocol::MQTT));
	disconnected = false;
	if (!cli->IsDown())
	{
		Net::MQTTConn mqtt(cli, OnDisconnect, 0);
		if (!mqtt.IsError())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("sswrMQTT/"));
			sb.AppendI64(Data::DateTimeUtil::GetCurrTimeMillis());
			Bool succ = mqtt.SendConnect(4, 30, sb.ToCString(), CSTR_NULL, CSTR_NULL);
			if (succ && mqtt.WaitConnAck(30000) == Net::MQTTConn::CS_ACCEPTED)
			{
				mqtt.HandlePublishMessage(OnMessage, 0);
				mqtt.SendSubscribe(1, CSTR("#"));
				while (!disconnected)
				{
					Sync::Thread::Sleep(10);
				}
			}
			else
			{
				printf("Error in MQTT Connect\r\n");
			}
		}
	}
	else
	{
		DEL_CLASS(cli);
	}
	return 0;
}
