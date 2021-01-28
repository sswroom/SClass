#ifndef _SM_MEDIA_IAUDIOFRAMESOURCE
#define _SM_MEDIA_IAUDIOFRAMESOURCE
#include "Media/IAudioSource.h"

namespace Media
{
	class IAudioFrameSource : public IAudioSource
	{
	public:
		virtual void AddBlock(UInt64 offset, UInt32 length, UInt32 decodedSample) = 0;
	};
}
#endif
