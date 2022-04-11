#ifndef _SM_MAP_ASSISTEDREVERSEGEOCODERPL
#define _SM_MAP_ASSISTEDREVERSEGEOCODERPL
#include "Data/ArrayList.h"
#include "Data/BTreeMap.h"
#include "Data/Comparator.h"
#include "Data/Int32Map.h"
#include "Data/UInt32Map.h"
#include "DB/DBTool.h"
#include "IO/LogTool.h"
#include "IO/Writer.h"
#include "Map/IAssistedReverseGeocoder.h"
#include "Sync/Mutex.h"

namespace Map
{
	class AssistedReverseGeocoderPL : public Map::IAssistedReverseGeocoder
	{
	private:
		struct AddressEntry
		{
			Int32 keyx;
			Int32 keyy;
			Text::String *address;
			Int64 addrTime;
		};
		typedef struct
		{
			UInt32 lcid;
			Data::ArrayList<AddressEntry *> *mainList;
		} LCIDInfo;

		class AddressComparator : public Data::Comparator<AddressEntry*>
		{
		public:
			virtual ~AddressComparator();

			virtual OSInt Compare(AddressEntry *a, AddressEntry *b);
		};
	private:
		Data::ArrayList<Map::IReverseGeocoder *> revGeos;
		UOSInt nextCoder;
		DB::DBTool *conn;
		IO::Writer *errWriter;
		Data::BTreeMap<Text::String *> strMap;
		Data::UInt32Map<LCIDInfo *> lcidMap;
		Sync::Mutex mut;
	public:
		AssistedReverseGeocoderPL(DB::DBTool *db, IO::Writer *errWriter);
		virtual ~AssistedReverseGeocoderPL();

		virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid);
		virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid);
		virtual void AddReverseGeocoder(Map::IReverseGeocoder *revGeo);
	private:
		OSInt AddressIndexOf(Data::ArrayList<AddressEntry *> *list, Int32 keyx, Int32 keyy);
	};
}
#endif
