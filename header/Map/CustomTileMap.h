#ifndef _SM_MAP_CUSTOMTILEMAP
#define _SM_MAP_CUSTOMTILEMAP
#include "Map/MercatorTileMap.h"

namespace Map
{
	class CustomTileMap : public Map::MercatorTileMap
	{
	private:
		Text::String *url;
		Text::String *name;
		UOSInt concurrCnt;

	public:
		CustomTileMap(Text::CString url, Text::CString cacheDir, UOSInt minLevel, UOSInt maxLevel, Net::SocketFactory *sockf, Net::SSLEngine *ssl);
		virtual ~CustomTileMap();

		virtual Text::CString GetName();
		virtual TileType GetTileType();
		virtual UOSInt GetConcurrentCount();
		virtual UTF8Char *GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId);
	};
}
#endif
