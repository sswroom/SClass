#include "Stdafx.h"
#include "Net/MQTTFailoverClient.h"

void Net::MQTTFailoverClient::FreeClient(ClientInfo *cliInfo)
{
	MemFree(cliInfo);
}

void __stdcall Net::MQTTFailoverClient::OnMessage(void *userObj, const UTF8Char *topic, const UInt8 *buff, UOSInt buffSize)
{
	ClientInfo *cliInfo = (ClientInfo *)userObj;
	if (cliInfo->me->foHdlr->GetCurrChannel() == cliInfo->client)
	{
		Sync::MutexUsage mutUsage(cliInfo->me->hdlrMut);
		UOSInt i = cliInfo->me->hdlrList->GetCount();
		while (i-- > 0)
		{
			cliInfo->me->hdlrList->GetItem(i)(cliInfo->me->hdlrObjList->GetItem(i), topic, buff, buffSize);
		}
	}
}

Net::MQTTFailoverClient::MQTTFailoverClient(Net::FailoverType foType, Net::SocketFactory *sockf, Net::SSLEngine *ssl, UInt16 kaSeconds)
{
	NEW_CLASS(this->foHdlr, Net::FailoverHandler<MQTTStaticClient>(foType));
	this->sockf = sockf;
	this->ssl = ssl;
	this->kaSeconds = kaSeconds;
	NEW_CLASS(this->hdlrMut, Sync::Mutex());
	NEW_CLASS(this->hdlrList, Data::ArrayList<Net::MQTTConn::PublishMessageHdlr>());
	NEW_CLASS(this->hdlrObjList, Data::ArrayList<void *>());
	NEW_CLASS(this->cliList, Data::ArrayList<ClientInfo*>());
}

Net::MQTTFailoverClient::~MQTTFailoverClient()
{
	DEL_CLASS(this->foHdlr);
	DEL_CLASS(this->hdlrMut);
	DEL_CLASS(this->hdlrList);
	DEL_CLASS(this->hdlrObjList);
	LIST_FREE_FUNC(this->cliList, this->FreeClient);
	DEL_CLASS(this->cliList);
}

void Net::MQTTFailoverClient::AddClient(const UTF8Char *host, UInt16 port, const UTF8Char *username, const UTF8Char *password)
{
	ClientInfo *cliInfo = MemAlloc(ClientInfo, 1);
	cliInfo->me = this;
	NEW_CLASS(cliInfo->client, Net::MQTTStaticClient(this->sockf, this->ssl, host, port, username, password, OnMessage, cliInfo, this->kaSeconds));
	this->cliList->Add(cliInfo);
	this->foHdlr->AddChannel(cliInfo->client);
}

void Net::MQTTFailoverClient::HandlePublishMessage(Net::MQTTConn::PublishMessageHdlr hdlr, void *hdlrObj)
{
	Sync::MutexUsage mutUsage(this->hdlrMut);
	this->hdlrList->Add(hdlr);
	this->hdlrObjList->Add(hdlrObj);
}

Bool Net::MQTTFailoverClient::Subscribe(const UTF8Char *topic)
{
	Data::ArrayList<Net::MQTTStaticClient*> cliList;
	this->foHdlr->GetAllChannels(&cliList);
	UOSInt i = cliList.GetCount();
	while (i-- > 0)
	{
		cliList.GetItem(i)->Subscribe(topic);
	}
	return true;
}

Bool Net::MQTTFailoverClient::Publish(const UTF8Char *topic, const UTF8Char *message)
{
	Net::MQTTStaticClient *cli = this->foHdlr->GetCurrChannel();
	if (cli == 0)
	{
		return false;
	}
	if (cli->Publish(topic, message))
	{
		return true;
	}
	Data::ArrayList<Net::MQTTStaticClient*> cliList;
	this->foHdlr->GetOtherChannels(&cliList);
	UOSInt i = 0;
	UOSInt j = cliList.GetCount();
	while (i < j)
	{
		if (cliList.GetItem(i)->Publish(topic, message))
		{
			this->foHdlr->SetCurrChannel(cliList.GetItem(i));
			return true;
		}
		i++;
	}
	return false;
}
