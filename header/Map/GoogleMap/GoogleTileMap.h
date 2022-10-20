#ifndef _SM_MAP_GOOGLEMAP_GOOGLETILEMAP
#define _SM_MAP_GOOGLEMAP_GOOGLETILEMAP
#include "Map/MercatorTileMap.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleTileMap : public Map::MercatorTileMap
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
			MapType mapType;

		public:
			GoogleTileMap(Text::CString cacheDir, MapType mapType, Net::SocketFactory *sockf, Net::SSLEngine *ssl);
			virtual ~GoogleTileMap();

			virtual Text::CString GetName();
			virtual TileType GetTileType();
			virtual UOSInt GetConcurrentCount();
			virtual UTF8Char *GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId);
		};
	}
}
#endif
