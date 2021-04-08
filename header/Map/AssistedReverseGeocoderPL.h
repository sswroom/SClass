#ifndef _SM_MAP_ASSISTEDREVERSEGEOCODERPL
#define _SM_MAP_ASSISTEDREVERSEGEOCODERPL
#include "Data/ArrayList.h"
#include "Data/BTreeUTF8Map.h"
#include "Data/Int32Map.h"
#include "DB/DBTool.h"
#include "IO/LogTool.h"
#include "IO/Writer.h"
#include "Map/IAssistedReverseGeocoder.h"
#include "Sync/Mutex.h"

namespace Map
{
	class AssistedReverseGeocoderPL : public Map::IAssistedReverseGeocoder
	{
	private:
		typedef struct
		{
			Data::Int32Map<Data::Int32Map<const UTF8Char *>*> *addrMap;
		} IndexInfo;

		typedef struct
		{
			Int32 lcid;
			Data::Int32Map<IndexInfo *> *indexMap;
		} LCIDInfo;
	private:
		Data::ArrayList<Map::IReverseGeocoder *> *revGeos;
		OSInt nextCoder;
		DB::DBTool *conn;
		IO::Writer *errWriter;
		Data::BTreeUTF8Map<const UTF8Char *> *strMap;
		Data::Int32Map<LCIDInfo *> *lcidMap;
		Sync::Mutex *mut;
	public:
		AssistedReverseGeocoderPL(DB::DBTool *db, IO::Writer *errWriter);
		virtual ~AssistedReverseGeocoderPL();

		virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
		virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
		virtual void AddReverseGeocoder(Map::IReverseGeocoder *revGeo);
	};
};
#endif
