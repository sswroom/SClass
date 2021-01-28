#ifndef _SM_MAP_ASSISTEDRREVERSEGEOCODER
#define _SM_MAP_ASSISTEDRREVERSEGEOCODER
#include "Data/ArrayList.h"
#include "IO/LogTool.h"
#include "IO/IWriter.h"
#include "DB/DBTool.h"
#include "Sync/Mutex.h"
#include "Map/IReverseGeocoder.h"

namespace Map
{
	class AssistedRReverseGeocoder : public Map::IReverseGeocoder
	{
	private:
		typedef struct
		{
			Int32 lcid;
			Int32 lang;
		} LangMap;

	private:
		Data::ArrayList<Map::IReverseGeocoder *> *revGeos;
		Data::ArrayList<LangMap*> *langMaps;
		OSInt nextCoder;
		DB::DBTool *conn;
		IO::IWriter *errWriter;
		Sync::Mutex *mut;
	private:
		OSInt GetLangIndex(Int32 lcid);
	public:
		AssistedRReverseGeocoder(const WChar *dsn, const WChar *uid, const WChar *pwd, IO::LogTool *log, IO::IWriter *errWriter);
		virtual ~AssistedRReverseGeocoder();

		virtual WChar *SearchName(WChar *buff, Double lat, Double lon, Int32 lcid);
		virtual WChar *CacheName(WChar *buff, Double lat, Double lon, Int32 lcid);
		void AddReverseGeocoder(Map::IReverseGeocoder *revGeo);

		void AddLangMap(Int32 lcid, Int32 lang);
		Int32 ToLang(const WChar *name);
	};
};
#endif
