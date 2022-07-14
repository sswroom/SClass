#ifndef _SM_MAP_MULTIREVERSEGEOCODER
#define _SM_MAP_MULTIREVERSEGEOCODER
#include "Data/ArrayList.h"
#include "IO/Writer.h"
#include "Sync/Mutex.h"
#include "Map/IReverseGeocoder.h"

namespace Map
{
	class MultiReverseGeocoder : public Map::IReverseGeocoder
	{
	private:
		Data::ArrayList<Map::IReverseGeocoder *> revGeos;
		UOSInt nextCoder;
		IO::Writer *errWriter;
		Sync::Mutex mut;
	public:
		MultiReverseGeocoder(IO::Writer *errWriter);
		virtual ~MultiReverseGeocoder();

		virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		void AddReverseGeocoder(Map::IReverseGeocoder *revGeo);
	};
}
#endif
