#include "Stdafx.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Map/BingMapsTile.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/StaticImage.h"
#include "Net/HTTPClient.h"
#include "Parser/FileParser/PNGParser.h"
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
	this->brandLogoImg = 0;
	this->hideLogo = false;
	this->dispDPI = 96;
	this->dispSize = Math::Size2D<Double>(640, 480);

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

	if (this->brandLogoUri)
	{
		IO::MemoryStream mstm(UTF8STRC("Map.BingMapsTile.mstm"));
		if (Net::HTTPClient::LoadContent(sockf, ssl, this->brandLogoUri->ToCString(), &mstm, 1048576))
		{
			Parser::FileParser::PNGParser parser;
			IO::StmData::MemoryDataRef fd(mstm.GetBuff(), mstm.GetLength());
			IO::ParsedObject *pobj = parser.ParseFile(&fd, 0, IO::ParserType::ImageList);
			if (pobj)
			{
				NEW_CLASS(this->brandLogoImg, Media::SharedImage((Media::ImageList*)pobj, false));
			}
		}
	}
}

Map::BingMapsTile::~BingMapsTile()
{
	SDEL_STRING(this->url);
	SDEL_STRING(this->key);
	SDEL_STRING(this->brandLogoUri);
	SDEL_CLASS(this->brandLogoImg);
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
	return 2 * this->subdomains.GetCount();
}

void Map::BingMapsTile::SetDispSize(Math::Size2D<Double> size, Double dpi)
{
	this->dispSize = size;
	this->dispDPI = dpi;
}

UTF8Char *Map::BingMapsTile::GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> tileId)
{
	Text::String *subdomain = this->GetNextSubdomain();
	UTF8Char *sptr = this->url->ConcatTo(sbuff);
	UTF8Char sbuff2[32];
	UTF8Char *sptr2;
	sptr = Text::StrReplaceC(sbuff, sptr, UTF8STRC("{subdomain}"), subdomain->v, subdomain->leng);
	sptr2 = GenQuadkey(sbuff2, level, tileId.x, tileId.y);
	sptr = Text::StrReplaceC(sbuff, sptr, UTF8STRC("{quadkey}"), sbuff2, (UOSInt)(sptr2 - sbuff2));
	return sptr;
}

UOSInt Map::BingMapsTile::GetScreenObjCnt()
{
	if (this->brandLogoImg && !this->hideLogo)
		return 1;
	return 0;
}

Math::Geometry::Vector2D *Map::BingMapsTile::CreateScreenObjVector(UOSInt index)
{
	if (index == 0 && this->brandLogoImg && !this->hideLogo)
	{
		Math::Coord2DDbl size96 = this->dispSize.ToCoord() * (96.0 / this->dispDPI);
		Media::StaticImage *img = this->brandLogoImg->GetImage(0);
		Math::Coord2DDbl imgSize = Math::Coord2DDbl(UOSInt2Double(img->info.dispWidth), UOSInt2Double(img->info.dispHeight));
		Math::Coord2DDbl pos = size96 - 16 - imgSize;
		return Math::Geometry::VectorImage::CreateScreenImage(0, this->brandLogoImg, pos / size96, imgSize / size96, this->brandLogoUri->ToCString());
	}
	return 0;
}

UTF8Char *Map::BingMapsTile::GetScreenObjURL(UTF8Char *sbuff, UOSInt index)
{
	if (index == 0 && this->brandLogoImg && !this->hideLogo)
	{
		return this->brandLogoUri->ConcatTo(sbuff);
	}
	return 0;
}

void Map::BingMapsTile::SetHideLogo(Bool hideLogo)
{
	this->hideLogo = hideLogo;
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