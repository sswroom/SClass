#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA1.h"
#include "Data/ArrayList.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "Net/HTTPClient.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
#include "Map/GoogleMap/GoogleSearcher.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"
#include "Text/Locale.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Map::GoogleMap::GoogleSearcher::GoogleSearcher(Net::SocketFactory *sockf, const UTF8Char *gooKey, const UTF8Char *gooCliId, const UTF8Char *gooPrivKey, IO::IWriter *errWriter)
{
	this->sockf = sockf;
	this->errWriter = errWriter;
	this->lastIsError = 0;
	this->srchCnt = 0;
	if (gooCliId)
	{
		Text::Encoding enc(65001);
		Crypto::Encrypt::Base64 b64;
		const UTF8Char *tmpKeyStr;

		this->gooCliId = Text::StrCopyNew(gooCliId);
		tmpKeyStr = Text::StrCopyNew(gooPrivKey);
		this->gooPrivKey = MemAlloc(UInt8, Text::StrCharCnt(tmpKeyStr) + 1);
		Text::StrReplace((Char*)tmpKeyStr, '-', '+');
		Text::StrReplace((Char*)tmpKeyStr, '_', '/');
		this->gooPrivKeyLeng = b64.Decrypt(tmpKeyStr, Text::StrCharCnt(tmpKeyStr), this->gooPrivKey, 0);
		Text::StrDelNew(tmpKeyStr);
		
		this->gooKey = 0;
	}
	else
	{
		this->gooCliId = 0;
		this->gooPrivKey = 0;
		if (gooKey == 0)
		{
			this->gooKey = 0;
		}
		else
		{
			this->gooKey = Text::StrCopyNew(gooKey);
		}
	}
	NEW_CLASS(this->lastSrchDate, Data::DateTime());
	NEW_CLASS(this->mut, Sync::Mutex());
	this->lastSrchDate->SetCurrTimeUTC();
}

Map::GoogleMap::GoogleSearcher::~GoogleSearcher()
{
	if (this->gooCliId)
	{
		Text::StrDelNew(this->gooCliId);
		this->gooCliId = 0;
	}
	if (this->gooPrivKey)
	{
		MemFree(this->gooPrivKey);
		this->gooPrivKey = 0;
	}
	if (this->gooKey)
	{
		Text::StrDelNew(this->gooKey);
		this->gooKey = 0;
	}
	if (this->lastSrchDate)
	{
		DEL_CLASS(this->lastSrchDate);
		this->lastSrchDate = 0;
	}
	if (this->mut)
	{
		DEL_CLASS(this->mut);
		this->mut = 0;
	}
}

