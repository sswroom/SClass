#ifndef _SM_MAP_MAPSEARCH
#define _SM_MAP_MAPSEARCH
#include "Data/ArrayList.h"
#include "Map/MapLayerData.h"
#include "Map/MapSearchManager.h"
#include "Text/CString.h"
#include "Text/PString.h"
#include "Text/String.h"

#define MAPSEARCH_LAYER_TYPES 7

namespace Map
{
	typedef struct
	{
		Int32 searchType;
		Double searchDist;
		Map::IMapSearchLayer *mapLayer;
		Text::String *searchStr;
		UOSInt strIndex;
	} MapSearchLayer;

	class MapSearch
	{
	private:
		Text::String *baseDir;
		Int32 concatType;
		Data::ArrayList<Map::MapSearchLayer*> **layersArr;

	public:
		MapSearch(Text::CString fileName, Map::MapSearchManager *manager);
		~MapSearch();

		UTF8Char *SearchName(UTF8Char *buff, Math::Coord2DDbl pos);
		Int32 SearchNames(UTF8Char *buff, Text::PString *outArrs, Math::Coord2DDbl *outPos, Int32 *resTypes, Math::Coord2DDbl pos);
		static UTF8Char *ConcatNames(UTF8Char *buff, Text::PString *strArrs, Int32 concatType);
		Bool IsError();
		Int32 GetConcatType();
	};
};
#endif
