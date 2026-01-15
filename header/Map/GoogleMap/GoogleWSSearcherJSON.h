#ifndef _SM_MAP_GOOGLEMAP_GOOGLEWSSEARCHERJSON
#define _SM_MAP_GOOGLEMAP_GOOGLEWSSEARCHERJSON
#include "Data/DateTime.h"
#include "IO/Writer.h"
#include "Map/ReverseGeocoder.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"
#include "Sync/Mutex.h"
#include "Text/EncodingFactory.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleWSSearcherJSON : public Map::ReverseGeocoder
		{
		private:
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			NN<IO::Writer> errWriter;
			Data::DateTime lastSrchDate;
			Sync::Mutex mut;
			Optional<Text::EncodingFactory> encFact;
			Int32 lastIsError;
			UInt32 srchCnt;
			Optional<Text::String> gooCliId;
			UnsafeArrayOpt<UInt8> gooPrivKey;
			UIntOS gooPrivKeyLeng;
			Optional<Text::String> gooAPIKey;

		public:
			GoogleWSSearcherJSON(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<IO::Writer> errWriter, Optional<Text::EncodingFactory> encFact);
			virtual ~GoogleWSSearcherJSON();

			void SetGoogleClientId(Optional<Text::String> gooCliId, Optional<Text::String> gooPrivKey);
			void SetGoogleClientId(Text::CString gooCliId, Text::CString gooPrivKey);
			void SetGoogleAPIKey(Optional<Text::String> gooAPIKey);
			void SetGoogleAPIKey(Text::CString gooAPIKey);

			UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos, UnsafeArrayOpt<const UTF8Char> lang); //lang = en-us, zh-cn, zh-tw
			virtual UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos, UInt32 lcid);
			virtual UnsafeArrayOpt<UTF8Char> CacheName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos, UInt32 lcid);
			virtual UInt32 GetSrchCnt();
		};
	}
}
#endif
