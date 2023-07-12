#ifndef _SM_MAP_CUSTOMTILEMAP
#define _SM_MAP_CUSTOMTILEMAP
#include "Map/MercatorTileMap.h"

namespace Map
{
	class CustomTileMap : public Map::MercatorTileMap
	{
	private:
		NotNullPtr<Text::String> url;
		NotNullPtr<Text::String> name;
		UOSInt concurrCnt;
		Math::RectAreaDbl bounds;

	public:
		CustomTileMap(Text::CString url, Text::CString cacheDir, UOSInt minLevel, UOSInt maxLevel, Net::SocketFactory *sockf, Net::SSLEngine *ssl);
		virtual ~CustomTileMap();

		virtual Text::CString GetName();
		virtual TileType GetTileType();
		virtual UOSInt GetConcurrentCount();
		virtual Bool GetBounds(Math::RectAreaDbl *bounds);
		virtual UTF8Char *GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> tileId);

		void SetBounds(Math::RectAreaDbl bounds);
		void SetName(Text::CString name);
	};
}
#endif
