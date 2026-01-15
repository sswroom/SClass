#ifndef _SM_MAP_GOOGLEMAP_GOOGLESTATICMAP
#define _SM_MAP_GOOGLEMAP_GOOGLESTATICMAP
#include "Math/Size2D.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"
#include "Sync/Mutex.h"
#include "Text/String.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleStaticMap
		{
		private:
			Optional<Text::String> gooKey;
			Optional<Text::String> gooCliId;
			UnsafeArrayOpt<UInt8> gooPrivKey;
			UIntOS gooPrivKeyLeng;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			
		public:
			GoogleStaticMap(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::String> gooKey, Optional<Text::String> gooCliId, Optional<Text::String> gooPrivKey);
			GoogleStaticMap(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CString gooKey, Text::CString gooCliId, Text::CString gooPrivKey);
			~GoogleStaticMap();

			static UInt32 Level2Scale(UInt32 level);
			static UInt32 Scale2Level(UInt32 scale);
			UIntOS GetMap(UnsafeArray<UInt8> buff, Double lat, Double lon, UInt32 scale, Math::Size2D<UIntOS> imgSize, Text::CString lang, Int32 format, Double marker_lat, Double marker_lon); //lang = en-us, zh-cn, zh-tw, format = 0:png, 1:gif, 2:jpg
		};
	}
}
#endif
