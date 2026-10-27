#ifndef _SM_MAP_TILEMAPMBTILESWRITER
#define _SM_MAP_TILEMAPMBTILESWRITER
#include "DB/SQLiteFile.h"
#include "Map/TileMapWriter.h"
#include "Sync/Mutex.h"

namespace Map
{
	class TileMapMbtilesWriter : public Map::TileMapWriter
	{
	private:
		NN<DB::SQLiteFile> db;
		Optional<DB::DBTransaction> sess;
		Sync::Mutex mut;
		UIntOS imgCount;

		void AddMetadata(Text::CStringNN key, Text::CStringNN value);
	public:
		TileMapMbtilesWriter(Text::CStringNN fileName, Map::TileMap::TileFormat format, UIntOS minLev, UIntOS maxLev, Math::RectAreaDbl bounds);
		virtual ~TileMapMbtilesWriter();

		virtual void BeginLevel(UIntOS level);
		virtual void AddX(Int32 x);
		virtual void AddImage(UIntOS level, Int32 x, Int32 y, Data::ByteArrayR imgData, Map::TileMap::TileFormat format);
	};
}
#endif
