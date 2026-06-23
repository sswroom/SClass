#ifndef _SM_MAP_GOOGLEMAP_GOOGLEWSSEARCHERXML
#define _SM_MAP_GOOGLEMAP_GOOGLEWSSEARCHERXML
#include "IO/Writer.h"
#include "Net/SocketFactory.h"
#include "Data/DateTime.h"
#include "Sync/Mutex.h"
#include "Map/ReverseGeocoder.h"
#include "Text/EncodingFactory.h"

namespace Map
{
	namespace GoogleMap
	{
		class GoogleWSSearcherXML : public Map::ReverseGeocoder
		{
		private:
			NN<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			NN<IO::Writer> errWriter;
			Data::DateTime lastSrchDate;
			Sync::Mutex mut;
			NN<Text::EncodingFactory> encFact;
			Bool lastIsError;
			
		public:
			GoogleWSSearcherXML(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, NN<IO::Writer> errWriter, NN<Text::EncodingFactory> encFact);
			virtual ~GoogleWSSearcherXML();

			UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Double lat, Double lon, Text::CString lang); //lang = en-us, zh-cn, zh-tw
			virtual UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Double lat, Double lon, Int32 lcid);
			virtual UnsafeArrayOpt<UTF8Char> CacheName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Double lat, Double lon, Int32 lcid);
		};
	}
}
#endif
