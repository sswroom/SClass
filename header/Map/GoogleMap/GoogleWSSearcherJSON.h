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
			NN<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			IO::Writer *errWriter;
			Data::DateTime lastSrchDate;
			Sync::Mutex mut;
			Optional<Text::EncodingFactory> encFact;
			Int32 lastIsError;
			UInt32 srchCnt;
			Text::String *gooCliId;
			UInt8 *gooPrivKey;
			UOSInt gooPrivKeyLeng;
			Optional<Text::String> gooAPIKey;

		public:
			GoogleWSSearcherJSON(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, IO::Writer *errWriter, Optional<Text::EncodingFactory> encFact);
			virtual ~GoogleWSSearcherJSON();

			void SetGoogleClientId(Text::String *gooCliId, Text::String *gooPrivKey);
			void SetGoogleClientId(Text::CString gooCliId, Text::CString gooPrivKey);
			void SetGoogleAPIKey(Optional<Text::String> gooAPIKey);
			void SetGoogleAPIKey(Text::CString gooAPIKey);

			UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, const UTF8Char *lang); //lang = en-us, zh-cn, zh-tw
			virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
			virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
			virtual UInt32 GetSrchCnt();
		};
	}
}
#endif
