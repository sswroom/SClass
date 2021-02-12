#ifndef _SM_MAP_GOOGLEMAP_GOOGLEWSSEARCHERXML
#define _SM_MAP_GOOGLEMAP_GOOGLEWSSEARCHERXML
#include "IO/Writer.h"
#include "Net/SocketFactory.h"
#include "Data/DateTime.h"
#include "Sync/Mutex.h"
#include "Map/IReverseGeocoder.h"
#include "Text/EncodingFactory.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleWSSearcherXML : public Map::IReverseGeocoder
		{
		private:
			Net::SocketFactory *sockf;
			IO::Writer *errWriter;
			Data::DateTime *lastSrchDate;
			Sync::Mutex *mut;
			Text::EncodingFactory *encFact;
			Bool lastIsError;
			
		public:
			GoogleWSSearcherXML(Net::SocketFactory *sockf, IO::Writer *errWriter, Text::EncodingFactory *encFact);
			virtual ~GoogleWSSearcherXML();

			WChar *SearchName(WChar *buff, Double lat, Double lon, const WChar *lang); //lang = en-us, zh-cn, zh-tw
			virtual WChar *SearchName(WChar *buff, Double lat, Double lon, Int32 lcid);
			virtual WChar *CacheName(WChar *buff, Double lat, Double lon, Int32 lcid);
		};
	}
}
#endif
