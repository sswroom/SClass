#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/MQTTStaticClient.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

UInt32 __stdcall Net::MQTTStaticClient::KAThread(void *userObj)
{
	Net::MQTTStaticClient *me = (Net::MQTTStaticClient*)userObj;
	me->kaRunning = true;
	while (!me->kaToStop)
	{
		Sync::MutexUsage mutUsage(me->connMut);
		if (me->conn)
		{
			me->conn->SendPing();
			mutUsage.EndUse();
		}
		else
		{
			mutUsage.EndUse();
			me->Connect();
		}
		me->kaEvt->Wait(me->kaSeconds * 500);
	}
	me->kaRunning = false;
	return 0;
}

void __stdcall Net::MQTTStaticClient::OnDisconnect(void *userObj)
{
	Net::MQTTStaticClient *me = (Net::MQTTStaticClient*)userObj;
	Sync::MutexUsage mutUsage(me->connMut);
	if (me->conn)
	{
		DEL_CLASS(me->conn);
		me->conn = 0;
		//me->kaEvt->Set();
	}
}

void Net::MQTTStaticClient::Connect()
{
	Net::MQTTConn *conn;
	NEW_CLASS(conn, Net::MQTTConn(this->sockf, this->ssl, this->host, this->port, OnDisconnect, this));
	if (conn->IsError())
	{
		DEL_CLASS(conn);
		return;
	}
	conn->HandlePublishMessage(this->hdlr, this->hdlrObj);

	Bool succ = conn->SendConnect(4, this->kaSeconds, this->clientId, this->username, this->password);
	if (succ)
	{
		Net::MQTTConn::ConnectStatus status = conn->WaitConnAck(30000);
		succ = (status == Net::MQTTConn::CS_ACCEPTED);
	}
	if (!succ)
	{
		DEL_CLASS(conn);
	}
	else
	{
		Sync::MutexUsage mutUsage(this->connMut);
		if (conn->IsError())
		{
			DEL_CLASS(conn);
		}
		else
		{
			this->conn = conn;
		}
	}
}

Net::MQTTStaticClient::MQTTStaticClient(Net::MQTTConn::PublishMessageHdlr hdlr, void *userObj)
{
	this->kaRunning = false;
	this->kaToStop = false;
	this->kaSeconds = 30;
	NEW_CLASS(this->kaEvt, Sync::Event(true, (const UTF8Char*)"Net.MQTThandler.kaEvt"));
	NEW_CLASS(this->connMut, Sync::Mutex());
	this->conn = 0;

	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"sswrMQTT/");
	sb.AppendI64(dt.ToTicks());
	this->clientId = Text::StrCopyNew(sb.ToString());
	this->hdlr = hdlr;
	this->hdlrObj = userObj;

	this->sockf = 0;
	this->ssl = 0;
	this->host = 0;
	this->port = 0;
	this->username = 0;
	this->password = 0;
}

Net::MQTTStaticClient::MQTTStaticClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *host, UInt16 port, const UTF8Char *username, const UTF8Char *password, Net::MQTTConn::PublishMessageHdlr hdlr, void *userObj, UInt16 kaSeconds) : Net::MQTTStaticClient(hdlr, userObj)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->host = Text::StrCopyNew(host);
	this->port = port;
	this->username = SCOPY_TEXT(username);
	this->password = SCOPY_TEXT(password);
	this->kaSeconds = kaSeconds;

	this->Connect();

	Sync::Thread::Create(KAThread, this);
	while (!this->kaRunning)
	{
		Sync::Thread::Sleep(10);
	}
}

Net::MQTTStaticClient::~MQTTStaticClient()
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
	SDEL_CLASS(this->conn);
	DEL_CLASS(this->connMut);
	DEL_CLASS(this->kaEvt);
	SDEL_TEXT(this->username);
	SDEL_TEXT(this->password);
	Text::StrDelNew(this->host);
	Text::StrDelNew(this->clientId);
}

Bool Net::MQTTStaticClient::IsStarted()
{
	return this->kaRunning;
}

Bool Net::MQTTStaticClient::ChannelFailure()
{
	return this->conn == 0;
}

Bool Net::MQTTStaticClient::Subscribe(const UTF8Char *topic)
{
	Sync::MutexUsage mutUsage(this->connMut);
	if (this->conn == 0) return false;
	if (this->conn->SendSubscribe(1, topic))
	{
		if (this->conn->WaitSubAck(1, 30000) <= 2)
		{
			return true;
		}
	}
	return false;
}

Bool Net::MQTTStaticClient::Publish(const UTF8Char *topic, const UTF8Char *message)
{
	Sync::MutexUsage mutUsage(this->connMut);
	if (this->conn == 0) return false;
	return this->conn->SendPublish(topic, message);
}
