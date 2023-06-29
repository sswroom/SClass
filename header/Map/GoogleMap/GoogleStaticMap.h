#ifndef _SM_MAP_GOOGLEMAP_GOOGLESTATICMAP
#define _SM_MAP_GOOGLEMAP_GOOGLESTATICMAP
#include "Math/Size2D.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleStaticMap
		{
		private:
			Text::String *gooKey;
			Text::String *gooCliId;
			UInt8 *gooPrivKey;
			UOSInt gooPrivKeyLeng;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			
		public:
			GoogleStaticMap(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::String *gooKey, Text::String *gooCliId, Text::String *gooPrivKey);
			GoogleStaticMap(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString gooKey, Text::CString gooCliId, Text::CString gooPrivKey);
			~GoogleStaticMap();

			static UInt32 Level2Scale(UInt32 level);
			static UInt32 Scale2Level(UInt32 scale);
			UOSInt GetMap(UInt8 *buff, Double lat, Double lon, UInt32 scale, Math::Size2D<UOSInt> imgSize, Text::CString lang, Int32 format, Double marker_lat, Double marker_lon); //lang = en-us, zh-cn, zh-tw, format = 0:png, 1:gif, 2:jpg
		};
	}
}
#endif
