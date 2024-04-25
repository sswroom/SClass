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
		Data::ArrayList<Map::IReverseGeocoder *> revGeos;
		UOSInt nextCoder;
		NN<DB::DBTool> conn;
		IO::Writer *errWriter;
		Sync::Mutex mut;
	public:
		AssistedReverseGeocoder(NN<DB::DBTool> db, IO::Writer *errWriter);
		virtual ~AssistedReverseGeocoder();

		virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		virtual void AddReverseGeocoder(Map::IReverseGeocoder *revGeo);
	};
}
#endif
