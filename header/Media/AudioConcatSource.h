#ifndef _SM_MEDIA_AUDIOCONCATSOURCE
#define _SM_MEDIA_AUDIOCONCATSOURCE
#include "Media/IAudioSource.h"
#include "Data/ArrayListNN.h"

namespace Media
{
	class AudioConcatSource : public Media::IAudioSource
	{
	private:
		Media::AudioFormat format;
		Data::ArrayListNN<Media::IAudioSource> stmList;

		Data::Duration stmTime;
		Bool canSeek;

		UOSInt currStm;
		Int64 readOfst;
		UOSInt readBlkSize;
		Optional<Sync::Event> readEvt;

	public:
		AudioConcatSource();
		virtual ~AudioConcatSource();

		Bool AppendAudio(NN<Media::IAudioSource> audio);
		Bool AppendSilent(UInt32 time);

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual void GetFormat(NN<AudioFormat> format);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);

		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool Start(Optional<Sync::Event> evt, UOSInt blkSize);
		virtual void Stop();
		virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual Data::Duration GetCurrTime();
		virtual UOSInt GetMinBlockSize();
		virtual Bool IsEnd();
	};
}
#endif
