#ifndef _SM_MEDIA_AUDIOCONCATSOURCE
#define _SM_MEDIA_AUDIOCONCATSOURCE
#include "Media/AudioSource.h"
#include "Data/ArrayListNN.hpp"

namespace Media
{
	class AudioConcatSource : public Media::AudioSource
	{
	private:
		Media::AudioFormat format;
		Data::ArrayListNN<Media::AudioSource> stmList;

		Data::Duration stmTime;
		Bool canSeek;

		UIntOS currStm;
		Int64 readOfst;
		UIntOS readBlkSize;
		Optional<Sync::Event> readEvt;

	public:
		AudioConcatSource();
		virtual ~AudioConcatSource();

		Bool AppendAudio(NN<Media::AudioSource> audio);
		Bool AppendSilent(UInt32 time);

		virtual UnsafeArrayOpt<UTF8Char> GetSourceName(UnsafeArray<UTF8Char> buff);
		virtual Bool CanSeek();
		virtual Data::Duration GetStreamTime();
		virtual void GetFormat(NN<AudioFormat> format);
		virtual Bool TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime);

		virtual Data::Duration SeekToTime(Data::Duration time);
		virtual Bool Start(Optional<Sync::Event> evt, UIntOS blkSize);
		virtual void Stop();
		virtual UIntOS ReadBlock(Data::ByteArray blk); //ret actual block size
		virtual Data::Duration GetCurrTime();
		virtual UIntOS GetMinBlockSize();
		virtual Bool IsEnd();
	};
}
#endif
