#include "Stdafx.h"
#include "MyMemory.h"
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
#include "Text/TextBinEnc/Base64Enc.h"

Map::GoogleMap::GoogleSearcher::GoogleSearcher(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::String *gooKey, Text::String *gooCliId, Text::String *gooPrivKey, IO::Writer *errWriter)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->errWriter = errWriter;
	this->lastIsError = 0;
	this->srchCnt = 0;
	if (gooCliId)
	{
		Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
		this->gooCliId = gooCliId->Clone();
		this->gooPrivKey = MemAlloc(UInt8, gooPrivKey->leng + 1);
		this->gooPrivKeyLeng = b64.DecodeBin(gooPrivKey->v, gooPrivKey->leng, this->gooPrivKey);
		this->gooKey = 0;
	}
	else
	{
		this->gooCliId = 0;
		this->gooPrivKey = 0;
		this->gooPrivKeyLeng = 0;
		this->gooKey = SCOPY_STRING(gooKey);
	}
	NEW_CLASS(this->lastSrchDate, Data::DateTime());
	NEW_CLASS(this->mut, Sync::Mutex());
	this->lastSrchDate->SetCurrTimeUTC();
}

Map::GoogleMap::GoogleSearcher::GoogleSearcher(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *gooKey, const UTF8Char *gooCliId, const UTF8Char *gooPrivKey, IO::Writer *errWriter)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->errWriter = errWriter;
	this->lastIsError = 0;
	this->srchCnt = 0;
	if (gooCliId)
	{
		Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
		UOSInt len = Text::StrCharCnt(gooPrivKey);
		this->gooCliId = Text::String::NewNotNull(gooCliId);
		this->gooPrivKey = MemAlloc(UInt8, len + 1);
		this->gooPrivKeyLeng = b64.DecodeBin(gooPrivKey, len, this->gooPrivKey);
		this->gooKey = 0;
	}
	else
	{
		this->gooCliId = 0;
		this->gooPrivKey = 0;
		this->gooKey = Text::String::NewOrNull(gooKey);
	}
	NEW_CLASS(this->lastSrchDate, Data::DateTime());
	NEW_CLASS(this->mut, Sync::Mutex());
	this->lastSrchDate->SetCurrTimeUTC();
}

Map::GoogleMap::GoogleSearcher::~GoogleSearcher()
{
	SDEL_STRING(this->gooCliId);
	if (this->gooPrivKey)
	{
		MemFree(this->gooPrivKey);
		this->gooPrivKey = 0;
	}
	SDEL_STRING(this->gooKey);
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
	UOSInt databuffSize;
	Data::DateTime currDt;
	OSInt si;
	UOSInt i;
	Char *ptrs[3];

	Sync::MutexUsage mutUsage(mut);
	this->srchCnt++;
	currDt.SetCurrTimeUTC();
	this->lastIsError = 0;
	if ((si = (OSInt)currDt.DiffMS(this->lastSrchDate)) < 200)
	{
		if (si >= 0)
		{
			Sync::Thread::Sleep((UOSInt)(200 - si));
		}
	}

	Net::HTTPClient *cli;
	urlStart = sptr = Text::StrConcatC(url, UTF8STRC("http://maps.google.com"));
	sptr = Text::StrConcatC(sptr, UTF8STRC("/maps/geo?q="));
	sptr = Text::StrDouble(sptr, lat);
	sptr = Text::StrConcatC(sptr, UTF8STRC(","));
	sptr = Text::StrDouble(sptr, lon);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&output=csv&oe=utf8&sensor=false"));
	if (this->gooCliId)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("&client="));
		sptr = this->gooCliId->ConcatTo(sptr);

		UInt8 result[20];
		Crypto::Hash::SHA1 sha;
		Crypto::Hash::HMAC hmac(&sha, this->gooPrivKey, this->gooPrivKeyLeng);
		hmac.Calc(urlStart, (UOSInt)(sptr - urlStart));
		hmac.GetValue(result);
		Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, false);
		sptr = Text::StrConcatC(sptr, UTF8STRC("&signature="));
		sptr = b64.EncodeBin(sptr, result, 20);
	}
	else if (this->gooKey)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("&key="));
		sptr = this->gooKey->ConcatTo(sptr);
	}
	else
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("&key="));
	}
	cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, url, 0, true);
	if (!cli->IsError())
	{
		if (lang)
		{
			cli->AddHeader((const UTF8Char*)"Accept-Language", lang);
		}
		Int32 status = cli->GetRespStatus();
		UOSInt readSize;
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
				i = Text::StrIndexOf(&ptrs[2][1], '"');
				if (i != INVALID_INDEX)
				{
					if (i < buffSize)
					{
						buff = Text::StrConcatC(buff, (const UTF8Char*)&ptrs[2][1], i);
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
			errWriter->WriteLineC(UTF8STRC("Google 602 error"));
			*buff = 0;
		}
		else
		{
			this->lastIsError = 1;
			sptr = Text::StrConcatC(url, UTF8STRC("Google "));
			sptr = Text::StrInt32(sptr, status);
			sptr = Text::StrConcatC(sptr, UTF8STRC(" Error"));
			errWriter->WriteLineC(url, (UOSInt)(sptr - url));
			*buff = 0;
		}
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		this->lastIsError = 2;
		*buff = 0;
		sb.AppendC(UTF8STRC("Cannot connect: "));
		sb.AppendC(url, (UOSInt)(sptr - url));
		errWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	this->lastSrchDate->SetCurrTimeUTC();
	DEL_CLASS(cli);
	mutUsage.EndUse();
	return buff;
}

UTF8Char *Map::GoogleMap::GoogleSearcher::SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid)
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

UTF8Char *Map::GoogleMap::GoogleSearcher::CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid)
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

UInt32 Map::GoogleMap::GoogleSearcher::GetSrchCnt()
{
	return this->srchCnt;
}
