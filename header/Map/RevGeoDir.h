#ifndef _SM_MAP_REVGEODIR
#define _SM_MAP_REVGEODIR
#include "Data/ArrayList.h"
#include "IO/Writer.h"
#include "Map/IReverseGeocoder.h"
#include "Map/RevGeoCfg.h"

namespace Map
{
	class RevGeoDir : public Map::IReverseGeocoder
	{
	private:
		typedef struct
		{
			Map::RevGeoCfg *cfg;
			UInt32 lcid;
		} RevGeoFile;

		Data::ArrayList<RevGeoFile*> files;
		Map::MapSearchManager mapSrchMgr;
		UInt32 defLCID;

	public:
		RevGeoDir(Text::CString cfgDir, UInt32 defLCID, IO::Writer *errWriter);
		virtual ~RevGeoDir();

		virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid);
		virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid);
	};
}
#endif
