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

Optional<Text::String> Map::BingMapsTile::GetNextSubdomain()
{
	Sync::MutexUsage mutUsage(this->urlMut);
	Optional<Text::String> thisUrl = this->subdomains.GetItem(this->urlNext);
	this->urlNext = (this->urlNext + 1) % this->subdomains.GetCount();
	return thisUrl;
}

Map::BingMapsTile::BingMapsTile(ImagerySet is, Text::CString key, Text::CString cacheDir, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl) : Map::MercatorTileMap(cacheDir, 1, 19, clif, ssl)
{
	this->url = nullptr;
	this->key = Text::String::NewOrNull(key);
	this->urlNext = 0;
	this->brandLogoUri = nullptr;
	this->brandLogoImg = nullptr;
	this->hideLogo = false;
	this->dispDPI = 96;
	this->dispSize = Math::Size2DDbl(640, 480);

	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	sb.AppendC(UTF8STRC("http://dev.virtualearth.net/REST/V1/Imagery/Metadata/"));
	sb.Append(ImagerySetGetName(is));
	sb.AppendC(UTF8STRC("?output=json&include=ImageryProviders&key="));
	sb.AppendOpt(key);
	if (!Net::HTTPClient::LoadContent(clif, ssl, sb.ToCString(), sb2, 1048576))
	{
		return;
	}
	NN<Text::JSONBase> json;
	NN<Text::String> s;
	if (Text::JSONBase::ParseJSONStr(sb2.ToCString()).SetTo(json))
	{
		this->brandLogoUri = Text::String::CopyOrNull(json->GetValueString(CSTR("brandLogoUri")));
		NN<Text::JSONBase> resourceBase;
		if (json->GetValue(CSTR("resourceSets[0].resources[0]")).SetTo(resourceBase) && resourceBase->GetType() == Text::JSONType::Object)
		{
			this->url = Text::String::CopyOrNull(resourceBase->GetValueString(CSTR("imageUrl")));
			NN<Text::JSONBase> subdObj;
			if (resourceBase->GetValue(CSTR("imageUrlSubdomains")).SetTo(subdObj) && subdObj->GetType() == Text::JSONType::Array)
			{
				NN<Text::JSONArray> subd = NN<Text::JSONArray>::ConvertFrom(subdObj);
				UIntOS i = 0;
				UIntOS j = subd->GetArrayLength();
				while (i < j)
				{
					if (subd->GetArrayString(i).SetTo(s))
					{
						this->subdomains.Add(s->Clone());
					}
					i++;
				}
			}
		}
		json->EndUse();
	}

	if (this->brandLogoUri.SetTo(s))
	{
		IO::MemoryStream mstm;
		if (Net::HTTPClient::LoadContent(clif, ssl, s->ToCString(), mstm, 1048576))
		{
			Parser::FileParser::PNGParser parser;
			IO::StmData::MemoryDataRef fd(mstm.GetBuff(), (UIntOS)mstm.GetLength());
			NN<IO::ParsedObject> pobj;
			if (parser.ParseFile(fd, nullptr, IO::ParserType::ImageList).SetTo(pobj))
			{
				NEW_CLASSOPT(this->brandLogoImg, Media::SharedImage(NN<Media::ImageList>::ConvertFrom(pobj), nullptr));
			}
		}
	}
}

Map::BingMapsTile::~BingMapsTile()
{
	OPTSTR_DEL(this->url);
	OPTSTR_DEL(this->key);
	OPTSTR_DEL(this->brandLogoUri);
	this->brandLogoImg.Delete();
	this->subdomains.FreeAll();
}

Bool Map::BingMapsTile::IsError() const
{
	return this->url.IsNull() || this->key.IsNull() || this->subdomains.GetCount() == 0;
}

Text::CStringNN Map::BingMapsTile::GetName() const
{
	return CSTR("Bing Maps");
}

Map::TileMap::TileType Map::BingMapsTile::GetTileType() const
{
	return TT_BING;
}

Map::TileMap::ImageType Map::BingMapsTile::GetImageType() const
{
	return IT_PNG;
}

UIntOS Map::BingMapsTile::GetConcurrentCount() const
{
	return 2 * this->subdomains.GetCount();
}

