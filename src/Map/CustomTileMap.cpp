#include "Stdafx.h"
#include "Map/CustomTileMap.h"

#include <stdio.h>

Map::CustomTileMap::CustomTileMap(Text::CString url, Text::CString cacheDir, UOSInt minLevel, UOSInt maxLevel, Net::SocketFactory *sockf, Net::SSLEngine *ssl) : Map::MercatorTileMap(cacheDir, minLevel, maxLevel, sockf, ssl)
{
	this->url = Text::String::New(url);
	this->name = Text::String::New(UTF8STRC("Custom Tile Map"));
	this->concurrCnt = 2;
	this->bounds = Math::RectAreaDbl(Math::Coord2DDbl(-180, -85.051128779806592377796715521925),
		Math::Coord2DDbl(180, 85.051128779806592377796715521925));
}

Map::CustomTileMap::~CustomTileMap()
{
	SDEL_STRING(this->url);
	SDEL_STRING(this->name);
}

Text::CString Map::CustomTileMap::GetName()
{
	return this->name->ToCString();
}

Map::TileMap::TileType Map::CustomTileMap::GetTileType()
{
	return Map::TileMap::TT_CUSTOM;
}

UOSInt Map::CustomTileMap::GetConcurrentCount()
{
	return this->concurrCnt;
}

Bool Map::CustomTileMap::GetBounds(Math::RectAreaDbl *bounds)
{
	*bounds = this->bounds;
	return true;
}

UTF8Char *Map::CustomTileMap::GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> tileId)
{
	UTF8Char sbuff2[16];
	UTF8Char *sptr2;
	UTF8Char *sptr;
	sptr = this->url->ConcatTo(sbuff);
	sptr2 = Text::StrInt32(sbuff2, tileId.x);
	sptr = Text::StrReplaceC(sbuff, sptr, UTF8STRC("{x}"), sbuff2, (UOSInt)(sptr2 - sbuff2));
	sptr2 = Text::StrInt32(sbuff2, tileId.y);
	sptr = Text::StrReplaceC(sbuff, sptr, UTF8STRC("{y}"), sbuff2, (UOSInt)(sptr2 - sbuff2));
	sptr2 = Text::StrUOSInt(sbuff2, level);
	sptr = Text::StrReplaceC(sbuff, sptr, UTF8STRC("{z}"), sbuff2, (UOSInt)(sptr2 - sbuff2));
	return sptr;
}

void Map::CustomTileMap::SetBounds(Math::RectAreaDbl bounds)
{
	this->bounds = bounds;
}

void Map::CustomTileMap::SetName(Text::CString name)
{
	if (name.v)
	{
		SDEL_STRING(this->name);
		this->name = Text::String::New(name);
	}
}