UTF8Char *Map::GoogleMap::GoogleSearcher::SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, const UTF8Char *lang)
{
	UTF8Char url[1024];
	UTF8Char *sptr;
	UTF8Char *urlStart;
	UInt8 databuff[2048];
	OSInt databuffSize;
	Data::DateTime currDt;
	Int32 i;
	Char *ptrs[3];

	Sync::MutexUsage mutUsage(mut);
	this->srchCnt++;
	currDt.SetCurrTimeUTC();
	this->lastIsError = 0;
	if ((i = (Int32)currDt.DiffMS(this->lastSrchDate)) < 200)
	{
		if (i >= 0)
		{
			Sync::Thread::Sleep(200 - i);
		}
	}

	Net::HTTPClient *cli;
	urlStart = sptr = Text::StrConcat(url, (const UTF8Char*)"http://maps.google.com");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"/maps/geo?q=");
	sptr = Text::StrDouble(sptr, lat);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
	sptr = Text::StrDouble(sptr, lon);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"&output=csv&oe=utf8&sensor=false");
	if (this->gooCliId)
	{
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"&client=");
		sptr = Text::StrConcat(sptr, this->gooCliId);

		UInt8 result[20];
		UInt8 result2[40];
		OSInt size = Text::StrConcatC(databuff, urlStart, sptr - urlStart) - databuff;
		Crypto::Hash::SHA1 sha;
		Crypto::Hash::HMAC hmac(&sha, this->gooPrivKey, this->gooPrivKeyLeng);
		hmac.Calc(databuff, size);
		hmac.GetValue(result);
		Crypto::Encrypt::Base64 b64;
		size = b64.Encrypt(result, 20, result2, 0);
		result2[size] = 0;
		Text::StrReplace((Char*)result2, '+', '-');
		Text::StrReplace((Char*)result2, '/', '_');
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"&signature=");
		sptr = Text::StrConcatC(sptr, result2, size);
	}
	else if (this->gooKey)
	{
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"&key=");
		sptr = Text::StrConcat(sptr, this->gooKey);
	}
	else
	{
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"&key=");
	}
	cli = Net::HTTPClient::CreateConnect(this->sockf, url, 0, true);
	if (!cli->IsError())
	{
		if (lang)
		{
			cli->AddHeader((const UTF8Char*)"Accept-Language", lang);
		}
		Int32 status = cli->GetRespStatus();
		OSInt readSize;
		databuffSize = 0;
		while ((readSize = cli->Read(&databuff[databuffSize], 2047 - databuffSize)) > 0)
		{
			databuffSize += readSize;
		}
		databuff[databuffSize] = 0;
		readSize = Text::StrSplit(ptrs, 3, (Char*)databuff, ',');
		status = Text::StrToInt32(ptrs[0]);
		if (status == 200)
		{
			sptr = buff;
			if (*ptrs[2] == '"')
			{
				status = (Int32)Text::StrIndexOf(&ptrs[2][1], '"');
				if (status >= 0)
				{
					if ((UOSInt)status < buffSize)
					{
						buff = Text::StrConcatC(buff, (const UTF8Char*)&ptrs[2][1], status);
					}
					else
					{
						buff = Text::StrConcatS(buff, (const UTF8Char*)ptrs[2], buffSize);
					}
				}
				else
				{
					buff = Text::StrConcatS(buff, (const UTF8Char*)ptrs[2], buffSize);
				}
			}
			else
			{
				buff = Text::StrConcatS(buff, (const UTF8Char*)ptrs[2], buffSize);
			}
			if (*sptr == 0)
			{
				buff = Text::StrConcatS(buff, (const UTF8Char*)"-", buffSize);
			}
		}
		else if (status == 602)
		{
			this->lastIsError = 1;
			errWriter->WriteLine((const UTF8Char*)"Google 602 error");
			*buff = 0;
		}
		else
		{
			this->lastIsError = 1;
			sptr = Text::StrConcat(url, (const UTF8Char*)"Google ");
			sptr = Text::StrInt32(sptr, status);
			sptr = Text::StrConcat(sptr, (const UTF8Char*)" Error");
			errWriter->WriteLine(url);
			*buff = 0;
		}
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		this->lastIsError = 2;
		*buff = 0;
		sb.Append((const UTF8Char*)"Cannot connect: ");
		sb.Append(url);
		errWriter->WriteLine(url);
	}
	this->lastSrchDate->SetCurrTimeUTC();
	DEL_CLASS(cli);
	mutUsage.EndUse();
	return buff;
}

UTF8Char *Map::GoogleMap::GoogleSearcher::SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid)
{
	if (this->lastIsError == 2)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.DiffMS(this->lastSrchDate) < 60000)
			return 0;
	}
	Text::Locale::LocaleEntry *ent = Text::Locale::GetLocaleEntry(lcid);
	if (ent == 0)
		return 0;
	return SearchName(buff, buffSize, lat, lon, ent->shortName);
}

UTF8Char *Map::GoogleMap::GoogleSearcher::CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid)
{
	if (this->lastIsError != 0)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.DiffMS(this->lastSrchDate) < 60000)
			return 0;
	}
	Text::Locale::LocaleEntry *ent = Text::Locale::GetLocaleEntry(lcid);
	if (ent == 0)
		return 0;
	return SearchName(buff, buffSize, lat, lon, ent->shortName);
}

Int32 Map::GoogleMap::GoogleSearcher::GetSrchCnt()
{
	return this->srchCnt;
}
