#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/MQTTStaticClient.h"
#include "Net/WebSocketClient.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::MQTTStaticClient::KAThread(NN<Sync::Thread> thread)
{
	NN<Net::MQTTStaticClient> me = thread->GetUserObj().GetNN<Net::MQTTStaticClient>();
	while (!thread->IsStopping())
	{
		NN<IO::Writer> errLog;
		Sync::MutexUsage mutUsage(me->connMut);
		NN<Net::MQTTConn> conn;
		if (me->conn.SetTo(conn))
		{
			conn->ClearPackets();
			conn->SendPing();
			mutUsage.EndUse();
		}
		else
		{
			mutUsage.EndUse();
			if (me->errLog.SetTo(errLog)) errLog->WriteLine(CSTR("MQTT: Reconnecting to server"));
			me->Connect();
		}
		thread->Wait((Int64)me->kaSeconds * 500);
	}
}

void __stdcall Net::MQTTStaticClient::OnDisconnect(AnyType userObj)
{
	NN<Net::MQTTStaticClient> me = userObj.GetNN<Net::MQTTStaticClient>();
	if (!me->kaThread.IsRunning())
	{
		return;
	}
	Sync::MutexUsage mutUsage(me->connMut);
	me->conn.Delete();
	//me->kaEvt->Set();
}

