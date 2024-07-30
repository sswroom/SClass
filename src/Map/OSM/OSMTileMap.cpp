#include "Stdafx.h"
#include "Map/OSM/OSMTileMap.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

Optional<Text::String> Map::OSM::OSMTileMap::GetNextURL()
{
	Sync::MutexUsage mutUsage(this->urlMut);
	Optional<Text::String> thisUrl = this->urls.GetItem(this->urlNext);
	this->urlNext = (this->urlNext + 1) % this->urls.GetCount();
	return thisUrl;
}

Map::OSM::OSMTileMap::OSMTileMap(Text::CStringNN url, Text::CString cacheDir, UOSInt minLevel, UOSInt maxLevel, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl) : Map::MercatorTileMap(cacheDir, minLevel, maxLevel, clif, ssl)
{
	this->urls.Add(Text::String::New(url));
	this->urlNext = 0;
}

Map::OSM::OSMTileMap::OSMTileMap(Text::CStringNN url, IO::SPackageFile *spkg, UOSInt minLevel, UOSInt maxLevel, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl) : Map::MercatorTileMap(CSTR_NULL, minLevel, maxLevel, clif, ssl)
{
	this->urls.Add(Text::String::New(url));
	this->urlNext = 0;
	this->SetSPackageFile(spkg);
}

Map::OSM::OSMTileMap::~OSMTileMap()
{
	this->urls.FreeAll();
}

void Map::OSM::OSMTileMap::AddAlternateURL(Text::CStringNN url)
{
	this->urls.Add(Text::String::New(url));
}

Optional<Text::String> Map::OSM::OSMTileMap::GetOSMURL(UOSInt index)
{
	return this->urls.GetItem(index);
}

Text::CStringNN Map::OSM::OSMTileMap::GetName() const
{
	return CSTR("OSMTileMap");
}

Map::TileMap::TileType Map::OSM::OSMTileMap::GetTileType() const
{
	return Map::TileMap::TT_OSM;
}

Map::TileMap::ImageType Map::OSM::OSMTileMap::GetImageType() const
{
	return Map::TileMap::IT_PNG;
}

UOSInt Map::OSM::OSMTileMap::GetConcurrentCount() const
{
	return 2 * this->urls.GetCount();
}

UnsafeArrayOpt<UTF8Char> Map::OSM::OSMTileMap::GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UOSInt level, Math::Coord2D<Int32> tileId)
{
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> thisUrl;
	if (this->GetNextURL().SetTo(thisUrl))
	{
		sptr = thisUrl->ConcatTo(sbuff);
		sptr = Text::StrUOSInt(sptr, level);
		sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
		sptr = Text::StrInt32(sptr, tileId.x);
		sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
		sptr = Text::StrInt32(sptr, tileId.y);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
		return sptr;
	}
	return 0;
}

Bool Map::OSM::OSMTileMap::GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId)
{
	NN<Text::String> s;
	if (this->GetNextURL().SetTo(s))
	{
		sb->Append(s);
		sb->AppendUOSInt(level);
		sb->AppendUTF8Char('/');
		sb->AppendI32(tileId.x);
		sb->AppendUTF8Char('/');
		sb->AppendI32(tileId.y);
		sb->AppendC(UTF8STRC(".png"));
		return true;
	}
	else
	{
		return false;
	}
}
