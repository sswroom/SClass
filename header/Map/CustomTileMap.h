#ifndef _SM_MAP_CUSTOMTILEMAP
#define _SM_MAP_CUSTOMTILEMAP
#include "Map/MercatorTileMap.h"

namespace Map
{
	class CustomTileMap : public Map::MercatorTileMap
	{
	private:
		NN<Text::String> url;
		NN<Text::String> name;
		UIntOS minLevel;
		UIntOS concurrCnt;
		Math::RectAreaDbl bounds;

	public:
		CustomTileMap(Text::CStringNN url, Text::CString cacheDir, UIntOS minLevel, UIntOS maxLevel, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl);
		virtual ~CustomTileMap();

		virtual Text::CStringNN GetName() const;
		virtual TileType GetTileType() const;
		virtual ImageType GetImageType() const;
		virtual UIntOS GetConcurrentCount() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
		virtual UnsafeArrayOpt<UTF8Char> GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UIntOS level, Math::Coord2D<Int32> tileId);
		virtual Bool GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UIntOS level, Math::Coord2D<Int32> tileId);

		void SetBounds(Math::RectAreaDbl bounds);
		void SetName(Text::CStringNN name);
	};
}
#endif
