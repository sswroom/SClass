#ifndef _SM_MEDIA_AACFRAMESOURCE
#define _SM_MEDIA_AACFRAMESOURCE
#include "Sync/Event.h"
#include "IO/IStreamData.h"
#include "Media/AudioFrameSource.h"

namespace Media
{
	class AACFrameSource : public AudioFrameSource
	{
	public:
		AACFrameSource(IO::IStreamData *fd, Media::AudioFormat *format, const UTF8Char *name);
		virtual ~AACFrameSource();

		virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size
		virtual UOSInt GetMinBlockSize();
	private:
		Int32 GetRateIndex();
	};
}
#endif
