#ifndef _SM_MAP_REVGEOCFG
#define _SM_MAP_REVGEOCFG
#include "Data/ArrayList.h"
#include "Map/MapSearchManager.h"

#define REVGEO_MAXID 4

namespace Map
{
	class RevGeoCfg
	{
	public:
		typedef struct
		{
			const UTF8Char *layerName;
			Int32 searchType;
			Int32 usedCnt;
			Map::IMapSearchLayer *data;
		} SearchLayer;

	private:
		Data::ArrayList<SearchLayer*> *layers[REVGEO_MAXID];

	public:
		RevGeoCfg(const UTF8Char *fileName, Map::MapSearchManager *mapSrchMgr);
		~RevGeoCfg();

		UTF8Char *GetStreetName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon);
	};
}
#endif
