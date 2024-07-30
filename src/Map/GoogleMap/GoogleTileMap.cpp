#include "Stdafx.h"
#include "Map/GoogleMap/GoogleTileMap.h"
#include "Text/MyString.h"

#define GMAPURL "http://mt1.google.com/vt/"

Map::GoogleMap::GoogleTileMap::GoogleTileMap(Text::CString cacheDir, MapType mapType, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl) : Map::MercatorTileMap(cacheDir, 0, 18, clif, ssl)
{
	this->mapType = mapType;
}

Map::GoogleMap::GoogleTileMap::~GoogleTileMap()
{
}

Text::CStringNN Map::GoogleMap::GoogleTileMap::GetName() const
{
	return CSTR("GoogleTileMap");
}

Map::TileMap::TileType Map::GoogleMap::GoogleTileMap::GetTileType() const
{
	return Map::TileMap::TT_GOOGLE;
}

Map::TileMap::ImageType Map::GoogleMap::GoogleTileMap::GetImageType() const
{
	return IT_PNG;
}

UOSInt Map::GoogleMap::GoogleTileMap::GetConcurrentCount() const
{
	return 2;
}

UnsafeArrayOpt<UTF8Char> Map::GoogleMap::GoogleTileMap::GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UOSInt level, Math::Coord2D<Int32> tileId)
{
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC(GMAPURL));
	sptr = Text::StrConcatC(sptr, UTF8STRC("lyrs="));
	*sptr++ = (UTF8Char)this->mapType;
	sptr = Text::StrConcatC(sptr, UTF8STRC("&x="));
	sptr = Text::StrInt32(sptr, tileId.x);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&y="));
	sptr = Text::StrInt32(sptr, tileId.y);
	sptr = Text::StrConcatC(sptr, UTF8STRC("&z="));
	sptr = Text::StrUOSInt(sptr, level);
	return sptr;
}

Bool Map::GoogleMap::GoogleTileMap::GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId)
{
	sb->AppendC(UTF8STRC(GMAPURL));
	sb->AppendC(UTF8STRC("lyrs="));
	sb->AppendUTF8Char((UTF8Char)this->mapType);
	sb->AppendC(UTF8STRC("&x="));
	sb->AppendI32(tileId.x);
	sb->AppendC(UTF8STRC("&y="));
	sb->AppendI32(tileId.y);
	sb->AppendC(UTF8STRC("&z="));
	sb->AppendUOSInt(level);
	return true;
}
