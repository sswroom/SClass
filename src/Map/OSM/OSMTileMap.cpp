#include "Stdafx.h"
#include "Map/OSM/OSMTileMap.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

Text::String *Map::OSM::OSMTileMap::GetNextURL()
{
	Sync::MutexUsage mutUsage(this->urlMut);
	Text::String *thisUrl = this->urls.GetItem(this->urlNext);
	this->urlNext = (this->urlNext + 1) % this->urls.GetCount();
	return thisUrl;
}

Map::OSM::OSMTileMap::OSMTileMap(Text::CStringNN url, Text::CString cacheDir, UOSInt maxLevel, NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl) : Map::MercatorTileMap(cacheDir, 0, maxLevel, sockf, ssl)
{
	this->urls.Add(Text::String::New(url));
	this->urlNext = 0;
}

Map::OSM::OSMTileMap::OSMTileMap(Text::CStringNN url, IO::SPackageFile *spkg, UOSInt maxLevel, NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl) : Map::MercatorTileMap(CSTR_NULL, 0, maxLevel, sockf, ssl)
{
	this->urls.Add(Text::String::New(url));
	this->urlNext = 0;
	this->SetSPackageFile(spkg);
}

Map::OSM::OSMTileMap::~OSMTileMap()
{
	UOSInt i;
	i = this->urls.GetCount();
	while (i-- > 0)
	{
		this->urls.GetItem(i)->Release();
	}
}

void Map::OSM::OSMTileMap::AddAlternateURL(Text::CStringNN url)
{
	this->urls.Add(Text::String::New(url));
}

Text::String *Map::OSM::OSMTileMap::GetOSMURL(UOSInt index)
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

UOSInt Map::OSM::OSMTileMap::GetConcurrentCount() const
{
	return 2 * this->urls.GetCount();
}

UTF8Char *Map::OSM::OSMTileMap::GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> tileId)
{
	UTF8Char *sptr;
	Text::String *thisUrl = this->GetNextURL();
	sptr = thisUrl->ConcatTo(sbuff);
	sptr = Text::StrUOSInt(sptr, level);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
	sptr = Text::StrInt32(sptr, tileId.x);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
	sptr = Text::StrInt32(sptr, tileId.y);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
	return sptr;
}
