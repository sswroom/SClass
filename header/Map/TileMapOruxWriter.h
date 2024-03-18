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
		NotNullPtr<DB::SQLiteFile> db;
		Optional<DB::DBTransaction> sess;
		UOSInt imgCount;
		Sync::Mutex mut;
		LevelInfo *levels;
		UOSInt minLev;
		UOSInt maxLev;
		Math::RectAreaDbl bounds;
	public:
		TileMapOruxWriter(Text::CStringNN fileName, UOSInt minLev, UOSInt maxLev, Math::RectAreaDbl bounds);
		virtual ~TileMapOruxWriter();

		virtual void BeginLevel(UOSInt level);
		virtual void AddX(Int32 x);
		virtual void AddImage(UOSInt level, Int32 x, Int32 y, Data::ByteArrayR imgData, Map::TileMap::ImageType imgType);
	};
}
#endif
