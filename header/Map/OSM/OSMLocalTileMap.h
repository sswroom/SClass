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
			//const WChar *tileDir;
			IO::PackageFile *pkgFile;
			UOSInt maxLevel;

			Math::Coord2DDbl min;
			Math::Coord2DDbl max;
			UOSInt tileWidth;
			UOSInt tileHeight;
			Math::CoordinateSystem *csys;

		public:
			OSMLocalTileMap(IO::PackageFile *pkgFile); //const WChar *tileDir);
			virtual ~OSMLocalTileMap();

			virtual Text::CString GetName();
			virtual Bool IsError();
			virtual TileType GetTileType();
			virtual UOSInt GetLevelCount();
			virtual Double GetLevelScale(UOSInt level);
			virtual UOSInt GetNearestLevel(Double scale);
			virtual UOSInt GetConcurrentCount();
			virtual Bool GetBounds(Math::RectAreaDbl *bounds);
			virtual Math::CoordinateSystem *GetCoordinateSystem();
			virtual Bool IsMercatorProj();
			virtual UOSInt GetTileSize();

			virtual UOSInt GetImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Int64> *ids);
			virtual Media::ImageList *LoadTileImage(UOSInt level, Int64 imgId, Parser::ParserList *parsers, Math::RectAreaDbl *bounds, Bool localOnly);
			virtual UTF8Char *GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId);
			virtual IO::IStreamData *LoadTileImageData(UOSInt level, Int64 imgId, Math::RectAreaDbl *bounds, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it);

			Bool GetTileBounds(UOSInt level, Int32 *minX, Int32 *minY, Int32 *maxX, Int32 *maxY);
		};
	}
}
#endif
