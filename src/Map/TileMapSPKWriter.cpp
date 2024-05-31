#include "Stdafx.h"
#include "IO/Path.h"
#include "Map/TileMapSPKWriter.h"

Map::TileMapSPKWriter::TileMapSPKWriter(Text::CStringNN fileName) : spkg(fileName)
{
}

Map::TileMapSPKWriter::~TileMapSPKWriter()
{
}

void Map::TileMapSPKWriter::BeginLevel(UOSInt level)
{	
}

void Map::TileMapSPKWriter::AddX(Int32 x)
{
}

void Map::TileMapSPKWriter::AddImage(UOSInt level, Int32 x, Int32 y, Data::ByteArrayR imgData, Map::TileMap::ImageType imgType)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUOSInt(sbuff, level);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, x);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, y);
	switch (imgType)
	{
	default:
	case Map::TileMap::IT_PNG:
		sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
		break;
	case Map::TileMap::IT_WEBP:
		sptr = Text::StrConcatC(sptr, UTF8STRC(".webp"));
		break;
	case Map::TileMap::IT_JPG:
		sptr = Text::StrConcatC(sptr, UTF8STRC(".jpg"));
		break;
	}
	this->spkg.AddFile(imgData.Arr(), imgData.GetSize(), CSTRP(sbuff, sptr), Data::Timestamp::UtcNow());
}
