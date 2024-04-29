#ifndef _SM_MAP_REVGEOCFG
#define _SM_MAP_REVGEOCFG
#include "Data/ArrayListNN.h"
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
			NN<Text::String> layerName;
			Int32 searchType;
			Int32 usedCnt;
			Map::MapSearchLayer *data;
			UOSInt strIndex;
		} SearchLayer;

	private:
		Data::ArrayListNN<SearchLayer> layers[REVGEO_MAXID];

	public:
		RevGeoCfg(Text::CStringNN fileName, Map::MapSearchManager *mapSrchMgr);
		~RevGeoCfg();

		UTF8Char *GetStreetName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos);
		Bool GetStreetName(NN<Text::StringBuilderUTF8> sb, Math::Coord2DDbl pos);
	};
}
#endif
