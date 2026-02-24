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
			UIntOS minLevel;
			UIntOS maxLevel;
			NN<Text::String> fmt;

			Math::Coord2DDbl min;
			Math::Coord2DDbl max;
			UIntOS tileWidth;
			UIntOS tileHeight;
			NN<Math::CoordinateSystem> csys;

		public:
			OSMLocalTileMap(NN<IO::PackageFile> pkgFile);
			OSMLocalTileMap(NN<IO::PackageFile> pkgFile, NN<Text::String> name, NN<Text::String> format, UIntOS minZoom, UIntOS maxZoom, Math::Coord2DDbl minCoord, Math::Coord2DDbl maxCoord);
			virtual ~OSMLocalTileMap();

			virtual Text::CStringNN GetName() const;
			virtual Bool IsError() const;
			virtual TileType GetTileType() const;
			virtual UIntOS GetMinLevel() const;
			virtual UIntOS GetMaxLevel() const;
			virtual Double GetLevelScale(UIntOS level) const;
			virtual UIntOS GetNearestLevel(Double scale) const;
			virtual UIntOS GetConcurrentCount() const;
			virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
			virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const;
			virtual Bool IsMercatorProj() const;
			virtual UIntOS GetTileSize() const;
			virtual TileFormat GetTileFormat() const;

			virtual UIntOS GetTileImageIDs(UIntOS level, Math::RectAreaDbl rect, NN<Data::ArrayListT<Math::Coord2D<Int32>>> ids);
			virtual Optional<Media::ImageList> LoadTileImage(UIntOS level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly);
			virtual UnsafeArrayOpt<UTF8Char> GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UIntOS level, Math::Coord2D<Int32> tileId);
			virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UIntOS level, Math::Coord2D<Int32> tileId);
			virtual Optional<IO::StreamData> LoadTileImageData(UIntOS level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<TileFormat> format);

			Bool GetTileBounds(UIntOS level, OutParam<Int32> minX, OutParam<Int32> minY, OutParam<Int32> maxX, OutParam<Int32> maxY);
		};
	}
}
#endif
