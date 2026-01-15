#ifndef _SM_MAP_TILEMAPSPKWRITER
#define _SM_MAP_TILEMAPSPKWRITER
#include "IO/SPackageFile.h"
#include "Map/TileMapWriter.h"
#include "Sync/Mutex.h"

namespace Map
{
	class TileMapSPKWriter : public Map::TileMapWriter
	{
	private:
		IO::SPackageFile spkg;
	public:
		TileMapSPKWriter(Text::CStringNN fileName);
		virtual ~TileMapSPKWriter();

		virtual void BeginLevel(UIntOS level);
		virtual void AddX(Int32 x);
		virtual void AddImage(UIntOS level, Int32 x, Int32 y, Data::ByteArrayR imgData, Map::TileMap::ImageType imgType);
	};
}
#endif
