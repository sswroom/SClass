#ifndef _SM_MAP_OSM_OSMTILEMAP
#define _SM_MAP_OSM_OSMTILEMAP
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListStrUTF8.h"
#include "IO/PackageFile.h"
#include "IO/SPackageFile.h"
#include "Map/TileMap.h"
#include "Net/SocketFactory.h"
#include "Sync/Mutex.h"

namespace Map
{
	namespace OSM
	{
		class OSMTileMap : public Map::TileMap
		{
		private:
			Data::ArrayListStrUTF8 *urls;
			OSInt urlNext;
			Sync::Mutex *urlMut;

			const UTF8Char *cacheDir;
			IO::SPackageFile *spkg;
			Net::SocketFactory *sockf;
			UOSInt maxLevel;

			UOSInt tileWidth;
			UOSInt tileHeight;

		public:
			OSMTileMap(const UTF8Char *url, const UTF8Char *cacheDir, UOSInt maxLevel, Net::SocketFactory *sockf);
			OSMTileMap(const UTF8Char *url, IO::SPackageFile *spkg, UOSInt maxLevel, Net::SocketFactory *sockf);
			virtual ~OSMTileMap();

			void AddAlternateURL(const UTF8Char *url);
			const UTF8Char *GetOSMURL(OSInt index);
			Bool HasSPackageFile();
			Bool ImportTiles(IO::PackageFile *pkg);
			Bool OptimizeToFile(const UTF8Char *fileName);

			virtual const UTF8Char *GetName();
			virtual Bool IsError();
			virtual TileType GetTileType();
			virtual UOSInt GetLevelCount();
			virtual Double GetLevelScale(UOSInt level);
			virtual UOSInt GetNearestLevel(Double scale);
			virtual UOSInt GetConcurrentCount();
			virtual Bool GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY);
			virtual ProjectionType GetProjectionType();
			virtual UOSInt GetTileSize();

			virtual UOSInt GetImageIDs(UOSInt level, Double x1, Double y1, Double x2, Double y2, Data::ArrayList<Int64> *ids);
			virtual Media::ImageList *LoadTileImage(UOSInt level, Int64 imgId, Parser::ParserList *parsers, Double *boundsXY, Bool localOnly);
			virtual UTF8Char *GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId);
			virtual IO::IStreamData *LoadTileImageData(UOSInt level, Int64 imgId, Double *boundsXY, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it);

			static Int32 Lon2TileX(Double lon, UOSInt level);
			static Int32 Lat2TileY(Double lat, UOSInt level);
			static Int32 Lon2TileXR(Double lon, UOSInt level);
			static Int32 Lat2TileYR(Double lat, UOSInt level);
			static Double TileX2Lon(Int32 x, UOSInt level);
			static Double TileY2Lat(Int32 y, UOSInt level);
		};
	}
}
#endif
