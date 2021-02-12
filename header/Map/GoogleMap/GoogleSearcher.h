#ifndef _SM_MAP_GOOGLEMAP_GOOGLESEARCHER
#define _SM_MAP_GOOGLEMAP_GOOGLESEARCHER
#include "Data/DateTime.h"
#include "IO/Writer.h"
#include "Sync/Mutex.h"
#include "Net/SocketFactory.h"
#include "Map/IReverseGeocoder.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleSearcher : public Map::IReverseGeocoder
		{
		private:
			const UTF8Char *gooKey;
			const UTF8Char *gooCliId;
			UInt8 *gooPrivKey;
			OSInt gooPrivKeyLeng;
			Net::SocketFactory *sockf;
			Data::DateTime *lastSrchDate;
			Sync::Mutex *mut;
			IO::Writer *errWriter;
			Int32 lastIsError;
			Int32 srchCnt;
			
		public:
			GoogleSearcher(Net::SocketFactory *sockf, const UTF8Char *gooKey, const UTF8Char *gooCliId, const UTF8Char *gooPrivKey, IO::Writer *errWriter);
			virtual ~GoogleSearcher();

			UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, const UTF8Char *lang); //lang = en-us, zh-cn, zh-tw
			virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
			virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
			virtual Int32 GetSrchCnt();
		};
	}
}
#endif