void Map::BingMapsTile::SetDispSize(Math::Size2DDbl size, Double dpi)
{
	this->dispSize = size;
	this->dispDPI = dpi;
}

UnsafeArrayOpt<UTF8Char> Map::BingMapsTile::GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UIntOS level, Math::Coord2D<Int32> tileId)
{
	NN<Text::String> subdomain;
	UnsafeArray<UTF8Char> sptr = Text::String::OrEmpty(this->url)->ConcatTo(sbuff);
	UTF8Char sbuff2[32];
	UnsafeArray<UTF8Char> sptr2;
	if (this->GetNextSubdomain().SetTo(subdomain))
	{
		sptr = Text::StrReplaceC(sbuff, sptr, UTF8STRC("{subdomain}"), subdomain->v, subdomain->leng);
	}
	sptr2 = GenQuadkey(sbuff2, level, tileId.x, tileId.y);
	sptr = Text::StrReplaceC(sbuff, sptr, UTF8STRC("{quadkey}"), sbuff2, (UIntOS)(sptr2 - sbuff2));
	return sptr;
}

Bool Map::BingMapsTile::GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UIntOS level, Math::Coord2D<Int32> imgId)
{
	NN<Text::String> subdomain;
	sb->AppendOpt(this->url);
	UTF8Char sbuff2[32];
	UnsafeArray<UTF8Char> sptr2;
	if (this->GetNextSubdomain().SetTo(subdomain))
		sb->ReplaceStr(UTF8STRC("{subdomain}"), subdomain->v, subdomain->leng);
	sptr2 = GenQuadkey(sbuff2, level, imgId.x, imgId.y);
	sb->ReplaceStr(UTF8STRC("{quadkey}"), sbuff2, (UIntOS)(sptr2 - sbuff2));
	return true;
}

UIntOS Map::BingMapsTile::GetScreenObjCnt()
{
	if (this->brandLogoImg.NotNull() && !this->hideLogo)
		return 1;
	return 0;
}

Optional<Math::Geometry::Vector2D> Map::BingMapsTile::CreateScreenObjVector(UIntOS index)
{
	NN<Media::SharedImage> brandLogoImg;
	if (index == 0 && this->brandLogoImg.SetTo(brandLogoImg) && !this->hideLogo)
	{
		Math::Coord2DDbl size96 = this->dispSize * (96.0 / this->dispDPI);
		NN<Media::StaticImage> img;
		if (brandLogoImg->GetImage(0).SetTo(img))
		{
			Math::Coord2DDbl imgSize = img->info.dispSize.ToDouble();
			Math::Coord2DDbl pos = size96 - 16 - imgSize;
			return Math::Geometry::VectorImage::CreateScreenImage(0, brandLogoImg, pos / size96, imgSize / size96, OPTSTR_CSTR(this->brandLogoUri));
		}
	}
	return nullptr;
}

UnsafeArrayOpt<UTF8Char> Map::BingMapsTile::GetScreenObjURL(UnsafeArray<UTF8Char> sbuff, UIntOS index)
{
	if (index == 0 && this->brandLogoImg.NotNull() && !this->hideLogo)
	{
		return Text::String::OrEmpty(this->brandLogoUri)->ConcatTo(sbuff);
	}
	return nullptr;
}

Bool Map::BingMapsTile::GetScreenObjURL(NN<Text::StringBuilderUTF8> sb, UIntOS index)
{
	if (index == 0 && this->brandLogoImg.NotNull() && !this->hideLogo)
	{
		sb->AppendOpt(this->brandLogoUri);
		return true;
	}
	return false;
}

void Map::BingMapsTile::SetHideLogo(Bool hideLogo)
{
	this->hideLogo = hideLogo;
}

void Map::BingMapsTile::GetDefaultCacheDir(ImagerySet is, NN<Text::StringBuilderUTF8> sb)
{
	IO::Path::GetProcessFileName(sb);
	IO::Path::AppendPath(sb, UTF8STRC("bingmaps"));
	sb->AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	sb->Append(ImagerySetGetName(is));
}

Text::CStringNN Map::BingMapsTile::ImagerySetGetName(ImagerySet is)
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

UnsafeArray<UTF8Char> Map::BingMapsTile::GenQuadkey(UnsafeArray<UTF8Char> sbuff, UIntOS level, Int32 imgX, Int32 imgY)
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
