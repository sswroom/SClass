#include "Stdafx.h"
#include "Map/OSM/OSMTemplateTileMap.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

Optional<Text::String> Map::OSM::OSMTemplateTileMap::GetNextURL()
{
	Sync::MutexUsage mutUsage(this->urlMut);
	Optional<Text::String> thisUrl = this->urls.GetItem(this->urlNext);
	this->urlNext = (this->urlNext + 1) % this->urls.GetCount();
	return thisUrl;
}

Map::OSM::OSMTemplateTileMap::OSMTemplateTileMap(Text::CStringNN url, Text::CStringNN cacheDir, UIntOS minLevel, UIntOS maxLevel, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl) : Map::MercatorTileMap(cacheDir, minLevel, maxLevel, clif, ssl)
{
	this->urls.Add(Text::String::New(url));
	this->urlNext = 0;
}

Map::OSM::OSMTemplateTileMap::~OSMTemplateTileMap()
{
	this->urls.FreeAll();
}

void Map::OSM::OSMTemplateTileMap::AddAlternateURL(Text::CStringNN url)
{
	this->urls.Add(Text::String::New(url));
}

Optional<Text::String> Map::OSM::OSMTemplateTileMap::GetOSMURL(UIntOS index)
{
	return this->urls.GetItem(index);
}

Text::CStringNN Map::OSM::OSMTemplateTileMap::GetName() const
{
	return CSTR("OSMTemplateTileMap");
}

Map::TileMap::TileType Map::OSM::OSMTemplateTileMap::GetTileType() const
{
	return Map::TileMap::TileType::OSMTemplate;
}

Map::TileMap::TileFormat Map::OSM::OSMTemplateTileMap::GetTileFormat() const
{
	return Map::TileMap::TileFormat::PNG;
}

UIntOS Map::OSM::OSMTemplateTileMap::GetConcurrentCount() const
{
	return 2 * this->urls.GetCount();
}

UnsafeArrayOpt<UTF8Char> Map::OSM::OSMTemplateTileMap::GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UIntOS level, Math::Coord2D<Int32> tileId)
{
	UTF8Char tmpBuff[64];
	UnsafeArray<UTF8Char> tmpPtr;
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> thisUrl;
	if (this->GetNextURL().SetTo(thisUrl))
	{
		sptr = thisUrl->ConcatTo(sbuff);
		tmpPtr = Text::StrUIntOS(tmpBuff, level);
		sptr = Text::StrReplaceC(sbuff, sptr, UTF8STRC("{z}"), tmpBuff, (UIntOS)(tmpPtr - tmpBuff));
		tmpPtr = Text::StrIntOS(tmpBuff, tileId.x);
		sptr = Text::StrReplaceC(sbuff, sptr, UTF8STRC("{x}"), tmpBuff, (UIntOS)(tmpPtr - tmpBuff));
		tmpPtr = Text::StrIntOS(tmpBuff, tileId.y);
		sptr = Text::StrReplaceC(sbuff, sptr, UTF8STRC("{y}"), tmpBuff, (UIntOS)(tmpPtr - tmpBuff));
		return sptr;
	}
	return nullptr;
}

Bool Map::OSM::OSMTemplateTileMap::GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UIntOS level, Math::Coord2D<Int32> tileId)
{
	UTF8Char tmpBuff[64];
	UnsafeArray<UTF8Char> tmpPtr;
	NN<Text::String> s;
	if (this->GetNextURL().SetTo(s))
	{
		sb->Append(s);
		tmpPtr = Text::StrUIntOS(tmpBuff, level);
		sb->ReplaceStr(CSTR("{z}"), CSTRP(tmpBuff, tmpPtr));
		tmpPtr = Text::StrIntOS(tmpBuff, tileId.x);
		sb->ReplaceStr(CSTR("{x}"), CSTRP(tmpBuff, tmpPtr));
		tmpPtr = Text::StrIntOS(tmpBuff, tileId.y);
		sb->ReplaceStr(CSTR("{y}"), CSTRP(tmpBuff, tmpPtr));
		return true;
	}
	else
	{
		return false;
	}
}
