#ifndef _SM_MAP_ASSISTEDREVERSEGEOCODERPL
#define _SM_MAP_ASSISTEDREVERSEGEOCODERPL
#include "Data/ArrayList.h"
#include "Data/BTreeUTF8Map.h"
#include "Data/Integer32Map.h"
#include "IO/LogTool.h"
#include "IO/IWriter.h"
#include "DB/DBTool.h"
#include "Sync/Mutex.h"
#include "Map/IAssistedReverseGeocoder.h"

namespace Map
{
	class AssistedReverseGeocoderPL : public Map::IAssistedReverseGeocoder
	{
	private:
		typedef struct
		{
			Data::Integer32Map<Data::Integer32Map<const UTF8Char *>*> *addrMap;
		} IndexInfo;

		typedef struct
		{
			Int32 lcid;
			Data::Integer32Map<IndexInfo *> *indexMap;
		} LCIDInfo;
	private:
		Data::ArrayList<Map::IReverseGeocoder *> *revGeos;
		OSInt nextCoder;
		DB::DBTool *conn;
		IO::IWriter *errWriter;
		Data::BTreeUTF8Map<const UTF8Char *> *strMap;
		Data::Integer32Map<LCIDInfo *> *lcidMap;
		Sync::Mutex *mut;
	public:
		AssistedReverseGeocoderPL(DB::DBTool *db, IO::IWriter *errWriter);
		virtual ~AssistedReverseGeocoderPL();

		virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
		virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
		virtual void AddReverseGeocoder(Map::IReverseGeocoder *revGeo);
	};
};
#endif
