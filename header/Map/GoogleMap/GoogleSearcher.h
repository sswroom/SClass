#ifndef _SM_MAP_GOOGLEMAP_GOOGLESEARCHER
#define _SM_MAP_GOOGLEMAP_GOOGLESEARCHER
#include "Data/DateTime.h"
#include "IO/Writer.h"
#include "Map/ReverseGeocoder.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"
#include "Sync/Mutex.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleSearcher : public Map::ReverseGeocoder
		{
		private:
			Optional<Text::String> gooKey;
			Text::String *gooCliId;
			UInt8 *gooPrivKey;
			UOSInt gooPrivKeyLeng;
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Data::DateTime lastSrchDate;
			Sync::Mutex mut;
			NN<IO::Writer> errWriter;
			Int32 lastIsError;
			UInt32 srchCnt;
			
		public:
			GoogleSearcher(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::String *gooKey, Text::String *gooCliId, Text::String *gooPrivKey, NN<IO::Writer> errWriter);
			GoogleSearcher(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CString gooKey, Text::CString gooCliId, Text::CString gooPrivKey, NN<IO::Writer> errWriter);
			virtual ~GoogleSearcher();

			UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, Text::CString lang); //lang = en-us, zh-cn, zh-tw
			virtual UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
			virtual UnsafeArrayOpt<UTF8Char> CacheName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
			virtual UInt32 GetSrchCnt();
		};
	}
}
#endif
