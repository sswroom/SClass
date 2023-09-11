#ifndef _SM_MAP_ESRI_ESRITILEMAP
#define _SM_MAP_ESRI_ESRITILEMAP
#include "Data/ArrayListDbl.h"
#include "Map/TileMap.h"
#include "Map/ESRI/ESRIMapServer.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/String.h"

namespace Map
{
	namespace ESRI
	{
		class ESRITileMap : public Map::TileMap
		{
		private:
			NotNullPtr<Text::String> cacheDir;
			Map::ESRI::ESRIMapServer *esriMap;
			Bool toRelease;
			Math::RectAreaDbl dispBounds;
			Math::Size2DDbl dispSize;
			Double dispDPI;

		public:
			ESRITileMap(Map::ESRI::ESRIMapServer *esriMap, Bool toRelease, Text::CString cacheDir);
			virtual ~ESRITileMap();

			virtual Text::CStringNN GetName() const;
			virtual Bool IsError() const;
			virtual TileType GetTileType() const;
			virtual UOSInt GetLevelCount() const;
			virtual Double GetLevelScale(UOSInt level) const;
			virtual UOSInt GetNearestLevel(Double scale) const;
			virtual UOSInt GetConcurrentCount() const;
			virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
			virtual Math::CoordinateSystem *GetCoordinateSystem();
			virtual Bool IsMercatorProj() const;
			virtual UOSInt GetTileSize() const;
			virtual Bool CanQuery() const;
			virtual Bool QueryInfos(Math::Coord2DDbl coord, UOSInt level, Data::ArrayList<Math::Geometry::Vector2D*> *vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListNN<Text::String> *nameList, Data::ArrayList<Text::String*> *valueList) const;
			virtual void SetDispSize(Math::Size2DDbl size, Double dpi);

			virtual UOSInt GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Math::Coord2D<Int32>> *ids);
			virtual Media::ImageList *LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, Parser::ParserList *parsers, Math::RectAreaDbl *bounds, Bool localOnly);
			virtual UTF8Char *GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> tileId);
			virtual IO::StreamData *LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, Math::RectAreaDbl *bounds, Bool localOnly, ImageType *it);
		};
	}
}
#endif
