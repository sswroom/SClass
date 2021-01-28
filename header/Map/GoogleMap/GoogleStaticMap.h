#ifndef _SM_MAP_GOOGLEMAP_GOOGLESTATICMAP
#define _SM_MAP_GOOGLEMAP_GOOGLESTATICMAP
#include "Net/SocketFactory.h"
#include "Sync/Mutex.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleStaticMap
		{
		private:
			const UTF8Char *gooKey;
			const UTF8Char *gooCliId;
			UInt8 *gooPrivKey;
			OSInt gooPrivKeyLeng;
			Net::SocketFactory *sockf;
			Data::DateTime *lastSrchDate;
			Sync::Mutex *mut;
			
		public:
			GoogleStaticMap(Net::SocketFactory *sockf, const UTF8Char *gooKey, const UTF8Char *gooCliId, const UTF8Char *gooPrivKey);
			~GoogleStaticMap();

			static Int32 Level2Scale(Int32 level);
			static Int32 Scale2Level(Int32 scale);
			OSInt GetMap(UInt8 *buff, Double lat, Double lon, Int32 scale, Int32 width, Int32 height, const UTF8Char *lang, Int32 format, Double marker_lat, Double marker_lon); //lang = en-us, zh-cn, zh-tw, format = 0:png, 1:gif, 2:jpg
		};
	};
};
#endif
