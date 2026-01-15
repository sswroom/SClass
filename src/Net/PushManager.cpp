#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/PushManager.h"
#include "Net/Google/GoogleFCM.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

#define FILENAME CSTR("PushServer.dat")

NN<Net::PushManager::UserInfo> Net::PushManager::GetUser(Text::CStringNN userName)
{
	NN<UserInfo> user;
	if (!this->userMap.GetC(userName).SetTo(user))
	{
		NEW_CLASSNN(user, UserInfo());
		user->userName = Text::String::New(userName);
		this->userMap.PutNN(user->userName, user);
	}
	return user;
}

void Net::PushManager::LoadData()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, FILENAME);
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Net::SocketUtil::AddressInfo addr;
		MemClear(&addr, sizeof(addr));
		addr.addrType = Net::AddrType::Unknown;
		Text::StringBuilderUTF8 sb;
		Text::UTF8Reader reader(fs);
		Text::PString sarr[5];
		DeviceType devType;
		this->loading = true;
		while (reader.ReadLine(sb, 1024))
		{
			if (Text::StrSplitP(sarr, 3, sb, ',') == 3)
			{
				if (sarr[0].leng < 20)
				{
					if (Text::StrSplitP(&sarr[2], 3, sarr[2], ',') == 3)
					{
						if (sarr[3].Equals(UTF8STRC("ios")))
						{
							devType = DeviceType::IOS;
						}
						else
						{
							devType = DeviceType::Android;
						}
						this->Subscribe(sarr[2].ToCString(), sarr[4].ToCString(), devType, addr, sarr[1].ToCString());
						{
							Sync::MutexUsage mutUsage(this->dataMut);
							NN<DeviceInfo2> dev;
							if (this->devMap.GetC(sarr[2].ToCString()).SetTo(dev))
							{
								dev->lastSubscribeTime = Data::Timestamp(sarr[0].ToInt64(), Data::DateTimeUtil::GetLocalTzQhr());
							}
						}
					}
				}
				else
				{
					if (sarr[1].Equals(UTF8STRC("ios")))
					{
						devType = DeviceType::IOS;
					}
					else
					{
						devType = DeviceType::Android;
					}
					sarr[2].Trim();
					this->Subscribe(sarr[0].ToCString(), sarr[2].ToCString(), devType, addr, 0);
				}
			}

			sb.ClearStr();
		}
		this->loading = false;
	}
}

void Net::PushManager::SaveData()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, FILENAME);
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::StringBuilderUTF8 sb;
		NN<DeviceInfo2> dev;
		NN<Text::String> devModel;
		Text::UTF8Writer writer(fs);
		Sync::MutexUsage mutUsage(this->dataMut);
		UOSInt i = 0;
		UOSInt j = this->devMap.GetCount();
		while (i < j)
		{
			dev = this->devMap.GetItemNoCheck(i);
			sb.ClearStr();
			sb.AppendI64(dev->lastSubscribeTime.ToTicks());
			sb.AppendUTF8Char(',');
			if (dev->devModel.SetTo(devModel))
			{
				if (devModel->IndexOf(',') != INVALID_INDEX)
				{
					devModel->Replace(',', '_');
				}
				sb.Append(devModel);
			}
			sb.AppendUTF8Char(',');
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
			writer.WriteLine(sb.ToCString());
			i++;
		}
	}
}

Optional<Text::String> Net::PushManager::GetAccessToken()
{
	NN<Net::Google::AccessTokenResponse> accessToken;
	Data::Timestamp currTime = Data::Timestamp::Now();
	if (this->accessToken.SetTo(accessToken) && currTime < this->accessTokenExpire)
	{
		return accessToken->GetAccessToken();
	}
	this->accessToken.Delete();
	if (this->oauth2.GetServiceToken(this->serviceAccount, CSTR("https://www.googleapis.com/auth/firebase.messaging")).SetTo(accessToken))
	{
		this->accessToken = accessToken;
		this->accessTokenExpire = Data::Timestamp::Now().AddSecond(accessToken->GetExpiresIn());
		return accessToken->GetAccessToken();
	}
	return nullptr;
}

Net::PushManager::PushManager(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<Net::Google::GoogleServiceAccount> serviceAccount, NN<IO::LogTool> log) : fcm(clif, ssl, serviceAccount->GetProjectId()->ToCString()), oauth2(clif, ssl)
{
	this->serviceAccount = serviceAccount;
	this->log = log;
	this->loading = false;
	this->accessToken = nullptr;
	this->accessTokenExpire = 0;
	this->LoadData();
}

Net::PushManager::~PushManager()
{
	UOSInt i;
	i = this->userMap.GetCount();
	while (i-- > 0)
	{
		NN<UserInfo> user = this->userMap.GetItemNoCheck(i);
		user->userName->Release();
		user.Delete();
	}
	i = this->devMap.GetCount();
	while (i-- > 0)
	{
		NN<DeviceInfo2> dev = this->devMap.GetItemNoCheck(i);
		dev->token->Release();
		dev->userName->Release();
		OPTSTR_DEL(dev->devModel);
		MemFreeNN(dev);
	}
	this->accessToken.Delete();
}

