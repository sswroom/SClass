#ifndef _SM_MAP_REVGEODIR
#define _SM_MAP_REVGEODIR
#include "Data/ArrayListNN.hpp"
#include "IO/Writer.h"
#include "Map/ReverseGeocoder.h"
#include "Map/RevGeoCfg.h"

namespace Map
{
	class RevGeoDir : public Map::ReverseGeocoder
	{
	private:
		typedef struct
		{
			Map::RevGeoCfg *cfg;
			UInt32 lcid;
		} RevGeoFile;

		Data::ArrayListNN<RevGeoFile> files;
		Map::MapSearchManager mapSrchMgr;
		UInt32 defLCID;

	public:
		RevGeoDir(Text::CStringNN cfgDir, UInt32 defLCID, NN<IO::Writer> errWriter);
		virtual ~RevGeoDir();

		virtual UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		virtual UnsafeArrayOpt<UTF8Char> CacheName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos, UInt32 lcid);
	};
}
#endif
