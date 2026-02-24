#include "Stdafx.h"
#include "Map/CustomTileMap.h"

#include <stdio.h>

Map::CustomTileMap::CustomTileMap(Text::CStringNN url, Text::CString cacheDir, UIntOS minLevel, UIntOS maxLevel, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl) : Map::MercatorTileMap(cacheDir, minLevel, maxLevel, clif, ssl)
{
	this->url = Text::String::New(url);
	this->name = Text::String::New(UTF8STRC("Custom Tile Map"));
	this->concurrCnt = 2;
	this->bounds = Math::RectAreaDbl(Math::Coord2DDbl(-180, -85.051128779806592377796715521925),
		Math::Coord2DDbl(180, 85.051128779806592377796715521925));
}

Map::CustomTileMap::~CustomTileMap()
{
	this->url->Release();
	this->name->Release();
}

Text::CStringNN Map::CustomTileMap::GetName() const
{
	return this->name->ToCString();
}

Map::TileMap::TileType Map::CustomTileMap::GetTileType() const
{
	return Map::TileMap::TileType::CUSTOM;
}

Map::TileMap::TileFormat Map::CustomTileMap::GetTileFormat() const
{
	return Map::TileMap::TileFormat::PNG;
}

UIntOS Map::CustomTileMap::GetConcurrentCount() const
{
	return this->concurrCnt;
}

Bool Map::CustomTileMap::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	bounds.Set(this->bounds);
	return true;
}

UnsafeArrayOpt<UTF8Char> Map::CustomTileMap::GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UIntOS level, Math::Coord2D<Int32> tileId)
{
	UTF8Char sbuff2[16];
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> sptr;
	sptr = this->url->ConcatTo(sbuff);
	sptr2 = Text::StrInt32(sbuff2, tileId.x);
	sptr = Text::StrReplaceC(sbuff, sptr, UTF8STRC("{x}"), sbuff2, (UIntOS)(sptr2 - sbuff2));
	sptr2 = Text::StrInt32(sbuff2, tileId.y);
	sptr = Text::StrReplaceC(sbuff, sptr, UTF8STRC("{y}"), sbuff2, (UIntOS)(sptr2 - sbuff2));
	sptr2 = Text::StrUIntOS(sbuff2, level);
	sptr = Text::StrReplaceC(sbuff, sptr, UTF8STRC("{z}"), sbuff2, (UIntOS)(sptr2 - sbuff2));
	return sptr;
}

Bool Map::CustomTileMap::GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UIntOS level, Math::Coord2D<Int32> tileId)
{
	UTF8Char sbuff2[16];
	UnsafeArray<UTF8Char> sptr2;
	sb->Append(this->url);
	sptr2 = Text::StrInt32(sbuff2, tileId.x);
	sb->ReplaceStr(UTF8STRC("{x}"), sbuff2, (UIntOS)(sptr2 - sbuff2));
	sptr2 = Text::StrInt32(sbuff2, tileId.y);
	sb->ReplaceStr(UTF8STRC("{y}"), sbuff2, (UIntOS)(sptr2 - sbuff2));
	sptr2 = Text::StrUIntOS(sbuff2, level);
	sb->ReplaceStr(UTF8STRC("{z}"), sbuff2, (UIntOS)(sptr2 - sbuff2));
	return true;
}

void Map::CustomTileMap::SetBounds(Math::RectAreaDbl bounds)
{
	this->bounds = bounds;
}

void Map::CustomTileMap::SetName(Text::CStringNN name)
{
	this->name->Release();
	this->name = Text::String::New(name);
}
