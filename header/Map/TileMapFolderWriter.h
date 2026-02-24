#ifndef _SM_MAP_TILEMAPFOLDERWRITER
#define _SM_MAP_TILEMAPFOLDERWRITER
#include "Map/TileMap.h"
#include "Map/TileMapWriter.h"

namespace Map
{
	class TileMapFolderWriter : public Map::TileMapWriter
	{
	private:
		NN<Text::String> folderName;
		Map::TileMap::TileFormat tileFormat;
		UIntOS minLev;
		UIntOS maxLev;
		Math::RectAreaDbl bounds;
		NN<Text::String> name;
	public:
		TileMapFolderWriter(Text::CStringNN folderName, Map::TileMap::TileFormat format, UIntOS minLev, UIntOS maxLev, Math::RectAreaDbl bounds);
		virtual ~TileMapFolderWriter();

		virtual void BeginLevel(UIntOS level);
		virtual void AddX(Int32 x);
		virtual void AddImage(UIntOS level, Int32 x, Int32 y, Data::ByteArrayR imgData, Map::TileMap::TileFormat format);
	};
}
#endif
