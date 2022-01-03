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
		me->kaEvt->Wait((UOSInt)me->kaSeconds * 500);
	}
	me->kaRunning = false;
	return 0;
}

void __stdcall Net::MQTTStaticClient::OnDisconnect(void *userObj)
{
	Net::MQTTStaticClient *me = (Net::MQTTStaticClient*)userObj;
	if (!me->kaRunning)
	{
		return;
	}
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
	this->packetId = 1;
	Sync::MutexUsage mutUsage(this->hdlrMut);
	UOSInt i = 0;
	UOSInt j = this->hdlrList->GetCount();
	while (i < j)
	{
		conn->HandlePublishMessage(this->hdlrList->GetItem(i), this->hdlrObjList->GetItem(i));
		i++;
	}
	mutUsage.EndUse();

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
		mutUsage.ReplaceMutex(this->connMut);
		if (conn->IsError())
		{
			DEL_CLASS(conn);
			return;
		}
		else
		{
			this->conn = conn;
		}
		Data::ArrayList<const UTF8Char*> topicList;
		mutUsage.ReplaceMutex(this->topicMut);
		topicList.AddAll(this->topicList);
		mutUsage.EndUse();
		
		i = 0;
		j = topicList.GetCount();
		while (i < j)
		{
			mutUsage.ReplaceMutex(this->connMut);
			if (this->conn == 0)
			{
				mutUsage.EndUse();
				break;
			}
			UInt16 packetId = GetNextPacketId();
			if (this->conn->SendSubscribe(packetId, topicList.GetItem(i)))
			{
				this->conn->WaitSubAck(packetId, 30000);
			}
			mutUsage.EndUse();

			i++;
		}
	}
}

UInt16 Net::MQTTStaticClient::GetNextPacketId()
{
	Sync::MutexUsage mutUsage(this->packetIdMut);
	return this->packetId++;
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
	sb.AppendC(UTF8STRC("sswrMQTT/"));
	sb.AppendI64(dt.ToTicks());
	this->clientId = Text::StrCopyNew(sb.ToString());
	this->packetId = 1;
	NEW_CLASS(this->packetIdMut, Sync::Mutex());
	NEW_CLASS(this->hdlrMut, Sync::Mutex());
	NEW_CLASS(this->hdlrList, Data::ArrayList<Net::MQTTConn::PublishMessageHdlr>());
	NEW_CLASS(this->hdlrObjList, Data::ArrayList<void *>());
	this->hdlrList->Add(hdlr);
	this->hdlrObjList->Add(userObj);
	NEW_CLASS(this->topicMut, Sync::Mutex());
	NEW_CLASS(this->topicList, Data::ArrayList<const UTF8Char*>());

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
	Sync::MutexUsage mutUsage(this->connMut);
	SDEL_CLASS(this->conn);
	mutUsage.EndUse();
	DEL_CLASS(this->connMut);
	DEL_CLASS(this->hdlrMut);
	DEL_CLASS(this->hdlrList);
	DEL_CLASS(this->hdlrObjList);
	DEL_CLASS(this->topicMut);
	LIST_FREE_FUNC(this->topicList, Text::StrDelNew);
	DEL_CLASS(this->topicList);
	DEL_CLASS(this->packetIdMut);
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

void Net::MQTTStaticClient::HandlePublishMessage(Net::MQTTConn::PublishMessageHdlr hdlr, void *hdlrObj)
{
	Sync::MutexUsage mutUsage(this->hdlrMut);
	this->hdlrList->Add(hdlr);
	this->hdlrObjList->Add(hdlrObj);
}

Bool Net::MQTTStaticClient::Subscribe(const UTF8Char *topic)
{
	Sync::MutexUsage mutUsage(this->topicMut);
	this->topicList->Add(Text::StrCopyNew(topic));
	mutUsage.ReplaceMutex(this->connMut);
	if (this->conn == 0) return false;
	UInt16 packetId = GetNextPacketId();
	if (this->conn->SendSubscribe(packetId, topic))
	{
		if (this->conn->WaitSubAck(packetId, 30000) <= 2)
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
