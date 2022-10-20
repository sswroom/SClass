#include "Stdafx.h"
#include "Map/GoogleMap/GoogleTileMap.h"
#include "Text/MyString.h"

#define GMAPURL "http://mt1.google.com/vt/"

Map::GoogleMap::GoogleTileMap::GoogleTileMap(Text::CString cacheDir, MapType mapType, Net::SocketFactory *sockf, Net::SSLEngine *ssl) : Map::MercatorTileMap(cacheDir, 0, 18, sockf, ssl)
{
	this->mapType = mapType;
}

Map::GoogleMap::GoogleTileMap::~GoogleTileMap()
{
}

Text::CString Map::GoogleMap::GoogleTileMap::GetName()
{
	return CSTR("GoogleTileMap");
}

Map::TileMap::TileType Map::GoogleMap::GoogleTileMap::GetTileType()
{
	return Map::TileMap::TT_GOOGLE;
}

UOSInt Map::GoogleMap::GoogleTileMap::GetConcurrentCount()
{
	return 2;
}

UTF8Char *Map::GoogleMap::GoogleTileMap::GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId)
{
	Int32 imgX = (Int32)(imgId >> 32);
	Int32 imgY = (Int32)(imgId & 0xffffffffLL);
	UTF8Char *sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC(GMAPURL));
	sptr = Text::StrConcatC(sptr, UTF8STRC("lyrs="));
	*sptr++ = (UTF8Char)this->mapType;
	sptr = Text::StrConcatC(sptr, UTF8STRC("&x="));
	sptr = Text::StrInt32(sptr, imgX);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&y="));
	sptr = Text::StrInt32(sptr, imgY);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&z="));
	sptr = Text::StrUOSInt(sptr, level);
	return sptr;
}
