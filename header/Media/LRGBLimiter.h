#ifndef _SM_MEDIA_LRGBLIMITER
#define _SM_MEDIA_LRGBLIMITER
#include "Sync/ParallelTask.h"

namespace Media
{
	class LRGBLimiter
	{
	private:
		struct ThreadStatus
		{
			UInt8 *imgPtr;
			UOSInt w;
			UOSInt h;
		};
	private:
		Sync::ParallelTask ptask;

		static void TaskFunc(void *userObj);
	public:
		LRGBLimiter();
		~LRGBLimiter();

		void LimitImageLRGB(UInt8 *imgPtr, UOSInt w, UOSInt h);
	};
}
#endif
