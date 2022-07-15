#ifndef _SM_MAP_ASSISTEDRREVERSEGEOCODER
#define _SM_MAP_ASSISTEDRREVERSEGEOCODER
#include "Data/ArrayList.h"
#include "IO/LogTool.h"
#include "IO/Writer.h"
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
			UInt32 lcid;
			Int32 lang;
		} LangMap;

	private:
		Data::ArrayList<Map::IReverseGeocoder *> revGeos;
		Data::ArrayList<LangMap*> langMaps;
		OSInt nextCoder;
		DB::DBTool *conn;
		IO::Writer *errWriter;
		Sync::Mutex mut;
	private:
		OSInt GetLangIndex(UInt32 lcid);
	public:
		AssistedRReverseGeocoder(Text::CString dsn, Text::CString uid, Text::CString pwd, IO::LogTool *log, IO::Writer *errWriter);
		virtual ~AssistedRReverseGeocoder();

		virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		void AddReverseGeocoder(Map::IReverseGeocoder *revGeo);

		void AddLangMap(Int32 lcid, Int32 lang);
		Int32 ToLang(const UTF8Char *name);
	};
}
#endif
