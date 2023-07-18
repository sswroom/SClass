#ifndef _SM_MAP_OSM_OSMTILEMAP
#define _SM_MAP_OSM_OSMTILEMAP
#include "Data/ArrayListNN.h"
#include "Map/MercatorTileMap.h"
#include "Sync/Mutex.h"

namespace Map
{
	namespace OSM
	{
		class OSMTileMap : public Map::MercatorTileMap
		{
		private:
			Data::ArrayListNN<Text::String> urls;
			UOSInt urlNext;
			Sync::Mutex urlMut;

			Text::String *GetNextURL();
		public:
			OSMTileMap(Text::CString url, Text::CString cacheDir, UOSInt maxLevel, NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl);
			OSMTileMap(Text::CString url, IO::SPackageFile *spkg, UOSInt maxLevel, NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl);
			virtual ~OSMTileMap();

			void AddAlternateURL(Text::CString url);
			Text::String *GetOSMURL(UOSInt index);

			virtual Text::CString GetName();
			virtual TileType GetTileType();
			virtual UOSInt GetConcurrentCount();
			virtual UTF8Char *GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> tileId);
		};
	}
}
#endif
