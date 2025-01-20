#ifndef _SM_MAP_REVERSEGEOCODER
#define _SM_MAP_REVERSEGEOCODER
#include "Math/Coord2DDbl.h"

namespace Map
{
	class ReverseGeocoder
	{
	public:
		virtual ~ReverseGeocoder() {};

		virtual UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid) = 0;
		virtual UnsafeArrayOpt<UTF8Char> CacheName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid) = 0;
	};
}
#endif
