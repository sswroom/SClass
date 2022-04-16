#ifndef _SM_MEDIA_SOXRFILTER
#define _SM_MEDIA_SOXRFILTER
#include "Media/IAudioFilter.h"

namespace Media
{
	class SOXRFilter : public Media::IAudioFilter
	{
	private:
		struct ClassData;
		ClassData *clsData;
	public:
		SOXRFilter(Media::IAudioSource *sourceAudio, UInt32 targetFreq);
		virtual ~SOXRFilter();

		virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size
	};
}
#endif
