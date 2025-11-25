#ifndef _SM_MAP_DBASSISTEDREVERSEGEOCODER
#define _SM_MAP_DBASSISTEDREVERSEGEOCODER
#include "Data/ArrayList.hpp"
#include "DB/DBTool.h"
#include "IO/LogTool.h"
#include "IO/Writer.h"
#include "Map/AssistedReverseGeocoder.h"
#include "Sync/Mutex.h"

namespace Map
{
	class DBAssistedReverseGeocoder : public Map::AssistedReverseGeocoder
	{
	private:
		Data::ArrayListNN<Map::ReverseGeocoder> revGeos;
		UOSInt nextCoder;
		NN<DB::DBTool> conn;
		NN<IO::Writer> errWriter;
		Sync::Mutex mut;
	public:
		DBAssistedReverseGeocoder(NN<DB::DBTool> db, NN<IO::Writer> errWriter);
		virtual ~DBAssistedReverseGeocoder();

		virtual UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		virtual UnsafeArrayOpt<UTF8Char> CacheName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		virtual void AddReverseGeocoder(NN<Map::ReverseGeocoder> revGeo);
	};
}
#endif
