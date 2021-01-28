#ifndef _SM_MAP_GOOGLEMAP_GOOGLETILEMAP
#define _SM_MAP_GOOGLEMAP_GOOGLETILEMAP
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListStrUTF8.h"
#include "IO/PackageFile.h"
#include "IO/SPackageFile.h"
#include "Map/TileMap.h"
#include "Net/SocketFactory.h"
#include "Sync/Mutex.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleTileMap : public Map::TileMap
		{
		public:
			typedef enum
			{
				MT_HYBRID = 'y',
				MT_SATELITE = 's',
				MT_TRAIN = 't',
				MT_MAP = 'm'
			} MapType;

		private:
			const UTF8Char *cacheDir;
			IO::SPackageFile *spkg;
			Net::SocketFactory *sockf;
			UOSInt tileWidth;
			UOSInt tileHeight;
			UOSInt maxLevel;
			MapType mapType;

		public:
			GoogleTileMap(const UTF8Char *cacheDir, MapType mapType, Net::SocketFactory *sockf);
			virtual ~GoogleTileMap();

			Bool HasSPackageFile();
			Bool ImportTiles(IO::PackageFile *pkg);
			Bool OptimizeToFile(const UTF8Char *fileName);

			virtual const UTF8Char *GetName();
			virtual Bool IsError();
			virtual TileType GetTileType();
			virtual UOSInt GetLevelCount();
			virtual Double GetLevelScale(OSInt level);
			virtual UOSInt GetNearestLevel(Double scale);
			virtual UOSInt GetConcurrentCount();
			virtual void GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY);
			virtual ProjectionType GetProjectionType();
			virtual UOSInt GetTileSize();

			virtual UOSInt GetImageIDs(UOSInt level, Double x1, Double y1, Double x2, Double y2, Data::ArrayList<Int64> *ids);
			virtual Media::ImageList *LoadTileImage(UOSInt level, Int64 imgId, Parser::ParserList *parsers, Double *boundsXY, Bool localOnly);
			virtual UTF8Char *GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId);
			virtual IO::IStreamData *LoadTileImageData(UOSInt level, Int64 imgId, Double *boundsXY, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it);
		};
	}
}
#endif
