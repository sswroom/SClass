#include "Stdafx.h"
#include "IO/Path.h"
#include "Map/BingMapsTile.h"
#include "Net/HTTPClient.h"
#include "Sync/MutexUsage.h"
#include "Text/JSON.h"

#include <stdio.h>

Text::String *Map::BingMapsTile::GetNextSubdomain()
{
	Sync::MutexUsage mutUsage(&this->urlMut);
	Text::String *thisUrl = this->subdomains.GetItem(this->urlNext);
	this->urlNext = (this->urlNext + 1) % this->subdomains.GetCount();
	return thisUrl;
}

Map::BingMapsTile::BingMapsTile(ImagerySet is, Text::CString key, Text::CString cacheDir, Net::SocketFactory *sockf, Net::SSLEngine *ssl) : Map::MercatorTileMap(cacheDir, 1, 19, sockf, ssl)
{
	this->url = 0;
	this->key = Text::String::NewOrNull(key);
	this->urlNext = 0;
	this->brandLogoUri = 0;

	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	sb.AppendC(UTF8STRC("http://dev.virtualearth.net/REST/V1/Imagery/Metadata/"));
	sb.Append(ImagerySetGetName(is));
	sb.AppendC(UTF8STRC("?output=json&include=ImageryProviders&key="));
	sb.Append(key);
	if (!Net::HTTPClient::LoadContent(sockf, ssl, sb.ToCString(), &sb2, 1048576))
	{
		return;
	}
	Text::JSONBase *json = Text::JSONBase::ParseJSONStr(sb2.ToCString());
	if (json)
	{
		Text::String *s = json->GetString(UTF8STRC("brandLogoUri"));
		this->brandLogoUri = SCOPY_STRING(s);
		Text::JSONBase *resourceBase = json->GetValue(UTF8STRC("resourceSets[0].resources[0]"));
		if (resourceBase && resourceBase->GetType() == Text::JSONType::Object)
		{
			s = resourceBase->GetString(UTF8STRC("imageUrl"));
			this->url = SCOPY_STRING(s);
			Text::JSONBase *subdObj = resourceBase->GetValue(UTF8STRC("imageUrlSubdomains"));
			if (subdObj && subdObj->GetType() == Text::JSONType::Array)
			{
				Text::JSONArray *subd = (Text::JSONArray*)subdObj;
				UOSInt i = 0;
				UOSInt j = subd->GetArrayLength();
				while (i < j)
				{
					s = subd->GetArrayString(i);
					if (s)
					{
						this->subdomains.Add(s->Clone());
					}
					i++;
				}
			}
		}
		json->EndUse();
	}
}

Map::BingMapsTile::~BingMapsTile()
{
	SDEL_STRING(this->url);
	SDEL_STRING(this->key);
	SDEL_STRING(this->brandLogoUri);
	LIST_FREE_STRING(&this->subdomains);
}

Bool Map::BingMapsTile::IsError()
{
	return this->url == 0 || this->key == 0 || this->subdomains.GetCount() == 0;
}

Text::CString Map::BingMapsTile::GetName()
{
	return CSTR("Bing Maps");
}

Map::TileMap::TileType Map::BingMapsTile::GetTileType()
{
	return TT_BING;
}

UOSInt Map::BingMapsTile::GetConcurrentCount()
{
	return 2;
}

UTF8Char *Map::BingMapsTile::GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId)
{
	Text::String *subdomain = this->GetNextSubdomain();
	UTF8Char *sptr = this->url->ConcatTo(sbuff);
	UTF8Char sbuff2[32];
	UTF8Char *sptr2;
	Int32 imgX = (Int32)(imgId >> 32);
	Int32 imgY = (Int32)(imgId & 0xffffffffLL);
	sptr = Text::StrReplaceC(sbuff, sptr, UTF8STRC("{subdomain}"), subdomain->v, subdomain->leng);
	sptr2 = GenQuadkey(sbuff2, level, imgX, imgY);
	sptr = Text::StrReplaceC(sbuff, sptr, UTF8STRC("{quadkey}"), sbuff2, (UOSInt)(sptr2 - sbuff2));
	return sptr;
}

void Map::BingMapsTile::GetDefaultCacheDir(ImagerySet is, Text::StringBuilderUTF8 *sb)
{
	IO::Path::GetProcessFileName(sb);
	IO::Path::AppendPath(sb, UTF8STRC("bingmaps"));
	sb->AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	sb->Append(ImagerySetGetName(is));
}

Text::CString Map::BingMapsTile::ImagerySetGetName(ImagerySet is)
{
	switch (is)
	{
	case ImagerySet::Aerial:
		return CSTR("Aerial");
	case ImagerySet::AerialWithLabelsOnDemand:
		return CSTR("AerialWithLabelsOnDemand");
	case ImagerySet::Birdseye:
		return CSTR("Birdseye");
	case ImagerySet::BirdseyeWithLabels:
		return CSTR("BirdseyeWithLabels");
	case ImagerySet::BirdseyeV2:
		return CSTR("BirdseyeV2");
	case ImagerySet::BirdseyeV2WithLabels:
		return CSTR("BirdseyeV2WithLabels");
	case ImagerySet::CanvasDark:
		return CSTR("CanvasDark");
	case ImagerySet::CanvasLight:
		return CSTR("CanvasLight");
	case ImagerySet::CanvasGray:
		return CSTR("CanvasGray");
	case ImagerySet::OrdnanceSurvey:
		return CSTR("OrdnanceSurvey");
	case ImagerySet::RoadOnDemand:
		return CSTR("RoadOnDemand");
	case ImagerySet::Streetside:
		return CSTR("Streetside");
	default:
		return CSTR("unknown");
	}
}

UTF8Char *Map::BingMapsTile::GenQuadkey(UTF8Char *sbuff, UOSInt level, Int32 imgX, Int32 imgY)
{
	while (level-- > 0)
	{
		UTF8Char c = '0';
		Int32 mask = 1 << level;
		if (imgX & mask)
		{
			c |= 1;
		}
		if (imgY & mask)
		{
			c |= 2;
		}
		*sbuff++ = c;
	}
	*sbuff = 0;
	return sbuff;
}
