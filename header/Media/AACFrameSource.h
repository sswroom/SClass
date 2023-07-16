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
		AACFrameSource(IO::StreamData *fd, Media::AudioFormat *format, NotNullPtr<Text::String> name);
		AACFrameSource(IO::StreamData *fd, Media::AudioFormat *format, Text::CString name);
		virtual ~AACFrameSource();

		virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual UOSInt GetMinBlockSize();
	private:
		Int32 GetRateIndex();
	};
}
#endif
