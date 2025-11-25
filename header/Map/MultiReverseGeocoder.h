#ifndef _SM_MAP_MULTIREVERSEGEOCODER
#define _SM_MAP_MULTIREVERSEGEOCODER
#include "Data/ArrayListNN.hpp"
#include "IO/Writer.h"
#include "Sync/Mutex.h"
#include "Map/ReverseGeocoder.h"

namespace Map
{
	class MultiReverseGeocoder : public Map::ReverseGeocoder
	{
	private:
		Data::ArrayListNN<Map::ReverseGeocoder> revGeos;
		UOSInt nextCoder;
		Optional<IO::Writer> errWriter;
		Sync::Mutex mut;
	public:
		MultiReverseGeocoder(Optional<IO::Writer> errWriter);
		virtual ~MultiReverseGeocoder();

		virtual UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		virtual UnsafeArrayOpt<UTF8Char> CacheName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid);
		void AddReverseGeocoder(NN<Map::ReverseGeocoder> revGeo);
	};
}
#endif
