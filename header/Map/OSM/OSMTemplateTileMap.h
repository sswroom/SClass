#ifndef _SM_MAP_OSM_OSMTEMPLATETILEMAP
#define _SM_MAP_OSM_OSMTEMPLATETILEMAP
#include "Map/MercatorTileMap.h"
#include "Sync/Mutex.h"

namespace Map
{
	namespace OSM
	{
		class OSMTemplateTileMap : public Map::MercatorTileMap
		{
		private:
			Data::ArrayListStringNN urls;
			UIntOS urlNext;
			Sync::Mutex urlMut;

			Optional<Text::String> GetNextURL();
		public:
			OSMTemplateTileMap(Text::CStringNN url, Text::CStringNN cacheDir, UIntOS minLevel, UIntOS maxLevel, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl);
			virtual ~OSMTemplateTileMap();

			void AddAlternateURL(Text::CStringNN url);
			Optional<Text::String> GetOSMURL(UIntOS index);

			virtual Text::CStringNN GetName() const;
			virtual TileType GetTileType() const;
			virtual TileFormat GetTileFormat() const;
			virtual UIntOS GetConcurrentCount() const;
			virtual UnsafeArrayOpt<UTF8Char> GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UIntOS level, Math::Coord2D<Int32> tileId);
			virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UIntOS level, Math::Coord2D<Int32> tileId);
		};
	}
}
#endif
