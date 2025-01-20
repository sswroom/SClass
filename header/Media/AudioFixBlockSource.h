#ifndef _SM_MEDIA_AUDIOFIXBLOCKSOURCE
#define _SM_MEDIA_AUDIOFIXBLOCKSOURCE
#include "Sync/Event.h"
#include "IO/StreamData.h"
#include "Media/MediaSource.h"
#include "Media/AudioSource.h"

namespace Media
{
	class AudioFixBlockSource : public AudioSource
	{
	private:
		Media::AudioFormat format;
		NN<IO::StreamData> data;
		NN<Text::String> name;

		UInt64 readOfst;
		Optional<Sync::Event> readEvt;
	public:
		AudioFixBlockSource(NN<IO::StreamData> fd, UInt64 ofst, UInt64 length, NN<const Media::AudioFormat> format, NN<Text::String> name);
		virtual ~AudioFixBlockSource();

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);

		virtual void GetFormat(NN<AudioFormat> format);

		virtual Bool Start(Optional<Sync::Event> evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
