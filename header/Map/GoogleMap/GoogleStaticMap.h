#ifndef _SM_MAP_GOOGLEMAP_GOOGLESTATICMAP
#define _SM_MAP_GOOGLEMAP_GOOGLESTATICMAP
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
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
			UOSInt gooPrivKeyLeng;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Data::DateTime *lastSrchDate;
			Sync::Mutex *mut;
			
		public:
			GoogleStaticMap(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *gooKey, const UTF8Char *gooCliId, const UTF8Char *gooPrivKey);
			~GoogleStaticMap();

			static UInt32 Level2Scale(UInt32 level);
			static UInt32 Scale2Level(UInt32 scale);
			UOSInt GetMap(UInt8 *buff, Double lat, Double lon, UInt32 scale, UInt32 width, UInt32 height, const UTF8Char *lang, Int32 format, Double marker_lat, Double marker_lon); //lang = en-us, zh-cn, zh-tw, format = 0:png, 1:gif, 2:jpg
		};
	}
}
#endif
