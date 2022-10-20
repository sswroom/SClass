#include "Stdafx.h"
#include "Map/OSM/OSMTileMap.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

Text::String *Map::OSM::OSMTileMap::GetNextURL()
{
	Sync::MutexUsage mutUsage(&this->urlMut);
	Text::String *thisUrl = this->urls.GetItem(this->urlNext);
	this->urlNext = (this->urlNext + 1) % this->urls.GetCount();
	return thisUrl;
}

Map::OSM::OSMTileMap::OSMTileMap(Text::CString url, Text::CString cacheDir, UOSInt maxLevel, Net::SocketFactory *sockf, Net::SSLEngine *ssl) : Map::MercatorTileMap(cacheDir, 0, maxLevel, sockf, ssl)
{
	this->urls.Add(Text::String::New(url.v, url.leng));
	this->urlNext = 0;
}

Map::OSM::OSMTileMap::OSMTileMap(Text::CString url, IO::SPackageFile *spkg, UOSInt maxLevel, Net::SocketFactory *sockf, Net::SSLEngine *ssl) : Map::MercatorTileMap(CSTR_NULL, 0, maxLevel, sockf, ssl)
{
	this->urls.Add(Text::String::New(url.v, url.leng));
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

void Map::OSM::OSMTileMap::AddAlternateURL(Text::CString url)
{
	this->urls.Add(Text::String::New(url.v, url.leng));
}

Text::String *Map::OSM::OSMTileMap::GetOSMURL(UOSInt index)
{
	return this->urls.GetItem(index);
}

Text::CString Map::OSM::OSMTileMap::GetName()
{
	return CSTR("OSMTileMap");
}

Map::TileMap::TileType Map::OSM::OSMTileMap::GetTileType()
{
	return Map::TileMap::TT_OSM;
}

UOSInt Map::OSM::OSMTileMap::GetConcurrentCount()
{
	return 2 * this->urls.GetCount();
}

UTF8Char *Map::OSM::OSMTileMap::GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId)
{
	Int32 imgX = (Int32)(imgId >> 32);
	Int32 imgY = (Int32)(imgId & 0xffffffffLL);
	UTF8Char *sptr;
	Text::String *thisUrl = this->GetNextURL();
	sptr = thisUrl->ConcatTo(sbuff);
	sptr = Text::StrUOSInt(sptr, level);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
	sptr = Text::StrInt32(sptr, imgX);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
	sptr = Text::StrInt32(sptr, imgY);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
	return sptr;
}
