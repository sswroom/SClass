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
			NotNullPtr<IO::PackageFile> pkgFile;
			Optional<IO::PackageFile> rootPkg;
			Text::String *name;
			UOSInt minLevel;
			UOSInt maxLevel;
			NotNullPtr<Text::String> fmt;

			Math::Coord2DDbl min;
			Math::Coord2DDbl max;
			UOSInt tileWidth;
			UOSInt tileHeight;
			NotNullPtr<Math::CoordinateSystem> csys;

		public:
			OSMLocalTileMap(NotNullPtr<IO::PackageFile> pkgFile);
			OSMLocalTileMap(NotNullPtr<IO::PackageFile> pkgFile, NotNullPtr<Text::String> name, NotNullPtr<Text::String> format, UOSInt minZoom, UOSInt maxZoom, Math::Coord2DDbl minCoord, Math::Coord2DDbl maxCoord);
			virtual ~OSMLocalTileMap();

			virtual Text::CStringNN GetName() const;
			virtual Bool IsError() const;
			virtual TileType GetTileType() const;
			virtual UOSInt GetLevelCount() const;
			virtual Double GetLevelScale(UOSInt level) const;
			virtual UOSInt GetNearestLevel(Double scale) const;
			virtual UOSInt GetConcurrentCount() const;
			virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
			virtual NotNullPtr<Math::CoordinateSystem> GetCoordinateSystem() const;
			virtual Bool IsMercatorProj() const;
			virtual UOSInt GetTileSize() const;

			virtual UOSInt GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Math::Coord2D<Int32>> *ids);
			virtual Media::ImageList *LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, Parser::ParserList *parsers, Math::RectAreaDbl *bounds, Bool localOnly);
			virtual UTF8Char *GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> tileId);
			virtual IO::StreamData *LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, Math::RectAreaDbl *bounds, Bool localOnly, ImageType *it);

			Bool GetTileBounds(UOSInt level, Int32 *minX, Int32 *minY, Int32 *maxX, Int32 *maxY);
		};
	}
}
#endif
