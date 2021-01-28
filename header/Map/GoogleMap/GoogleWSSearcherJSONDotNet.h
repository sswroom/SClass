#ifndef _SM_MAP_GOOGLEMAP_GOOGLEWSSEARCHERJSONDOTNET
#define _SM_MAP_GOOGLEMAP_GOOGLEWSSEARCHERJSONDOTNET
#include "Data/DateTime.h"
#include "IO/IWriter.h"
#include "Map/IReverseGeocoder.h"
#include "Net/SocketFactory.h"
#include "Sync/Mutex.h"
#include "Text/EncodingFactory.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleWSSearcherJSONDotNet : public Map::IReverseGeocoder
		{
		private:
			Net::SocketFactory *sockf;
			IO::IWriter *errWriter;
			Data::DateTime *lastSrchDate;
			Sync::Mutex *mut;
			Text::EncodingFactory *encFact;
			Int32 lastIsError;
			Int32 srchCnt;
			const UTF8Char *gooCliId;
			UInt8 *gooPrivKey;
			OSInt gooPrivKeyLeng;
			const UTF8Char *gooAPIKey;

		public:
			GoogleWSSearcherJSONDotNet(Net::SocketFactory *sockf, IO::IWriter *errWriter, Text::EncodingFactory *encFact);
			virtual ~GoogleWSSearcherJSONDotNet();

			void SetGoogleClientId(const UTF8Char *gooCliId, const UTF8Char *gooPrivKey);
			void SetGoogleAPIKey(const UTF8Char *gooAPIKey);

			UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, const UTF8Char *lang); //lang = en-us, zh-cn, zh-tw
			virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
			virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
			virtual Int32 GetSrchCnt();
		};
	}
}
#endif
