#ifndef _SM_MEDIA_SOXRFILTER
#define _SM_MEDIA_SOXRFILTER
#include "Media/AudioFilter.h"

namespace Media
{
	class SOXRFilter : public Media::AudioFilter
	{
	private:
		struct ClassData;
		NN<ClassData> clsData;
	public:
		SOXRFilter(NN<Media::AudioSource> sourceAudio, UInt32 targetFreq);
		virtual ~SOXRFilter();

		virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
	};
}
#endif
