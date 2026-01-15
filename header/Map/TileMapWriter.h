#ifndef _SM_MAP_TILEMAPWRITER
#define _SM_MAP_TILEMAPWRITER
#include "Map/TileMap.h"

namespace Map
{
	class TileMapWriter
	{
	public:
		virtual ~TileMapWriter(){};

		virtual void BeginLevel(UIntOS level) = 0;
		virtual void AddX(Int32 x) = 0;
		virtual void AddImage(UIntOS level, Int32 x, Int32 y, Data::ByteArrayR imgData, Map::TileMap::ImageType imgType) = 0;
	};
}
#endif
