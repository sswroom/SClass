#ifndef _SM_MAP_TILEMAPORUXWRITER
#define _SM_MAP_TILEMAPORUXWRITER
#include "DB/SQLiteFile.h"
#include "Map/TileMapWriter.h"
#include "Sync/Mutex.h"

namespace Map
{
	class TileMapOruxWriter : public Map::TileMapWriter
	{
	private:
		struct LevelInfo
		{
			Int32 minX;
			Int32 minY;
		};
	private:
		NN<DB::SQLiteFile> db;
		Optional<DB::DBTransaction> sess;
		UIntOS imgCount;
		Sync::Mutex mut;
		LevelInfo *levels;
		UIntOS minLev;
		UIntOS maxLev;
		Math::RectAreaDbl bounds;
	public:
		TileMapOruxWriter(Text::CStringNN fileName, UIntOS minLev, UIntOS maxLev, Math::RectAreaDbl bounds);
		virtual ~TileMapOruxWriter();

		virtual void BeginLevel(UIntOS level);
		virtual void AddX(Int32 x);
		virtual void AddImage(UIntOS level, Int32 x, Int32 y, Data::ByteArrayR imgData, Map::TileMap::TileFormat format);
	};
}
#endif
