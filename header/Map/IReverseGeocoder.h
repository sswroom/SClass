#ifndef _SM_MAP_IREVERSEGEOCODER
#define _SM_MAP_IREVERSEGEOCODER
#include "Math/Coord2DDbl.h"

namespace Map
{
	class IReverseGeocoder
	{
	public:
		virtual ~IReverseGeocoder() {};

		virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid) = 0;
		virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Math::Coord2DDbl pos, UInt32 lcid) = 0;
	};
}
#endif
