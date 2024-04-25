#ifndef _SM_MEDIA_AACFRAMESOURCE
#define _SM_MEDIA_AACFRAMESOURCE
#include "Sync/Event.h"
#include "IO/StreamData.h"
#include "Media/AudioFrameSource.h"

namespace Media
{
	class AACFrameSource : public AudioFrameSource
	{
	public:
		AACFrameSource(NN<IO::StreamData> fd, NN<const Media::AudioFormat> format, NN<Text::String> name);
		AACFrameSource(NN<IO::StreamData> fd, NN<const Media::AudioFormat> format, Text::CStringNN name);
		virtual ~AACFrameSource();

		virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual UOSInt GetMinBlockSize();
	private:
		Int32 GetRateIndex();
	};
}
#endif
