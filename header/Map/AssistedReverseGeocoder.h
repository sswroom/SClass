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
		Data::ArrayList<Map::IReverseGeocoder *> *revGeos;
		OSInt nextCoder;
		DB::DBTool *conn;
		IO::Writer *errWriter;
		Sync::Mutex *mut;
	public:
		AssistedReverseGeocoder(DB::DBTool *db, IO::Writer *errWriter);
		virtual ~AssistedReverseGeocoder();

		virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
		virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
		virtual void AddReverseGeocoder(Map::IReverseGeocoder *revGeo);
	};
}
#endif
