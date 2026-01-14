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
			NN<Text::String> cacheDir;
			NN<Map::ESRI::ESRIMapServer> esriMap;
			Bool toRelease;
			Math::RectAreaDbl dispBounds;
			Math::Size2DDbl dispSize;
			Double dispDPI;

		public:
			ESRITileMap(NN<Map::ESRI::ESRIMapServer> esriMap, Bool toRelease, Text::CStringNN cacheDir);
			virtual ~ESRITileMap();

			virtual Text::CStringNN GetName() const;
			virtual Bool IsError() const;
			virtual TileType GetTileType() const;
			virtual UOSInt GetMinLevel() const;
			virtual UOSInt GetMaxLevel() const;
			virtual Double GetLevelScale(UOSInt level) const;
			virtual UOSInt GetNearestLevel(Double scale) const;
			virtual UOSInt GetConcurrentCount() const;
			virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
			virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const;
			virtual Bool IsMercatorProj() const;
			virtual UOSInt GetTileSize() const;
			virtual ImageType GetImageType() const;
			virtual Bool CanQuery() const;
			virtual Bool QueryInfos(Math::Coord2DDbl coord, UOSInt level, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList) const;
			virtual void SetDispSize(Math::Size2DDbl size, Double dpi);

			virtual UOSInt GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, NN<Data::ArrayListT<Math::Coord2D<Int32>>> ids);
			virtual Optional<Media::ImageList> LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly);
			virtual UnsafeArrayOpt<UTF8Char> GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UOSInt level, Math::Coord2D<Int32> tileId);
			virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId);
			virtual Optional<IO::StreamData> LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it);
		};
	}
}
#endif
