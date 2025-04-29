#include "Stdafx.h"
#include "Net/MQTTFailoverClient.h"

void __stdcall Net::MQTTFailoverClient::FreeClient(NN<ClientInfo> cliInfo)
{
	MemFreeNN(cliInfo);
}

void __stdcall Net::MQTTFailoverClient::OnMessage(AnyType userObj, Text::CStringNN topic, const Data::ByteArrayR &buff)
{
	NN<ClientInfo> cliInfo = userObj.GetNN<ClientInfo>();
	if (cliInfo->me->foHdlr.GetCurrChannel() == cliInfo->client)
	{
		Sync::MutexUsage mutUsage(cliInfo->me->hdlrMut);
		UOSInt i = cliInfo->me->hdlrList.GetCount();
		while (i-- > 0)
		{
			Data::CallbackStorage<Net::MQTTConn::PublishMessageHdlr> cb = cliInfo->me->hdlrList.GetItem(i);
			cb.func(cb.userObj, topic, buff);
		}
	}
}

Net::MQTTFailoverClient::MQTTFailoverClient(Net::FailoverType foType, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, UInt16 kaSeconds) : foHdlr(foType)
{
	this->clif = clif;
	this->ssl = ssl;
	this->kaSeconds = kaSeconds;
}

Net::MQTTFailoverClient::~MQTTFailoverClient()
{
	this->cliList.FreeAll(FreeClient);
}

void Net::MQTTFailoverClient::AddClient(Text::CStringNN host, UInt16 port, Text::CString username, Text::CString password, Bool webSocket)
{
	NN<ClientInfo> cliInfo = MemAllocNN(ClientInfo);
	cliInfo->me = this;
	NEW_CLASSNN(cliInfo->client, Net::MQTTStaticClient(this->clif, this->ssl, host, port, username, password, webSocket, OnMessage, cliInfo, this->kaSeconds, 0));
	this->cliList.Add(cliInfo);
	this->foHdlr.AddChannel(cliInfo->client);
}

void Net::MQTTFailoverClient::HandlePublishMessage(Net::MQTTConn::PublishMessageHdlr hdlr, AnyType hdlrObj)
{
	Sync::MutexUsage mutUsage(this->hdlrMut);
	this->hdlrList.Add({hdlr, hdlrObj});
}

Bool Net::MQTTFailoverClient::Subscribe(Text::CStringNN topic)
{
	Data::ArrayListNN<Net::MQTTStaticClient> cliList;
	this->foHdlr.GetAllChannels(cliList);
	UOSInt i = cliList.GetCount();
	while (i-- > 0)
	{
		cliList.GetItemNoCheck(i)->Subscribe(topic);
	}
	return true;
}

Bool Net::MQTTFailoverClient::Publish(Text::CStringNN topic, Text::CStringNN message, Bool dup, UInt8 qos, Bool retain)
{
	NN<Net::MQTTStaticClient> cli;
	if (!this->foHdlr.GetCurrChannel().SetTo(cli))
	{
		return false;
	}
	if (cli->Publish(topic, message, dup, qos, retain))
	{
		return true;
	}
	Data::ArrayListNN<Net::MQTTStaticClient> cliList;
	this->foHdlr.GetOtherChannels(cliList);
	UOSInt i = 0;
	UOSInt j = cliList.GetCount();
	while (i < j)
	{
		if (cliList.GetItemNoCheck(i)->Publish(topic, message, dup, qos, retain))
		{
			this->foHdlr.SetCurrChannel(cliList.GetItemNoCheck(i));
			return true;
		}
		i++;
	}
	return false;
}
