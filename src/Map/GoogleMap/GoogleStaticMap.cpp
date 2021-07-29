#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA1.h"
#include "Data/DateTime.h"
#include "Data/ArrayList.h"
#include "IO/Stream.h"
#include "Map/GoogleMap/GoogleStaticMap.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
#include "Net/HTTPClient.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Map::GoogleMap::GoogleStaticMap::GoogleStaticMap(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *gooKey, const UTF8Char *gooCliId, const UTF8Char *gooPrivKey)
{
	this->sockf = sockf;
	this->ssl = ssl;
	if (gooCliId && gooPrivKey)
	{
		Crypto::Encrypt::Base64 b64;
		const UTF8Char *tmpKeyStr;
		UOSInt tmpKeyStrLeng;

		this->gooCliId = Text::StrCopyNew(gooCliId);
		tmpKeyStr = Text::StrCopyNew(gooPrivKey);
		tmpKeyStrLeng = Text::StrCharCnt(tmpKeyStr);
		this->gooPrivKey = MemAlloc(UInt8, tmpKeyStrLeng + 1);
		Text::StrReplace((Char*)tmpKeyStr, '-', '+');
		Text::StrReplace((Char*)tmpKeyStr, '_', '/');
		this->gooPrivKeyLeng = b64.Decrypt(tmpKeyStr, tmpKeyStrLeng, this->gooPrivKey, 0);
		Text::StrDelNew(tmpKeyStr);
		
		this->gooKey = 0;
	}
	else
	{
		this->gooCliId = 0;
		this->gooPrivKey = 0;
		this->gooKey = Text::StrCopyNew(gooKey);
	}
	NEW_CLASS(this->lastSrchDate, Data::DateTime());
	NEW_CLASS(this->mut, Sync::Mutex());
	this->lastSrchDate->SetCurrTimeUTC();
}

Map::GoogleMap::GoogleStaticMap::~GoogleStaticMap()
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

UInt32 Map::GoogleMap::GoogleStaticMap::Level2Scale(UInt32 level)
{
	UInt32 levels[] = {100000000,100000000,100000000,100000000,49827656,24913828,12456914,6228457,3114229,1557114,778557,389279,194639,97320,48660,24330,12165,6082,3041,1521};
	return levels[level];
}

UInt32 Map::GoogleMap::GoogleStaticMap::Scale2Level(UInt32 scale)
{
	if (scale > 70466947)
	{
		return 3;
	}
	else if (scale > 35233473)
	{
		return 4;
	}
	else if (scale > 17616737)
	{
		return 5;
	}
	else if (scale > 8808368)
	{
		return 6;
	}
	else if (scale > 4404184)
	{
		return 7;
	}
	else if (scale > 2202092)
	{
		return 8;
	}
	else if (scale > 1101046)
	{
		return 9;
	}
	else if (scale > 550523)
	{
		return 10;
	}
	else if (scale > 275262)
	{
		return 11;
	}
	else if (scale > 137631)
	{
		return 12;
	}
	else if (scale > 68815)
	{
		return 13;
	}
	else if (scale > 34408)
	{
		return 14;
	}
	else if (scale > 17204)
	{
		return 15;
	}
	else if (scale > 8602)
	{
		return 16;
	}
	else if (scale > 4301)
	{
		return 17;
	}
	else if (scale > 2150)
	{
		return 18;
	}
	else
	{
		return 19;
	}
}

UOSInt Map::GoogleMap::GoogleStaticMap::GetMap(UInt8 *buff, Double lat, Double lon, UInt32 scale, UInt32 width, UInt32 height, const UTF8Char *lang, Int32 format, Double marker_lat, Double marker_lon)
{
	Net::HTTPClient *cli;
	UTF8Char url[512];
	UTF8Char *sptr;
	UTF8Char *urlStart;
	UOSInt retSize = 0;
	UOSInt thisSize;
	urlStart = sptr = Text::StrConcat(url, (const UTF8Char*)"http://maps.google.com");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"/maps/api/staticmap?format=");
	if (format == 1)
	{
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"gif");
	}
	else if (format == 2)
	{
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"jpg");
	}
	else
	{
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"png");
	}
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"&center=");
	sptr = Text::StrDouble(sptr, lat);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
	sptr = Text::StrDouble(sptr, lon);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"&zoom=");
	sptr = Text::StrUInt32(sptr, Map::GoogleMap::GoogleStaticMap::Scale2Level(scale));
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"&size=");
	sptr = Text::StrUInt32(sptr, width);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"x");
	sptr = Text::StrUInt32(sptr, height);
	if (marker_lat != 0 || marker_lon != 0)
	{
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"&markers=");
		sptr = Text::StrDouble(sptr, marker_lat);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
		sptr = Text::StrDouble(sptr, marker_lon);
	}
	if (this->gooCliId)
	{
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"&sensor=false");
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"&client=");
		sptr = Text::StrConcat(sptr, this->gooCliId);

		UInt8 result[20];
		UInt8 result2[40];
		UOSInt size = (UOSInt)(Text::StrConcatC(buff, urlStart, (UOSInt)(sptr - urlStart)) - buff);
		Crypto::Hash::SHA1 sha;
		Crypto::Hash::HMAC hmac(&sha, this->gooPrivKey, this->gooPrivKeyLeng);
		hmac.Calc(buff, size);
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
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"&sensor=false");
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"&key=");
		sptr = Text::StrConcat(sptr, this->gooKey);
	}

//	wprintf(L"%s\r\n", url);
	cli = Net::HTTPClient::CreateConnect(sockf, ssl, url, 0, true);
	if (!cli->IsError())
	{
		if (lang)
		{
			cli->AddHeader((const UTF8Char*)"Accept-Language", lang);
		}

		while ((thisSize = cli->Read(buff, 2048)) > 0)
		{
			retSize += thisSize;
			buff = &buff[thisSize];
		}
	}
	DEL_CLASS(cli);
	
	return retSize;
} //lang = en-us, zh-cn, zh-tw, format = 0:png, 1:gif, 2:jpg
