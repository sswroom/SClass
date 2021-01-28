#ifndef _SM_MEDIA_LRGBLIMITER
#define _SM_MEDIA_LRGBLIMITER

namespace Media
{
/*	class LRGBLimiter
	{
	public:
		static void LimitImageLRGB(UInt8 *imgPtr, OSInt w, OSInt h);
	};*/
};
extern "C" void LRGBLimiter_LimitImageLRGB(UInt8 *imgPtr, OSInt w, OSInt h);
#endif
