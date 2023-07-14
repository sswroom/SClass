#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/MQTTStaticClient.h"
#include "Net/WebSocketClient.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"

UInt32 __stdcall Net::MQTTStaticClient::KAThread(void *userObj)
{
	Net::MQTTStaticClient *me = (Net::MQTTStaticClient*)userObj;
	me->kaRunning = true;
	while (!me->kaToStop)
	{
		Sync::MutexUsage mutUsage(&me->connMut);
		if (me->conn)
		{
			me->conn->ClearPackets();
			me->conn->SendPing();
			mutUsage.EndUse();
		}
		else
		{
			mutUsage.EndUse();
			if (me->errLog) me->errLog->WriteLineC(UTF8STRC("MQTT: Reconnecting to server"));
			me->Connect();
		}
		me->kaEvt.Wait((UOSInt)me->kaSeconds * 500);
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
	Sync::MutexUsage mutUsage(&me->connMut);
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
	if (this->webSocket)
	{
		NotNullPtr<Net::WebSocketClient> ws;
		NEW_CLASSNN(ws, Net::WebSocketClient(this->sockf, this->ssl, this->host->ToCString(), this->port, CSTR("/mqtt"), CSTR_NULL, Net::WebSocketClient::Protocol::MQTT, this->connTimeout));
		if (ws->IsDown())
		{
			ws.Delete();
			if (errLog) errLog->WriteLineC(UTF8STRC("MQTT: Error in initializing websocket"));
			return;
		}
		NEW_CLASS(conn, Net::MQTTConn(ws, 0, 0));
	}
	else
	{
		NEW_CLASS(conn, Net::MQTTConn(this->sockf, this->ssl, this->host->ToCString(), this->port, OnDisconnect, this, this->connTimeout));
	}
	if (conn->IsError())
	{
		if (errLog) errLog->WriteLineC(UTF8STRC("MQTT: Error in connecting to server"));
		DEL_CLASS(conn);
		return;
	}
	this->packetId = 1;
	Sync::MutexUsage mutUsage(&this->hdlrMut);
	UOSInt i = 0;
	UOSInt j = this->hdlrList.GetCount();
	while (i < j)
	{
		conn->HandlePublishMessage(this->hdlrList.GetItem(i), this->hdlrObjList.GetItem(i));
		i++;
	}
	mutUsage.EndUse();

	Bool succ = conn->SendConnect(4, this->kaSeconds, this->clientId->ToCString(), this->username->ToCString(), this->password->ToCString());
	if (succ)
	{
		Net::MQTTConn::ConnectStatus status = conn->WaitConnAck(30000);
		succ = (status == Net::MQTTConn::CS_ACCEPTED);
	}
	if (!succ)
	{
		DEL_CLASS(conn);
		if (errLog) errLog->WriteLineC(UTF8STRC("MQTT: Error in sending connect packet"));
	}
	else
	{
		mutUsage.ReplaceMutex(&this->connMut);
		if (conn->IsError())
		{
			if (errLog) errLog->WriteLineC(UTF8STRC("MQTT: Connection is error"));
			DEL_CLASS(conn);
			return;
		}
		else
		{
			this->conn = conn;
		}
		Data::ArrayList<Text::String*> topicList;
		mutUsage.ReplaceMutex(&this->topicMut);
		topicList.AddAll(&this->topicList);
		mutUsage.EndUse();
		
		i = 0;
		j = topicList.GetCount();
		while (i < j)
		{
			mutUsage.ReplaceMutex(&this->connMut);
			if (this->conn == 0)
			{
				mutUsage.EndUse();
				break;
			}
			UInt16 packetId = GetNextPacketId();
			if (this->conn->SendSubscribe(packetId, topicList.GetItem(i)->ToCString()))
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
	Sync::MutexUsage mutUsage(&this->packetIdMut);
	return this->packetId++;
}

Net::MQTTStaticClient::MQTTStaticClient(Net::MQTTConn::PublishMessageHdlr hdlr, void *userObj, IO::Writer *errLog)
{
	this->kaRunning = false;
	this->kaToStop = false;
	this->kaSeconds = 30;
	this->conn = 0;
	this->errLog = errLog;
	this->connTimeout = 30000;

	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("sswrMQTT/"));
	sb.AppendI64(dt.ToTicks());
	this->clientId = Text::String::New(sb.ToString(), sb.GetLength());
	this->packetId = 1;
	this->hdlrList.Add(hdlr);
	this->hdlrObjList.Add(userObj);

	this->sockf = 0;
	this->ssl = 0;
	this->host = 0;
	this->port = 0;
	this->username = 0;
	this->password = 0;
	this->webSocket = false;
}

Net::MQTTStaticClient::MQTTStaticClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString host, UInt16 port, Text::CString username, Text::CString password, Bool webSocket, Net::MQTTConn::PublishMessageHdlr hdlr, void *userObj, UInt16 kaSeconds, IO::Writer *errLog) : Net::MQTTStaticClient(hdlr, userObj, errLog)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->host = Text::String::New(host).Ptr();
	this->port = port;
	this->username = Text::String::New(username).Ptr();
	this->password = Text::String::New(password).Ptr();
	this->kaSeconds = kaSeconds;
	this->webSocket = webSocket;

	this->Connect();

	Sync::ThreadUtil::Create(KAThread, this);
	while (!this->kaRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

Net::MQTTStaticClient::~MQTTStaticClient()
{
	if (this->kaRunning)
	{
		this->kaToStop = true;
		this->kaEvt.Set();
		while (this->kaRunning)
		{
			Sync::SimpleThread::Sleep(10);
		}
	}
	Sync::MutexUsage mutUsage(&this->connMut);
	SDEL_CLASS(this->conn);
	mutUsage.EndUse();
	LIST_FREE_STRING(&this->topicList);
	SDEL_STRING(this->username);
	SDEL_STRING(this->password);
	this->host->Release();
	this->clientId->Release();
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
	Sync::MutexUsage mutUsage(&this->hdlrMut);
	this->hdlrList.Add(hdlr);
	this->hdlrObjList.Add(hdlrObj);
}

Bool Net::MQTTStaticClient::Subscribe(Text::CString topic)
{
	Sync::MutexUsage mutUsage(&this->topicMut);
	this->topicList.Add(Text::String::New(topic).Ptr());
	mutUsage.ReplaceMutex(&this->connMut);
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

Bool Net::MQTTStaticClient::Publish(Text::CString topic, Text::CString message)
{
	Sync::MutexUsage mutUsage(&this->connMut);
	if (this->conn == 0) return false;
	return this->conn->SendPublish(topic, message);
}
