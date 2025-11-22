#ifndef _SM_MAP_DBASSISTEDRREVERSEGEOCODER
#define _SM_MAP_DBASSISTEDRREVERSEGEOCODER
#include "Data/ArrayList.hpp"
#include "IO/LogTool.h"
#include "IO/Writer.h"
#include "DB/DBTool.h"
#include "Sync/Mutex.h"
#include "Map/ReverseGeocoder.h"

namespace Map
{
	class DBAssistedRReverseGeocoder : public Map::ReverseGeocoder
	{
	private:
		typedef struct
		{
			UInt32 lcid;
			Int32 lang;
		} LangMap;

	private:
		Data::ArrayListNN<Map::ReverseGeocoder> revGeos;
		Data::ArrayListNN<LangMap> langMaps;
		OSInt nextCoder;
		Optional<DB::DBTool> conn;
		Optional<IO::Writer> errWriter;
		Sync::Mutex mut;
	private:
		OSInt GetLangIndex(UInt32 lcid);
	public:
		DBAssistedRReverseGeocoder(Text::CStringNN dsn, Text::CString uid, Text::CString pwd, NN<IO::LogTool> log, Optional<IO::Writer> errWriter);
		virtual ~DBAssistedRReverseGeocoder();

		virtual UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		virtual UnsafeArrayOpt<UTF8Char> CacheName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		void AddReverseGeocoder(NN<Map::ReverseGeocoder> revGeo);

		void AddLangMap(Int32 lcid, Int32 lang);
		Int32 ToLang(const UTF8Char *name);
	};
}
#endif
