#ifndef _SM_MEDIA_IAUDIOSOURCE
#define _SM_MEDIA_IAUDIOSOURCE
#include "Media/AudioFormat.h"
#include "Media/IMediaSource.h"
#include "Sync/Event.h"

namespace Media
{
	class IAudioSource : public IMediaSource
	{
	public:
		virtual UTF8Char *GetSourceName(UTF8Char *buff) = 0;
		virtual Bool CanSeek() = 0;
		virtual Int32 GetStreamTime() = 0; //ms
		virtual UInt32 SeekToTime(UInt32 time) = 0; //ms, ret actual time
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime) = 0;

		virtual void GetFormat(AudioFormat *format) = 0;

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize) = 0;
		virtual void Stop() = 0;
		virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize) = 0; //ret actual block size
		virtual UOSInt GetMinBlockSize() = 0;
		virtual UInt32 GetCurrTime() = 0;
		virtual Bool IsEnd() = 0;
		virtual MediaType GetMediaType();

		virtual Bool SupportSampleRead();
		virtual UOSInt ReadSample(Int64 sampleOfst, UOSInt sampleCount, UInt8 *buff);
		virtual Int64 GetSampleCount(); // -1 = infinity

		UOSInt ReadBlockLPCM(UInt8 *buff, UOSInt blkSize, AudioFormat *format);
	};
}
#endif
