#ifndef _SM_MEDIA_AUDIOFIXBLOCKSOURCE
#define _SM_MEDIA_AUDIOFIXBLOCKSOURCE
#include "Sync/Event.h"
#include "IO/StreamData.h"
#include "Media/IMediaSource.h"
#include "Media/IAudioSource.h"

namespace Media
{
	class AudioFixBlockSource : public IAudioSource
	{
	private:
		Media::AudioFormat format;
		NotNullPtr<IO::StreamData> data;
		NotNullPtr<Text::String> name;

		UInt64 readOfst;
		Sync::Event *readEvt;
	public:
		AudioFixBlockSource(NotNullPtr<IO::StreamData> fd, UInt64 ofst, UInt64 length, NotNullPtr<const Media::AudioFormat> format, NotNullPtr<Text::String> name);
		virtual ~AudioFixBlockSource();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(NotNullPtr<AudioFormat> format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
