#include "Stdafx.h"
#include "MyMemory.h"
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
#include "Sync/ThreadUtil.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/TextBinEnc/Base64Enc.h"

Map::GoogleMap::GoogleStaticMap::GoogleStaticMap(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::String *gooKey, Text::String *gooCliId, Text::String *gooPrivKey)
{
	this->sockf = sockf;
	this->ssl = ssl;
	if (gooCliId && gooPrivKey)
	{
		Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, false);
		this->gooCliId = gooCliId->Clone().Ptr();
		this->gooPrivKey = MemAlloc(UInt8, gooPrivKey->leng + 1);
		this->gooPrivKeyLeng = b64.DecodeBin(gooPrivKey->v, gooPrivKey->leng, this->gooPrivKey);
		this->gooKey = 0;
	}
	else
	{
		this->gooCliId = 0;
		this->gooPrivKey = 0;
		this->gooKey = SCOPY_STRING(gooKey);
	}
}

Map::GoogleMap::GoogleStaticMap::GoogleStaticMap(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString gooKey, Text::CString gooCliId, Text::CString gooPrivKey)
{
	this->sockf = sockf;
	this->ssl = ssl;
	if (gooCliId.leng > 0 && gooPrivKey.leng > 0)
	{
		Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::URL, false);

		this->gooCliId = Text::String::New(gooCliId).Ptr();
		this->gooPrivKey = MemAlloc(UInt8, gooPrivKey.leng + 1);
		this->gooPrivKeyLeng = b64.DecodeBin(gooPrivKey.v, gooPrivKey.leng, this->gooPrivKey);
		this->gooKey = 0;
	}
	else
	{
		this->gooCliId = 0;
		this->gooPrivKey = 0;
		this->gooKey = Text::String::NewOrNull(gooKey);
	}
}

Map::GoogleMap::GoogleStaticMap::~GoogleStaticMap()
{
	SDEL_STRING(this->gooCliId);
	if (this->gooPrivKey)
	{
		MemFree(this->gooPrivKey);
		this->gooPrivKey = 0;
	}
	SDEL_STRING(this->gooKey);
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

UOSInt Map::GoogleMap::GoogleStaticMap::GetMap(UInt8 *buff, Double lat, Double lon, UInt32 scale, Math::Size2D<UOSInt> imgSize, Text::CString lang, Int32 format, Double marker_lat, Double marker_lon)
{
	NotNullPtr<Net::HTTPClient> cli;
	UTF8Char url[512];
	UTF8Char *sptr;
	UTF8Char *urlStart;
	UOSInt retSize = 0;
	UOSInt thisSize;
	urlStart = sptr = Text::StrConcatC(url, UTF8STRC("http://maps.google.com"));
	sptr = Text::StrConcatC(sptr, UTF8STRC("/maps/api/staticmap?format="));
	if (format == 1)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("gif"));
	}
	else if (format == 2)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("jpg"));
	}
	else
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("png"));
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC("&center="));
	sptr = Text::StrDouble(sptr, lat);
	sptr = Text::StrConcatC(sptr, UTF8STRC(","));
	sptr = Text::StrDouble(sptr, lon);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&zoom="));
	sptr = Text::StrUInt32(sptr, Map::GoogleMap::GoogleStaticMap::Scale2Level(scale));
	sptr = Text::StrConcatC(sptr, UTF8STRC("&size="));
	sptr = Text::StrUOSInt(sptr, imgSize.x);
	sptr = Text::StrConcatC(sptr, UTF8STRC("x"));
	sptr = Text::StrUOSInt(sptr, imgSize.y);
	if (marker_lat != 0 || marker_lon != 0)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("&markers="));
		sptr = Text::StrDouble(sptr, marker_lat);
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		sptr = Text::StrDouble(sptr, marker_lon);
	}
	if (this->gooCliId)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("&sensor=false"));
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
		sptr = Text::StrConcatC(sptr, UTF8STRC("&sensor=false"));
		sptr = Text::StrConcatC(sptr, UTF8STRC("&key="));
		sptr = this->gooKey->ConcatTo(sptr);
	}

//	wprintf(L"%s\r\n", url);
	cli = Net::HTTPClient::CreateConnect(sockf, ssl, CSTRP(url, sptr), Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (!cli->IsError())
	{
		if (lang.leng > 0)
		{
			cli->AddHeaderC(CSTR("Accept-Language"), lang);
		}

		while ((thisSize = cli->Read(buff, 2048)) > 0)
		{
			retSize += thisSize;
			buff = &buff[thisSize];
		}
	}
	cli.Delete();
	
	return retSize;
} //lang = en-us, zh-cn, zh-tw, format = 0:png, 1:gif, 2:jpg
