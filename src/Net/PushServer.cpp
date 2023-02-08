#include "Stdafx.h"
#include "Net/PushServer.h"
#include "Net/PushServerHandler.h"

Net::PushServer::UserInfo *Net::PushServer::GetUser(Text::CString userName)
{
	UserInfo *user = this->userMap.GetC(userName);
	if (user == 0)
	{
		NEW_CLASS(user, UserInfo());
		user->userName = Text::String::New(userName);
		this->userMap.Put(user->userName, user);
	}
	return user;
}

Net::PushServer::PushServer(Net::SocketFactory *sockf, Net::SSLEngine *ssl, UInt16 port, Text::CString fcmKey, IO::LogTool *log)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->listener = 0;
	this->webHdlr = 0;
	this->fcmKey = Text::String::New(fcmKey);
	this->log = log;
	NEW_CLASS(this->webHdlr, Net::PushServerHandler(this));
	NEW_CLASS(this->listener, Net::WebServer::WebListener(sockf, 0, this->webHdlr, port, 120, 4, CSTR("PushServer/1.0"), false, true, true));
	if (this->listener->IsError())
	{
		DEL_CLASS(this->listener);
		this->listener = 0;
	}
	else
	{
		if (log)
			this->listener->SetAccessLog(log, IO::ILogHandler::LogLevel::Action);
	}
}

Net::PushServer::~PushServer()
{
	SDEL_CLASS(this->listener);
	SDEL_CLASS(this->webHdlr);
	UOSInt i;
	i = this->userMap.GetCount();
	while (i-- > 0)
	{
		UserInfo *user = this->userMap.GetItem(i);
		user->userName->Release();
		DEL_CLASS(user);
	}
	i = this->devMap.GetCount();
	while (i-- > 0)
	{
		DeviceInfo *dev = this->devMap.GetItem(i);
		dev->token->Release();
		dev->userName->Release();
		MemFree(dev);
	}
	this->fcmKey->Release();
}

Bool Net::PushServer::IsError()
{
	return this->listener == 0;
}

Bool Net::PushServer::Subscribe(Text::CString token, Text::CString userName, DeviceType devType)
{
	Sync::MutexUsage mutUsage(&this->dataMut);
	DeviceInfo *dev = this->devMap.GetC(token);
	UserInfo *user;
	if (dev)
	{
		if (dev->userName->Equals(userName.v, userName.leng))
			return true;
		user = this->userMap.Get(dev->userName);
		if (user)
		{
			user->devMap.RemoveC(token);
		}
		dev->userName->Release();
	}
	else
	{
		dev = MemAlloc(DeviceInfo, 1);
		dev->token = Text::String::New(token);
		dev->userName = 0;
		dev->devType = devType;
	}
	user = this->GetUser(userName);
	dev->userName = user->userName->Clone();
	user->devMap.Put(dev->token, dev);
	return true;
}

Bool Net::PushServer::Unsubscribe(Text::CString token)
{
	Sync::MutexUsage mutUsage(&this->dataMut);
	DeviceInfo *dev = this->devMap.RemoveC(token);
	UserInfo *user;
	if (dev)
	{
		user = this->userMap.Get(dev->userName);
		if (user)
		{
			user->devMap.RemoveC(token);
		}
		dev->userName->Release();
		dev->token->Release();
		MemFree(dev);
		return true;
	}
	else
	{
		return false;
	}
}

Bool Net::PushServer::Send(Data::ArrayList<Text::String*> *userNames, Text::String *message)
{
	Sync::MutexUsage mutUsage(&this->dataMut);
	UserInfo *user;
	Data::ArrayList<Text::String*> tokenList;
	UOSInt i = 0;
	UOSInt j = userNames->GetCount();
	while (i < j)
	{
		user = this->userMap.Get(userNames->GetItem(i));
		if (user)
		{
			UOSInt k = user->devMap.GetCount();
			while (k-- > 0)
			{
				tokenList.Add(user->devMap.GetItem(k)->token->Clone());
			}
		}
		i++;
	}
	if (tokenList.GetCount() == 0)
	{
		if (this->log)
			this->log->LogMessage(CSTR("Send: Device not found"), IO::ILogHandler::LogLevel::Error);
		return false;
	}
	Text::StringBuilderUTF8 sbResult;
	sbResult.AppendC(UTF8STRC("Send Message result: "));
	Bool ret = Net::GoogleFCM::SendMessage(this->sockf, this->ssl, this->fcmKey->ToCString(), &tokenList, message->ToCString(), &sbResult);
	if (this->log)
		this->log->LogMessage(sbResult.ToCString(), IO::ILogHandler::LogLevel::Action);
	LIST_FREE_STRING(&tokenList);
	return ret;
}

UOSInt Net::PushServer::GetUsers(Data::ArrayList<Text::String*> *users, Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(&this->dataMut);
	UOSInt i = 0;
	UOSInt j = this->userMap.GetCount();
	UOSInt ret = 0;
	UserInfo *user;
	while (i < j)
	{
		user = this->userMap.GetItem(i);
		if (user->devMap.GetCount() > 0)
		{
			users->Add(user->userName);
			ret++;
		}
		i++;
	}
	return ret;
}