void Net::MQTTStaticClient::Connect()
{
	NN<Text::String> host;
	NN<IO::Writer> errLog;
	if (!this->host.SetTo(host))
	{
		return;
	}
	NN<Net::MQTTConn> conn;
	if (this->webSocket)
	{
		NN<Net::WebSocketClient> ws;
		NEW_CLASSNN(ws, Net::WebSocketClient(this->clif, this->ssl, host->ToCString(), this->port, CSTR("/mqtt"), nullptr, Net::WebSocketClient::Protocol::MQTT, this->connTimeout));
		if (ws->IsDown())
		{
			ws.Delete();
			if (this->errLog.SetTo(errLog)) errLog->WriteLine(CSTR("MQTT: Error in initializing websocket"));
			return;
		}
		NEW_CLASSNN(conn, Net::MQTTConn(ws, 0, 0));
	}
	else
	{
		NEW_CLASSNN(conn, Net::MQTTConn(this->clif, this->ssl, host->ToCString(), this->port, OnDisconnect, this, this->connTimeout));
	}
	if (conn->IsError())
	{
		if (this->errLog.SetTo(errLog)) errLog->WriteLine(CSTR("MQTT: Error in connecting to server"));
		conn.Delete();
		return;
	}
	this->packetId = 1;
	Sync::MutexUsage mutUsage(this->hdlrMut);
	UOSInt i = 0;
	UOSInt j = this->hdlrList.GetCount();
	while (i < j)
	{
		Data::CallbackStorage<Net::MQTTConn::PublishMessageHdlr> cb = this->hdlrList.GetItem(i);
		conn->HandlePublishMessage(cb.func, cb.userObj);
		i++;
	}
	mutUsage.EndUse();

	Bool succ = conn->SendConnect(4, this->kaSeconds, this->clientId->ToCString(), OPTSTR_CSTR(this->username), OPTSTR_CSTR(this->password));
	if (succ)
	{
		Net::MQTTConn::ConnectStatus status = conn->WaitConnAck(30000);
		succ = (status == Net::MQTTConn::CS_ACCEPTED);
	}
	if (!succ)
	{
		conn.Delete();
		if (this->errLog.SetTo(errLog)) errLog->WriteLine(CSTR("MQTT: Error in sending connect packet"));
	}
	else
	{
		mutUsage.ReplaceMutex(this->connMut);
		if (conn->IsError())
		{
			if (this->errLog.SetTo(errLog)) errLog->WriteLine(CSTR("MQTT: Connection is error"));
			conn.Delete();
			return;
		}
		else
		{
			this->conn = conn;
		}
		Data::ArrayListNN<Text::String> topicList;
		mutUsage.ReplaceMutex(this->topicMut);
		topicList.AddAll(this->topicList);
		mutUsage.EndUse();
		
		i = 0;
		j = topicList.GetCount();
		while (i < j)
		{
			mutUsage.ReplaceMutex(this->connMut);
			if (!this->conn.SetTo(conn))
			{
				mutUsage.EndUse();
				break;
			}
			UInt16 packetId = GetNextPacketId();
			if (conn->SendSubscribe(packetId, topicList.GetItemNoCheck(i)->ToCString()))
			{
				conn->WaitSubAck(packetId, 30000);
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

void Net::MQTTStaticClient::Init(NN<Net::TCPClientFactory> clif, Net::MQTTConn::PublishMessageHdlr hdlr, AnyType userObj, Optional<IO::Writer> errLog)
{
	this->kaSeconds = 30;
	this->conn = nullptr;
	this->errLog = errLog;
	this->connTimeout = 30000;

	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("sswrMQTT/"));
	sb.AppendI64(Data::DateTimeUtil::GetCurrTimeMillis());
	this->clientId = Text::String::New(sb.ToString(), sb.GetLength());
	this->packetId = 1;
	this->hdlrList.Add({hdlr, userObj});

	this->clif = clif;
	this->ssl = nullptr;
	this->host = nullptr;
	this->port = 0;
	this->username = nullptr;
	this->password = nullptr;
	this->webSocket = false;
}

Net::MQTTStaticClient::MQTTStaticClient(NN<Net::TCPClientFactory> clif, Net::MQTTConn::PublishMessageHdlr hdlr, AnyType userObj, Optional<IO::Writer> errLog) : kaThread(KAThread, this, CSTR("MQTTStaticCliKA"))
{
	this->Init(clif, hdlr, userObj, errLog);
}

Net::MQTTStaticClient::MQTTStaticClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, Text::CString username, Text::CString password, Bool webSocket, Net::MQTTConn::PublishMessageHdlr hdlr, AnyType userObj, UInt16 kaSeconds, Optional<IO::Writer> errLog) : kaThread(KAThread, this, CSTR("MQTTStaticCliKA"))
{
	this->Init(clif, hdlr, userObj, errLog);
	this->ssl = ssl;
	this->host = Text::String::New(host);
	this->port = port;
	this->username = Text::String::NewOrNull(username);
	this->password = Text::String::NewOrNull(password);
	this->kaSeconds = kaSeconds;
	this->webSocket = webSocket;

	this->Connect();
	this->kaThread.Start();
}

Net::MQTTStaticClient::~MQTTStaticClient()
{
	this->kaThread.Stop();
	Sync::MutexUsage mutUsage(this->connMut);
	this->conn.Delete();
	mutUsage.EndUse();
	NNLIST_FREE_STRING(&this->topicList);
	OPTSTR_DEL(this->username);
	OPTSTR_DEL(this->password);
	OPTSTR_DEL(this->host);
	this->clientId->Release();
}

Bool Net::MQTTStaticClient::IsStarted()
{
	return this->kaThread.IsRunning();
}

Bool Net::MQTTStaticClient::ChannelFailure()
{
	return this->conn.IsNull();
}

void Net::MQTTStaticClient::HandlePublishMessage(Net::MQTTConn::PublishMessageHdlr hdlr, AnyType hdlrObj)
{
	Sync::MutexUsage mutUsage(this->hdlrMut);
	this->hdlrList.Add({hdlr, hdlrObj});
}

Bool Net::MQTTStaticClient::Subscribe(Text::CStringNN topic)
{
	Sync::MutexUsage mutUsage(this->topicMut);
	NN<Net::MQTTConn> conn;
	this->topicList.Add(Text::String::New(topic));
	mutUsage.ReplaceMutex(this->connMut);
	if (!this->conn.SetTo(conn)) return false;
	UInt16 packetId = GetNextPacketId();
	if (conn->SendSubscribe(packetId, topic))
	{
		if (conn->WaitSubAck(packetId, 30000) <= 2)
		{
			return true;
		}
	}
	return false;
}

Bool Net::MQTTStaticClient::Publish(Text::CStringNN topic, Text::CStringNN message, Bool dup, UInt8 qos, Bool retain)
{
	Sync::MutexUsage mutUsage(this->connMut);
	NN<Net::MQTTConn> conn;
	if (!this->conn.SetTo(conn)) return false;
	return conn->SendPublish(topic, message, dup, qos, retain);
}
