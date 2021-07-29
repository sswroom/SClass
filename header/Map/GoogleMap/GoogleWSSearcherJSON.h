#ifndef _SM_MAP_GOOGLEMAP_GOOGLEWSSEARCHERJSON
#define _SM_MAP_GOOGLEMAP_GOOGLEWSSEARCHERJSON
#include "Data/DateTime.h"
#include "IO/Writer.h"
#include "Map/IReverseGeocoder.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Sync/Mutex.h"
#include "Text/EncodingFactory.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleWSSearcherJSON : public Map::IReverseGeocoder
		{
		private:
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			IO::Writer *errWriter;
			Data::DateTime *lastSrchDate;
			Sync::Mutex *mut;
			Text::EncodingFactory *encFact;
			Int32 lastIsError;
			UInt32 srchCnt;
			const UTF8Char *gooCliId;
			UInt8 *gooPrivKey;
			UOSInt gooPrivKeyLeng;
			const UTF8Char *gooAPIKey;

		public:
			GoogleWSSearcherJSON(Net::SocketFactory *sockf, Net::SSLEngine *ssl, IO::Writer *errWriter, Text::EncodingFactory *encFact);
			virtual ~GoogleWSSearcherJSON();

			void SetGoogleClientId(const UTF8Char *gooCliId, const UTF8Char *gooPrivKey);
			void SetGoogleAPIKey(const UTF8Char *gooAPIKey);

			UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, const UTF8Char *lang); //lang = en-us, zh-cn, zh-tw
			virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid);
			virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid);
			virtual UInt32 GetSrchCnt();
		};
	}
}
#endif