Bool Net::PushManager::Subscribe(Text::CStringNN token, Text::CStringNN userName, DeviceType devType, NN<const Net::SocketUtil::AddressInfo> remoteAddr, Text::CString devModel)
{
	Sync::MutexUsage mutUsage(this->dataMut);
	NN<DeviceInfo2> dev;
	NN<UserInfo> user;
	Text::CStringNN nndevModel;
	NN<Text::String> sdevModel;
	if (this->devMap.GetC(token).SetTo(dev))
	{
		if (dev->userName->Equals(userName.v, userName.leng))
		{
			dev->lastSubscribeTime = Data::Timestamp::Now();
			dev->subscribeAddr = remoteAddr.Ptr()[0];
			if (devModel.SetTo(nndevModel) && nndevModel.leng > 0)
			{
				if (!dev->devModel.SetTo(sdevModel))
				{
					dev->devModel = Text::String::New(nndevModel);
				}
				else if (!sdevModel->Equals(nndevModel))
				{
					OPTSTR_DEL(dev->devModel);
					dev->devModel = Text::String::New(nndevModel);
				}
			}
			return true;
		}
		if (this->userMap.Get(dev->userName).SetTo(user))
		{
			user->devMap.RemoveC(token);
		}
		dev->userName->Release();
	}
	else
	{
		dev = MemAllocNN(DeviceInfo2);
		dev->token = Text::String::New(token);
		dev->devType = devType;
		dev->subscribeAddr.addrType = Net::AddrType::Unknown;
		dev->devModel = nullptr;
		dev->lastSubscribeTime = 0;
		this->devMap.PutNN(dev->token, dev);
	}
	user = this->GetUser(userName);
	dev->userName = user->userName->Clone();
	dev->lastSubscribeTime = Data::Timestamp::Now();
	dev->subscribeAddr = remoteAddr.Ptr()[0];
	if (devModel.SetTo(nndevModel) && nndevModel.leng > 0)
	{
		if (!dev->devModel.SetTo(sdevModel))
		{
			dev->devModel = Text::String::New(nndevModel);
		}
		else if (!sdevModel->Equals(nndevModel))
		{
			OPTSTR_DEL(dev->devModel);
			dev->devModel = Text::String::New(nndevModel);
		}
	}
	user->devMap.PutNN(dev->token, dev);
	if (!this->loading)
		this->SaveData();
	return true;
}

Bool Net::PushManager::Unsubscribe(Text::CStringNN token)
{
	Sync::MutexUsage mutUsage(this->dataMut);
	NN<DeviceInfo2> dev;
	NN<UserInfo> user;
	if (this->devMap.RemoveC(token).SetTo(dev))
	{
		if (this->userMap.Get(dev->userName).SetTo(user))
		{
			user->devMap.RemoveC(token);
		}
		dev->userName->Release();
		dev->token->Release();
		OPTSTR_DEL(dev->devModel);
		MemFreeNN(dev);
		this->SaveData();
		return true;
	}
	else
	{
		return false;
	}
}

Bool Net::PushManager::Send(NN<Data::ArrayListStringNN> userNames, NN<Text::String> message)
{
	Sync::MutexUsage mutUsage(this->dataMut);
	NN<UserInfo> user;
	Data::ArrayListStringNN tokenList;
	Data::ArrayIterator<NN<Text::String>> it = userNames->Iterator();
	while (it.HasNext())
	{
		if (this->userMap.GetNN(it.Next()).SetTo(user))
		{
			UOSInt k = user->devMap.GetCount();
			while (k-- > 0)
			{
				tokenList.Add(user->devMap.GetItemNoCheck(k)->token->Clone());
			}
		}
	}
	if (tokenList.GetCount() == 0)
	{
		this->log->LogMessage(CSTR("Send: Device not found"), IO::LogHandler::LogLevel::Error);
		return false;
	}
	else
	{
		Bool ret = false;
		NN<Text::String> accessToken;
		if (this->GetAccessToken().SetTo(accessToken))
		{
			Text::StringBuilderUTF8 sbResult;
			it = tokenList.Iterator();
			while (it.HasNext())
			{
				sbResult.ClearStr();
				sbResult.AppendC(UTF8STRC("Send Message result: "));
				ret |= this->fcm.SendMessage(accessToken->ToCString(), it.Next()->ToCString(), message->ToCString(), sbResult);
				this->log->LogMessage(sbResult.ToCString(), IO::LogHandler::LogLevel::Action);
			}
		}
		else
		{
			this->log->LogMessage(CSTR("Send: Error in getting access token"), IO::LogHandler::LogLevel::Error);
		}
		tokenList.FreeAll();
		return ret;
	}
}

UOSInt Net::PushManager::GetUsers(NN<Data::ArrayListStringNN> users, NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->dataMut);
	UOSInt i = 0;
	UOSInt j = this->userMap.GetCount();
	UOSInt ret = 0;
	NN<UserInfo> user;
	while (i < j)
	{
		user = this->userMap.GetItemNoCheck(i);
		if (user->devMap.GetCount() > 0)
		{
			users->Add(user->userName);
			ret++;
		}
		i++;
	}
	return ret;
}

NN<const Data::ReadingListNN<Net::PushManager::DeviceInfo2>> Net::PushManager::GetDevices(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->dataMut);
	return this->devMap;
}

void Net::PushManager::LogMessage(Text::CStringNN msg, IO::LogHandler::LogLevel logLev)
{
	this->log->LogMessage(msg, logLev);
}

Text::CStringNN Net::PushManager::DeviceTypeGetName(DeviceType devType)
{
	switch (devType)
	{
	case DeviceType::Android:
		return CSTR("Android");
	case DeviceType::IOS:
		return CSTR("iOS");
	default:
		return CSTR("Unknown");
	}
}
