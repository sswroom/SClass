#ifndef _SM_MAP_IREVERSEGEOCODER
#define _SM_MAP_IREVERSEGEOCODER

namespace Map
{
	class IReverseGeocoder
	{
	public:
		virtual ~IReverseGeocoder() {};

		virtual UTF8Char *SearchName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid) = 0;
		virtual UTF8Char *CacheName(UTF8Char *buff, UOSInt buffSize, Double lat, Double lon, UInt32 lcid) = 0;
	};
}
#endif
