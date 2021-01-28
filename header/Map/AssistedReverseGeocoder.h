#ifndef _SM_MAP_ASSISTEDREVERSEGEOCODER
#define _SM_MAP_ASSISTEDREVERSEGEOCODER
#include "Data/ArrayList.h"
#include "IO/LogTool.h"
#include "IO/IWriter.h"
#include "DB/DBTool.h"
#include "Sync/Mutex.h"
#include "Map/IAssistedReverseGeocoder.h"

namespace Map
{
	class AssistedReverseGeocoder : public Map::IAssistedReverseGeocoder
	{
	private:
		Data::ArrayList<Map::IReverseGeocoder *> *revGeos;
		OSInt nextCoder;
		DB::DBTool *conn;
		IO::IWriter *errWriter;
		Sync::Mutex *mut;
	public:
		AssistedReverseGeocoder(DB::DBTool *db, IO::IWriter *errWriter);
		virtual ~AssistedReverseGeocoder();

		virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
		virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
		virtual void AddReverseGeocoder(Map::IReverseGeocoder *revGeo);
	};
}
#endif
