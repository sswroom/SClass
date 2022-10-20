#include "Stdafx.h"
#include "Map/CustomTileMap.h"

Map::CustomTileMap::CustomTileMap(Text::CString url, Text::CString cacheDir, UOSInt minLevel, UOSInt maxLevel, Net::SocketFactory *sockf, Net::SSLEngine *ssl) : Map::MercatorTileMap(cacheDir, minLevel, maxLevel, sockf, ssl)
{
	this->url = Text::String::New(url);
	this->name = Text::String::New(UTF8STRC("Custom Tile Map"));
	this->concurrCnt = 2;
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

UTF8Char *Map::CustomTileMap::GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId)
{
	Int32 imgX = (Int32)(imgId >> 32);
	Int32 imgY = (Int32)(imgId & 0xffffffffLL);
	UTF8Char sbuff2[16];
	UTF8Char *sptr2;
	UTF8Char *sptr;
	sptr = this->url->ConcatTo(sbuff);
	sptr2 = Text::StrInt32(sbuff2, imgX);
	sptr = Text::StrReplaceC(sbuff, sptr, sbuff2, (UOSInt)(sptr2 - sbuff2), UTF8STRC("{x}"));
	sptr2 = Text::StrInt32(sbuff2, imgY);
	sptr = Text::StrReplaceC(sbuff, sptr, sbuff2, (UOSInt)(sptr2 - sbuff2), UTF8STRC("{y}"));
	sptr2 = Text::StrUOSInt(sbuff2, level);
	sptr = Text::StrReplaceC(sbuff, sptr, sbuff2, (UOSInt)(sptr2 - sbuff2), UTF8STRC("{z}"));
	return sptr;

}
