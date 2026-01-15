#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA1.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "Net/HTTPClient.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
#include "Map/GoogleMap/GoogleSearcher.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Text/Encoding.h"
#include "Text/Locale.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/TextBinEnc/Base64Enc.h"

Map::GoogleMap::GoogleSearcher::GoogleSearcher(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::String> gooKey, Optional<Text::String> gooCliId, Optional<Text::String> gooPrivKey, NN<IO::Writer> errWriter)
{
	this->clif = clif;
	this->ssl = ssl;
	this->errWriter = errWriter;
	this->lastIsError = 0;
	this->srchCnt = 0;
	NN<Text::String> nngooCliId;
	NN<Text::String> nngooPrivKey;
	if (gooCliId.SetTo(nngooCliId) && gooPrivKey.SetTo(nngooPrivKey))
	{
		Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
		this->gooCliId = nngooCliId->Clone();
		UnsafeArray<UInt8> gooPrivKeyArr;
		this->gooPrivKey = gooPrivKeyArr = MemAllocArr(UInt8, nngooPrivKey->leng + 1);
		this->gooPrivKeyLeng = b64.DecodeBin(nngooPrivKey->ToCString(), gooPrivKeyArr);
		this->gooKey = nullptr;
	}
	else
	{
		this->gooCliId = nullptr;
		this->gooPrivKey = nullptr;
		this->gooPrivKeyLeng = 0;
		this->gooKey = Text::String::CopyOrNull(gooKey);
	}
	this->lastSrchDate.SetCurrTimeUTC();
}

Map::GoogleMap::GoogleSearcher::GoogleSearcher(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CString gooKey, Text::CString gooCliId, Text::CString gooPrivKey, NN<IO::Writer> errWriter)
{
	this->clif = clif;
	this->ssl = ssl;
	this->errWriter = errWriter;
	this->lastIsError = 0;
	this->srchCnt = 0;
	Text::CStringNN nngooCliId;
	Text::CStringNN nngooPrivKey;
	if (gooCliId.SetTo(nngooCliId) && gooPrivKey.SetTo(nngooPrivKey) && nngooCliId.leng > 0)
	{
		Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, true);
		this->gooCliId = Text::String::New(nngooCliId).Ptr();
		UnsafeArray<UInt8> gooPrivKeyArr;
		this->gooPrivKey = gooPrivKeyArr = MemAllocArr(UInt8, gooPrivKey.leng + 1);
		this->gooPrivKeyLeng = b64.DecodeBin(gooPrivKey.OrEmpty(), gooPrivKeyArr);
		this->gooKey = nullptr;
	}
	else
	{
		this->gooCliId = nullptr;
		this->gooPrivKey = nullptr;
		this->gooKey = Text::String::NewOrNull(gooKey);
	}
	this->lastSrchDate.SetCurrTimeUTC();
}

Map::GoogleMap::GoogleSearcher::~GoogleSearcher()
{
	UnsafeArray<UInt8> gooPrivKey;
	OPTSTR_DEL(this->gooCliId);
	if (this->gooPrivKey.SetTo(gooPrivKey))
	{
		MemFreeArr(gooPrivKey);
		this->gooPrivKey = nullptr;
	}
	OPTSTR_DEL(this->gooKey);
}

