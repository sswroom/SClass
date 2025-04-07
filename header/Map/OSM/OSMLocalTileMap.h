#ifndef _SM_MAP_OSM_OSMLOCALTILEMAP
#define _SM_MAP_OSM_OSMLOCALTILEMAP
#include "Data/ArrayListDbl.h"
#include "IO/PackageFile.h"
#include "Map/TileMap.h"

namespace Map
{
	namespace OSM
	{
		class OSMLocalTileMap : public Map::TileMap
		{
		private:
			NN<IO::PackageFile> pkgFile;
			Optional<IO::PackageFile> rootPkg;
			Text::String *name;
			UOSInt minLevel;
			UOSInt maxLevel;
			NN<Text::String> fmt;

			Math::Coord2DDbl min;
			Math::Coord2DDbl max;
			UOSInt tileWidth;
			UOSInt tileHeight;
			NN<Math::CoordinateSystem> csys;

		public:
			OSMLocalTileMap(NN<IO::PackageFile> pkgFile);
			OSMLocalTileMap(NN<IO::PackageFile> pkgFile, NN<Text::String> name, NN<Text::String> format, UOSInt minZoom, UOSInt maxZoom, Math::Coord2DDbl minCoord, Math::Coord2DDbl maxCoord);
			virtual ~OSMLocalTileMap();

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

			virtual UOSInt GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, NN<Data::ArrayList<Math::Coord2D<Int32>>> ids);
			virtual Optional<Media::ImageList> LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly);
			virtual UnsafeArrayOpt<UTF8Char> GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UOSInt level, Math::Coord2D<Int32> tileId);
			virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId);
			virtual Optional<IO::StreamData> LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it);

			Bool GetTileBounds(UOSInt level, OutParam<Int32> minX, OutParam<Int32> minY, OutParam<Int32> maxX, OutParam<Int32> maxY);
		};
	}
}
#endif
