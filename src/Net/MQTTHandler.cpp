#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/MQTTHandler.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

UInt32 __stdcall Net::MQTTHandler::KAThread(void *userObj)
{
	Net::MQTTHandler *me = (Net::MQTTHandler*)userObj;
	me->kaRunning = true;
	while (!me->kaToStop)
	{
		me->client->SendPing();
		me->kaEvt->Wait(me->kaSeconds * 500);
	}
	me->kaRunning = false;
	return 0;
}

Net::MQTTHandler::MQTTHandler(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UTF8Char *username, const UTF8Char *password, Net::MQTTClient::PublishMessageHdlr hdlr, void *userObj, UInt32 kaSeconds)
{
	NEW_CLASS(this->client, Net::MQTTClient(sockf, addr, port));
	if (this->client->IsError())
	{
		DEL_CLASS(this->client);
		this->client = 0;
		return;
	}
	this->client->HandlePublishMessage(hdlr, userObj);
	this->kaRunning = false;
	this->kaToStop = false;
	this->kaSeconds = kaSeconds;
	NEW_CLASS(this->kaEvt, Sync::Event(true, (const UTF8Char*)"Net.MQTThandler.kaEvt"));

	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"sswrMQTT/");
	sb.AppendI64(dt.ToTicks());
	Bool succ = this->client->SendConnect(4, kaSeconds, sb.ToString(), username, password);
	if (succ)
	{
		Net::MQTTClient::ConnectStatus status = this->client->WaitConnAck(30000);
		succ = (status == Net::MQTTClient::CS_ACCEPTED);
	}
	if (!succ)
	{
		DEL_CLASS(this->client);
		this->client = 0;
	}
	else
	{
		Sync::Thread::Create(KAThread, this);
		while (!this->kaRunning)
		{
			Sync::Thread::Sleep(10);
		}
	}
}

Net::MQTTHandler::~MQTTHandler()
{
	if (this->kaRunning)
	{
		this->kaToStop = true;
		this->kaEvt->Set();
		while (this->kaRunning)
		{
			Sync::Thread::Sleep(10);
		}
	}
	SDEL_CLASS(this->client);
	DEL_CLASS(this->kaEvt);
}

Bool Net::MQTTHandler::IsError()
{
	return this->client == 0;
}

Bool Net::MQTTHandler::Subscribe(const UTF8Char *topic)
{
	if (this->client == 0) return false;
	if (this->client->SendSubscribe(1, topic))
	{
		if (this->client->WaitSubAck(1, 30000) <= 2)
		{
			return true;
		}
	}
	return false;
}

Bool Net::MQTTHandler::Publish(const UTF8Char *topic, const UTF8Char *message)
{
	if (this->client == 0) return false;
	return this->client->SendPublish(topic, message);
}
