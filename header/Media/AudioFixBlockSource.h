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
		IO::StreamData *data;
		Text::String *name;

		UInt64 readOfst;
		Sync::Event *readEvt;
	public:
		AudioFixBlockSource(IO::StreamData *fd, UInt64 ofst, UInt64 length, Media::AudioFormat *format, Text::String *name);
		virtual ~AudioFixBlockSource();

		virtual UTF8Char *GetSourceName(UTF8Char *buff);
		virtual Bool CanSeek();
		virtual Int32 GetStreamTime(); //ms
		virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime);

		virtual void GetFormat(AudioFormat *format);

		virtual Bool Start(Sync::Event *evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual UInt32 GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
