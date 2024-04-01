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
			NotNullPtr<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			IO::Writer *errWriter;
			Data::DateTime lastSrchDate;
			Sync::Mutex mut;
			NotNullPtr<Text::EncodingFactory> encFact;
			Bool lastIsError;
			
		public:
			GoogleWSSearcherXML(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, IO::Writer *errWriter, NotNullPtr<Text::EncodingFactory> encFact);
			virtual ~GoogleWSSearcherXML();

			UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Text::CString lang); //lang = en-us, zh-cn, zh-tw
			virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
			virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, Int32 lcid);
		};
	}
}
#endif
