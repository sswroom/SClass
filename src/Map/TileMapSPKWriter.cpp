#include "Stdafx.h"
#include "IO/Path.h"
#include "Map/TileMapSPKWriter.h"

Map::TileMapSPKWriter::TileMapSPKWriter(Text::CStringNN fileName) : spkg(fileName)
{
}

Map::TileMapSPKWriter::~TileMapSPKWriter()
{
}

void Map::TileMapSPKWriter::BeginLevel(UIntOS level)
{	
}

void Map::TileMapSPKWriter::AddX(Int32 x)
{
}

void Map::TileMapSPKWriter::AddImage(UIntOS level, Int32 x, Int32 y, Data::ByteArrayR imgData, Map::TileMap::TileFormat format)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUIntOS(sbuff, level);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, x);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, y);
	switch (format)
	{
	default:
	case Map::TileMap::TileFormat::PNG:
		sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
		break;
	case Map::TileMap::TileFormat::WEBP:
		sptr = Text::StrConcatC(sptr, UTF8STRC(".webp"));
		break;
	case Map::TileMap::TileFormat::JPG:
		sptr = Text::StrConcatC(sptr, UTF8STRC(".jpg"));
		break;
	}
	this->spkg.AddFile(imgData.Arr(), imgData.GetSize(), CSTRP(sbuff, sptr), Data::Timestamp::UtcNow());
}
