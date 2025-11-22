#ifndef _SM_MAP_MAPSEARCH
#define _SM_MAP_MAPSEARCH
#include "Data/ArrayList.hpp"
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
		NN<Map::MapSearchLayer> mapLayer;
		Optional<Text::String> searchStr;
		UOSInt strIndex;
	} MapSearchLayerInfo;

	class MapSearch
	{
	private:
		Optional<Text::String> baseDir;
		Int32 concatType;
		UnsafeArray<NN<Data::ArrayListNN<Map::MapSearchLayerInfo>>> layersArr;

	public:
		MapSearch(Text::CStringNN fileName, NN<Map::MapSearchManager> manager);
		~MapSearch();

		UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, Math::Coord2DDbl pos);
		Int32 SearchNames(UnsafeArray<UTF8Char> buff, UnsafeArray<Text::PString> outArrs, UnsafeArray<Math::Coord2DDbl> outPos, UnsafeArray<Int32> resTypes, Math::Coord2DDbl pos);
		static UnsafeArrayOpt<UTF8Char> ConcatNames(UnsafeArray<UTF8Char> buff, UnsafeArray<Text::PString> strArrs, Int32 concatType);
		Bool IsError();
		Int32 GetConcatType();
	};
};
#endif
