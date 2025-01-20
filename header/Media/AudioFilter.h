#ifndef _SM_MEDIA_AUDIOFILTER
#define _SM_MEDIA_AUDIOFILTER
#include "Media/AudioSource.h"

namespace Media
{
	class AudioFilter : public AudioSource
	{
	protected:
		NN<Media::AudioSource> sourceAudio;

	public:
		AudioFilter(NN<AudioSource> sourceAudio);
		virtual ~AudioFilter();

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);

		virtual void GetFormat(NN<AudioFormat> format);

		virtual Bool Start(Optional<Sync::Event> evt, UOSInt blkSize);
		virtual void Stop();
		//virtual OSInt ReadBlock(UInt8 *buff, OSInt blkSize); //ret actual block size
		virtual UOSInt GetMinBlockSize();
		virtual Data::Duration GetCurrTime();
		virtual Bool IsEnd();
	};
}
#endif
