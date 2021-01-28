#ifndef _SM_MAP_MAPSEARCH
#define _SM_MAP_MAPSEARCH
#include "Data/ArrayList.h"
#include "Map/MapLayerData.h"
#include "Map/MapSearchManager.h"

#define MAPSEARCH_LAYER_TYPES 7

namespace Map
{
	typedef struct
	{
		Int32 searchType;
		Double searchDist;
		Map::IMapSearchLayer *mapLayer;
		const UTF8Char *searchStr;
	} MapSearchLayer;

	class MapSearch
	{
	private:
		const UTF8Char *baseDir;
		Int32 concatType;
		Data::ArrayList<Map::MapSearchLayer*> **layersArr;

	public:
		MapSearch(const UTF8Char *fileName, Map::MapSearchManager *manager);
		~MapSearch();

		UTF8Char *SearchName(UTF8Char *buff, Double lat, Double lon);
		Int32 SearchNames(UTF8Char *buff, UTF8Char **outArrs, Double *outPos, Int32 *resTypes, Double lat, Double lon);
		static UTF8Char *ConcatNames(UTF8Char *buff, UTF8Char **strArrs, Int32 concatType);
		Bool IsError();
		Int32 GetConcatType();
	};
};
#endif
