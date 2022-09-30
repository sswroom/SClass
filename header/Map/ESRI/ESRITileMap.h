#ifndef _SM_MAP_ESRI_ESRITILEMAP
#define _SM_MAP_ESRI_ESRITILEMAP
#include "Data/ArrayListDbl.h"
#include "Map/TileMap.h"
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
			Text::String *url;
			Text::String *cacheDir;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Math::Coord2DDbl ori;
			Math::Coord2DDbl min;
			Math::Coord2DDbl max;
			Math::CoordinateSystem *csys;

			UOSInt tileWidth;
			UOSInt tileHeight;
			Data::ArrayListDbl levels;

		public:
			ESRITileMap(Text::String *url, Text::CString cacheDir, Net::SocketFactory *sockf, Net::SSLEngine *ssl);
			virtual ~ESRITileMap();

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
		};
	}
}
#endif
