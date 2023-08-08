#ifndef _SM_MAP_REVGEOCFG
#define _SM_MAP_REVGEOCFG
#include "Data/ArrayList.h"
#include "Map/MapSearchManager.h"
#include "Text/String.h"

#define REVGEO_MAXID 4

namespace Map
{
	class RevGeoCfg
	{
	public:
		typedef struct
		{
			NotNullPtr<Text::String> layerName;
			Int32 searchType;
			Int32 usedCnt;
			Map::MapSearchLayer *data;
			UOSInt strIndex;
		} SearchLayer;

	private:
		Data::ArrayList<SearchLayer*> layers[REVGEO_MAXID];

	public:
		RevGeoCfg(Text::CStringNN fileName, Map::MapSearchManager *mapSrchMgr);
		~RevGeoCfg();

		UTF8Char *GetStreetName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos);
	};
}
#endif
