#include "Stdafx.h"
#include "Core/Core.h"
#include "Net/MQTTConn.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebSocketClient.h"
#include "Sync/SimpleThread.h"

#include <stdio.h>

Bool disconnected;
void __stdcall OnDisconnect(AnyType userObj)
{
	disconnected = true;
	printf("Disconnected\r\n");
}

void __stdcall OnMessage(AnyType userObj, Text::CStringNN topic, const Data::ByteArrayR &buff)
{
	printf("Data from topic: %s\r\n", topic.v.Ptr());
}

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	Net::OSSocketFactory sockf(true);
	Net::TCPClientFactory clif(sockf);
	NN<Net::WebSocketClient> cli;
//	NEW_CLASSNN(cli, Net::WebSocketClient(&sockf, 0, CSTR("192.168.1.24"), 9001, CSTR("/mqtt"), CSTR_NULL, Net::WebSocketClient::Protocol::MQTT, 30000));
//	NEW_CLASSNN(cli, Net::WebSocketClient(&sockf, 0, CSTR("ws.vi-server.org"), 80, CSTR("/mirror"), CSTR("http://127.0.0.1"), Net::WebSocketClient::Protocol::Chat, 30000));
//	NEW_CLASSNN(cli, Net::WebSocketClient(&sockf, 0, CSTR("test.mosquitto.org"), 8080, CSTR("/mqtt"), CSTR_NULL, Net::WebSocketClient::Protocol::MQTT, 30000));
	NEW_CLASSNN(cli, Net::WebSocketClient(clif, 0, CSTR("127.0.0.1"), 9001, CSTR("/mqtt"), CSTR_NULL, Net::WebSocketClient::Protocol::MQTT, 30000));
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
					Sync::SimpleThread::Sleep(10);
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
		cli.Delete();
	}
	return 0;
}
