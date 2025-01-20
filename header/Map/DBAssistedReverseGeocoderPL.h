#ifndef _SM_MAP_DBASSISTEDREVERSEGEOCODERPL
#define _SM_MAP_DBASSISTEDREVERSEGEOCODERPL
#include "Data/ArrayListNN.h"
#include "Data/BTreeMap.h"
#include "Data/Comparator.h"
#include "Data/FastMapNN.h"
#include "DB/DBTool.h"
#include "IO/LogTool.h"
#include "IO/Writer.h"
#include "Map/AssistedReverseGeocoder.h"
#include "Sync/Mutex.h"

namespace Map
{
	class DBAssistedReverseGeocoderPL : public Map::AssistedReverseGeocoder
	{
	private:
		struct AddressEntry
		{
			Int32 keyx;
			Int32 keyy;
			Text::String *address;
			Int64 addrTime;
		};
		struct LCIDInfo
		{
			UInt32 lcid;
			Data::ArrayListNN<AddressEntry> mainList;
		};

		class AddressComparator : public Data::Comparator<NN<AddressEntry>>
		{
		public:
			virtual ~AddressComparator();

			virtual OSInt Compare(NN<AddressEntry> a, NN<AddressEntry> b) const;
		};
	private:
		Data::ArrayListNN<Map::ReverseGeocoder> revGeos;
		UOSInt nextCoder;
		NN<DB::DBTool> conn;
		NN<IO::Writer> errWriter;
		Data::BTreeMap<Text::String *> strMap;
		Data::FastMapNN<UInt32, LCIDInfo> lcidMap;
		Sync::Mutex mut;
	public:
		DBAssistedReverseGeocoderPL(NN<DB::DBTool> db, NN<IO::Writer> errWriter);
		virtual ~DBAssistedReverseGeocoderPL();

		virtual UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		virtual UnsafeArrayOpt<UTF8Char> CacheName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		virtual void AddReverseGeocoder(NN<Map::ReverseGeocoder> revGeo);
	private:
		OSInt AddressIndexOf(NN<Data::ArrayListNN<AddressEntry>> list, Int32 keyx, Int32 keyy);
	};
}
#endif
