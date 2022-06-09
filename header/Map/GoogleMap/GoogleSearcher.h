#ifndef _SM_MAP_GOOGLEMAP_GOOGLESEARCHER
#define _SM_MAP_GOOGLEMAP_GOOGLESEARCHER
#include "Data/DateTime.h"
#include "IO/Writer.h"
#include "Map/IReverseGeocoder.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Sync/Mutex.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleSearcher : public Map::IReverseGeocoder
		{
		private:
			Text::String *gooKey;
			Text::String *gooCliId;
			UInt8 *gooPrivKey;
			UOSInt gooPrivKeyLeng;
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Data::DateTime lastSrchDate;
			Sync::Mutex mut;
			IO::Writer *errWriter;
			Int32 lastIsError;
			UInt32 srchCnt;
			
		public:
			GoogleSearcher(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::String *gooKey, Text::String *gooCliId, Text::String *gooPrivKey, IO::Writer *errWriter);
			GoogleSearcher(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString gooKey, Text::CString gooCliId, Text::CString gooPrivKey, IO::Writer *errWriter);
			virtual ~GoogleSearcher();

			UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, Text::CString lang); //lang = en-us, zh-cn, zh-tw
			virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
			virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
			virtual UInt32 GetSrchCnt();
		};
	}
}
#endif
