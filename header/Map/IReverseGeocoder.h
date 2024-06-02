#ifndef _SM_MAP_IREVERSEGEOCODER
#define _SM_MAP_IREVERSEGEOCODER
#include "Math/Coord2DDbl.h"

namespace Map
{
	class IReverseGeocoder
	{
	public:
		virtual ~IReverseGeocoder() {};

		virtual UnsafeArrayOpt<UTF8Char> SearchName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid) = 0;
		virtual UnsafeArrayOpt<UTF8Char> CacheName(UnsafeArray<UTF8Char> buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid) = 0;
	};
}
#endif
