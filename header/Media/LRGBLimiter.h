#ifndef _SM_MEDIA_LRGBLIMITER
#define _SM_MEDIA_LRGBLIMITER
#include "AnyType.h"
#include "Sync/ParallelTask.h"

namespace Media
{
	class LRGBLimiter
	{
	private:
		struct ThreadStatus
		{
			UnsafeArray<UInt8> imgPtr;
			UOSInt w;
			UOSInt h;
		};
	private:
		Sync::ParallelTask ptask;

		static void __stdcall TaskFunc(AnyType userObj);
	public:
		LRGBLimiter();
		~LRGBLimiter();

		void LimitImageLRGB(UnsafeArray<UInt8> imgPtr, UOSInt w, UOSInt h);
	};
}
#endif
