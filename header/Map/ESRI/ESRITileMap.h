#ifndef _SM_MAP_ESRI_ESRITILEMAP
#define _SM_MAP_ESRI_ESRITILEMAP
#include "Net/SocketFactory.h"
#include "Data/ArrayListDbl.h"
#include "Map/TileMap.h"

namespace Map
{
	namespace ESRI
	{
		class ESRITileMap : public Map::TileMap
		{
		private:
			const UTF8Char *url;
			const UTF8Char *cacheDir;
			Net::SocketFactory *sockf;
			Double oriX;
			Double oriY;
			Double minX;
			Double minY;
			Double maxX;
			Double maxY;
			Bool isMercatorProj;

			UOSInt tileWidth;
			UOSInt tileHeight;
			Data::ArrayListDbl *levels;

		public:
			ESRITileMap(const UTF8Char *url, const UTF8Char *cacheDir, Net::SocketFactory *sockf);
			virtual ~ESRITileMap();

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
			static Double TileX2Lon(Int32 x, UOSInt level);
			static Double TileY2Lat(Int32 y, UOSInt level);
			static Double WebMercatorX2Lon(Double x);
			static Double WebMercatorY2Lat(Double y);
			static Double Lon2WebMercatorX(Double lon);
			static Double Lat2WebMercatorY(Double lat);
		};
	};
};
#endif
