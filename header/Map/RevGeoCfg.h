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
			Text::String *layerName;
			Int32 searchType;
			Int32 usedCnt;
			Map::IMapSearchLayer *data;
		} SearchLayer;

	private:
		Data::ArrayList<SearchLayer*> *layers[REVGEO_MAXID];

	public:
		RevGeoCfg(Text::CString fileName, Map::MapSearchManager *mapSrchMgr);
		~RevGeoCfg();

		UTF8Char *GetStreetName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon);
	};
}
#endif
