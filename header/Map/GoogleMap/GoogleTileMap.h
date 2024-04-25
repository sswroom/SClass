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
			GoogleTileMap(Text::CString cacheDir, MapType mapType, NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl);
			virtual ~GoogleTileMap();

			virtual Text::CStringNN GetName() const;
			virtual TileType GetTileType() const;
			virtual ImageType GetImageType() const;
			virtual UOSInt GetConcurrentCount() const;
			virtual UTF8Char *GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> tileId);
			virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId);
		};
	}
}
#endif
