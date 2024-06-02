#ifndef _SM_MAP_ASSISTEDREVERSEGEOCODER
#define _SM_MAP_ASSISTEDREVERSEGEOCODER
#include "Data/ArrayList.h"
#include "DB/DBTool.h"
#include "IO/LogTool.h"
#include "IO/Writer.h"
#include "Map/IAssistedReverseGeocoder.h"
#include "Sync/Mutex.h"

namespace Map
{
	class AssistedReverseGeocoder : public Map::IAssistedReverseGeocoder
	{
	private:
		Data::ArrayListNN<Map::IReverseGeocoder> revGeos;
		UOSInt nextCoder;
		NN<DB::DBTool> conn;
		NN<IO::Writer> errWriter;
		Sync::Mutex mut;
	public:
		AssistedReverseGeocoder(NN<DB::DBTool> db, NN<IO::Writer> errWriter);
		virtual ~AssistedReverseGeocoder();

		virtual UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		virtual UnsafeArrayOpt<UTF8Char> CacheName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		virtual void AddReverseGeocoder(NN<Map::IReverseGeocoder> revGeo);
	};
}
#endif
