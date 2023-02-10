#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/GoogleFCM.h"
#include "Net/PushManager.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

#define FILENAME CSTR("PushServer.dat")

Net::PushManager::UserInfo *Net::PushManager::GetUser(Text::CString userName)
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

void Net::PushManager::LoadData()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, FILENAME);
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::StringBuilderUTF8 sb;
		Text::UTF8Reader reader(&fs);
		Text::PString sarr[3];
		DeviceType devType;
		this->loading = true;
		while (reader.ReadLine(&sb, 1024))
		{
			if (Text::StrSplitP(sarr, 3, sb, ',') == 3)
			{
				if (sarr[1].Equals(UTF8STRC("ios")))
				{
					devType = DeviceType::IOS;
				}
				else
				{
					devType = DeviceType::Android;
				}
				this->Subscribe(sarr[0].ToCString(), sarr[2].ToCString(), devType);
			}

			sb.ClearStr();
		}
		this->loading = false;
	}
}

void Net::PushManager::SaveData()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, FILENAME);
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::StringBuilderUTF8 sb;
		DeviceInfo *dev;
		Text::UTF8Writer writer(&fs);
		Sync::MutexUsage mutUsage(&this->dataMut);
		UOSInt i = 0;
		UOSInt j = this->devMap.GetCount();
		while (i < j)
		{
			dev = this->devMap.GetItem(i);
			sb.ClearStr();
			sb.Append(dev->token);
			if (dev->devType == DeviceType::IOS)
			{
				sb.AppendC(UTF8STRC(",ios,"));
			}
			else
			{
				sb.AppendC(UTF8STRC(",android,"));
			}
			sb.Append(dev->userName);
			writer.WriteLineCStr(sb.ToCString());
			i++;
		}
	}
}

Net::PushManager::PushManager(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString fcmKey, IO::LogTool *log)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->fcmKey = Text::String::New(fcmKey);
	this->log = log;
	this->loading = false;
	this->LoadData();
}

Net::PushManager::~PushManager()
{
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

Bool Net::PushManager::Subscribe(Text::CString token, Text::CString userName, DeviceType devType)
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
		this->devMap.Put(dev->token, dev);
	}
	user = this->GetUser(userName);
	dev->userName = user->userName->Clone();
	user->devMap.Put(dev->token, dev);
	if (!this->loading)
		this->SaveData();
	return true;
}

Bool Net::PushManager::Unsubscribe(Text::CString token)
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
		this->SaveData();
		return true;
	}
	else
	{
		return false;
	}
}

Bool Net::PushManager::Send(Data::ArrayList<Text::String*> *userNames, Text::String *message)
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

UOSInt Net::PushManager::GetUsers(Data::ArrayList<Text::String*> *users, Sync::MutexUsage *mutUsage)
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
