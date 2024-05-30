#ifndef _SM_MEDIA_IAUDIOSOURCE
#define _SM_MEDIA_IAUDIOSOURCE
#include "Data/ByteArray.h"
#include "Media/AudioFormat.h"
#include "Media/IMediaSource.h"
#include "Sync/Event.h"

namespace Media
{
	class IAudioSource : public IMediaSource
	{
	public:
		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff) = 0;
		virtual Bool CanSeek() = 0;
		virtual Data::Duration GetStreamTime() = 0;
		virtual Data::Duration SeekToTime(Data::Duration time) = 0;
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime) = 0;

		virtual void GetFormat(NN<AudioFormat> format) = 0;

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize) = 0;
		virtual void Stop() = 0;
		virtual UOSInt ReadBlock(Data::ByteArray blk) = 0; //ret actual block size
		virtual UOSInt GetMinBlockSize() = 0;
		virtual Data::Duration GetCurrTime() = 0;
		virtual Bool IsEnd() = 0;
		virtual MediaType GetMediaType();

		virtual Bool SupportSampleRead();
		virtual UOSInt ReadSample(UInt64 sampleOfst, UOSInt sampleCount, Data::ByteArray buff);
		virtual Int64 GetSampleCount(); // -1 = infinity

		UOSInt ReadBlockLPCM(Data::ByteArray blk, NN<const AudioFormat> format);
	};
}
#endif
