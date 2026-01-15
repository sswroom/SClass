#ifndef _SM_MAP_TILEMAPZIPWRITER
#define _SM_MAP_TILEMAPZIPWRITER
#include "Data/ArrayListInt32.h"
#include "IO/FileStream.h"
#include "IO/ZIPBuilder.h"
#include "Map/TileMap.h"
#include "Map/TileMapWriter.h"

namespace Map
{
	class TileMapZipWriter : public Map::TileMapWriter
	{
	private:
		IO::FileStream fs;
		IO::ZIPBuilder zip;
		Map::TileMap::ImageType imgType;
		UIntOS minLev;
		UIntOS maxLev;
		Math::RectAreaDbl bounds;
		Data::ArrayListInt32 xList;
		NN<Text::String> name;
		UIntOS currLev;
	public:
		TileMapZipWriter(Text::CStringNN fileName, Map::TileMap::ImageType imgType, UIntOS minLev, UIntOS maxLev, Math::RectAreaDbl bounds);
		virtual ~TileMapZipWriter();

		virtual void BeginLevel(UIntOS level);
		virtual void AddX(Int32 x);
		virtual void AddImage(UIntOS level, Int32 x, Int32 y, Data::ByteArrayR imgData, Map::TileMap::ImageType imgType);
	};
}
#endif
