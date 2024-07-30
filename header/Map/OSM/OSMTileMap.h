#ifndef _SM_MAP_OSM_OSMTILEMAP
#define _SM_MAP_OSM_OSMTILEMAP
#include "Data/ArrayListStringNN.h"
#include "Map/MercatorTileMap.h"
#include "Sync/Mutex.h"

namespace Map
{
	namespace OSM
	{
		class OSMTileMap : public Map::MercatorTileMap
		{
		private:
			Data::ArrayListStringNN urls;
			UOSInt urlNext;
			Sync::Mutex urlMut;

			Optional<Text::String> GetNextURL();
		public:
			OSMTileMap(Text::CStringNN url, Text::CString cacheDir, UOSInt minLevel, UOSInt maxLevel, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl);
			OSMTileMap(Text::CStringNN url, IO::SPackageFile *spkg, UOSInt minLevel, UOSInt maxLevel, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl);
			virtual ~OSMTileMap();

			void AddAlternateURL(Text::CStringNN url);
			Optional<Text::String> GetOSMURL(UOSInt index);

			virtual Text::CStringNN GetName() const;
			virtual TileType GetTileType() const;
			virtual ImageType GetImageType() const;
			virtual UOSInt GetConcurrentCount() const;
			virtual UnsafeArrayOpt<UTF8Char> GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UOSInt level, Math::Coord2D<Int32> tileId);
			virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId);
		};
	}
}
#endif