UnsafeArrayOpt<UTF8Char> Map::GoogleMap::GoogleSearcher::SearchName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos, Text::CString lang)
{
	UTF8Char url[1024];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> urlStart;
	UInt8 databuff[2048];
	UIntOS databuffSize;
	Data::DateTime currDt;
	IntOS si;
	UIntOS i;
	UnsafeArray<UTF8Char> ptrs[3];

	Sync::MutexUsage mutUsage(this->mut);
	this->srchCnt++;
	currDt.SetCurrTimeUTC();
	this->lastIsError = 0;
	if ((si = (IntOS)currDt.DiffMS(this->lastSrchDate)) < 200)
	{
		if (si >= 0)
		{
			Sync::SimpleThread::Sleep((UIntOS)(200 - si));
		}
	}

	NN<Text::String> s;
	NN<Net::HTTPClient> cli;
	urlStart = sptr = Text::StrConcatC(url, UTF8STRC("http://maps.google.com"));
	sptr = Text::StrConcatC(sptr, UTF8STRC("/maps/geo?q="));
	sptr = Text::StrDouble(sptr, pos.GetLat());
	sptr = Text::StrConcatC(sptr, UTF8STRC(","));
	sptr = Text::StrDouble(sptr, pos.GetLon());
	sptr = Text::StrConcatC(sptr, UTF8STRC("&output=csv&oe=utf8&sensor=false"));
	NN<Text::String> gooCliId;
	UnsafeArray<UInt8> gooPrivKey;
	if (this->gooCliId.SetTo(gooCliId) && this->gooPrivKey.SetTo(gooPrivKey))
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("&client="));
		sptr = gooCliId->ConcatTo(sptr);

		UInt8 result[20];
		Crypto::Hash::SHA1 sha;
		Crypto::Hash::HMAC hmac(sha, gooPrivKey, this->gooPrivKeyLeng);
		hmac.Calc(urlStart, (UIntOS)(sptr - urlStart));
		hmac.GetValue(result);
		Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, false);
		sptr = Text::StrConcatC(sptr, UTF8STRC("&signature="));
		sptr = b64.EncodeBin(sptr, result, 20);
	}
	else if (this->gooKey.SetTo(s))
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("&key="));
		sptr = s->ConcatTo(sptr);
	}
	else
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("&key="));
	}
	cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (!cli->IsError())
	{
		if (lang.leng)
		{
			cli->AddHeaderC(CSTR("Accept-Language"), lang);
		}
		Int32 status = cli->GetRespStatus();
		UIntOS readSize;
		databuffSize = 0;
		while ((readSize = cli->Read(Data::ByteArray(&databuff[databuffSize], 2047 - databuffSize))) > 0)
		{
			databuffSize += readSize;
		}
		databuff[databuffSize] = 0;
		readSize = Text::StrSplit(ptrs, 3, databuff, ',');
		status = Text::StrToInt32(ptrs[0]);
		if (status == 200)
		{
			sptr = buff;
			if (*ptrs[2] == '"')
			{
				i = Text::StrIndexOfChar(&ptrs[2][1], '"');
				if (i != INVALID_INDEX)
				{
					if (i < buffSize)
					{
						buff = Text::StrConcatC(buff, &ptrs[2][1], i);
					}
					else
					{
						buff = Text::StrConcatS(buff, ptrs[2], buffSize);
					}
				}
				else
				{
					buff = Text::StrConcatS(buff, ptrs[2], buffSize);
				}
			}
			else
			{
				buff = Text::StrConcatS(buff, ptrs[2], buffSize);
			}
			if (*sptr == 0)
			{
				buff = Text::StrConcatS(buff, U8STR("-"), buffSize);
			}
		}
		else if (status == 602)
		{
			this->lastIsError = 1;
			errWriter->WriteLine(CSTR("Google 602 error"));
			*buff = 0;
		}
		else
		{
			this->lastIsError = 1;
			sptr = Text::StrConcatC(url, UTF8STRC("Google "));
			sptr = Text::StrInt32(sptr, status);
			sptr = Text::StrConcatC(sptr, UTF8STRC(" Error"));
			errWriter->WriteLine(CSTRP(url, sptr));
			*buff = 0;
		}
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		this->lastIsError = 2;
		*buff = 0;
		sb.AppendC(UTF8STRC("Cannot connect: "));
		sb.AppendC(url, (UIntOS)(sptr - url));
		errWriter->WriteLine(sb.ToCString());
	}
	this->lastSrchDate.SetCurrTimeUTC();
	cli.Delete();
	mutUsage.EndUse();
	return buff;
}

UnsafeArrayOpt<UTF8Char> Map::GoogleMap::GoogleSearcher::SearchName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	if (this->lastIsError == 2)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.DiffMS(this->lastSrchDate) < 60000)
			return nullptr;
	}
	NN<Text::Locale::LocaleEntry> ent;
	if (!Text::Locale::GetLocaleEntry(lcid).SetTo(ent))
		return nullptr;
	return SearchName(buff, buffSize, pos, {ent->shortName, ent->shortNameLen});
}

UnsafeArrayOpt<UTF8Char> Map::GoogleMap::GoogleSearcher::CacheName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	if (this->lastIsError != 0)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.DiffMS(this->lastSrchDate) < 60000)
			return nullptr;
	}
	NN<Text::Locale::LocaleEntry> ent;
	if (!Text::Locale::GetLocaleEntry(lcid).SetTo(ent))
		return nullptr;
	return SearchName(buff, buffSize, pos, {ent->shortName, ent->shortNameLen});
}

UInt32 Map::GoogleMap::GoogleSearcher::GetSrchCnt()
{
	return this->srchCnt;
}
