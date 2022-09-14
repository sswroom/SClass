#ifndef _SM_MAP_OSM_OSMTILEMAP
#define _SM_MAP_OSM_OSMTILEMAP
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListString.h"
#include "IO/PackageFile.h"
#include "IO/SPackageFile.h"
#include "Map/TileMap.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Sync/Mutex.h"
#include "Text/CString.h"

namespace Map
{
	namespace OSM
	{
		class OSMTileMap : public Map::TileMap
		{
		private:
			Data::ArrayListString urls;
			UOSInt urlNext;
			Sync::Mutex urlMut;

			Text::String *cacheDir;
			IO::SPackageFile *spkg;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			UOSInt maxLevel;

			UOSInt tileWidth;
			UOSInt tileHeight;
			Math::CoordinateSystem *csys;

			Text::String *GetNextURL();
		public:
			OSMTileMap(Text::CString url, Text::CString cacheDir, UOSInt maxLevel, Net::SocketFactory *sockf, Net::SSLEngine *ssl);
			OSMTileMap(Text::CString url, IO::SPackageFile *spkg, UOSInt maxLevel, Net::SocketFactory *sockf, Net::SSLEngine *ssl);
			virtual ~OSMTileMap();

			void AddAlternateURL(Text::CString url);
			Text::String *GetOSMURL(UOSInt index);
			Bool HasSPackageFile();
			Bool ImportTiles(IO::PackageFile *pkg);
			Bool OptimizeToFile(Text::CString fileName);

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
