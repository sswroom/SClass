#ifndef _SM_MEDIA_AFILTER_AUDIOSAMPLERIPPER
#define _SM_MEDIA_AFILTER_AUDIOSAMPLERIPPER
#include "Media/AudioFilter.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AFilter
	{
		class AudioSampleRipper : public AudioFilter
		{
		private:
			UInt32 nChannel;
			UInt32 bitCount;
			UInt32 soundBuffLeng;
			UnsafeArrayOpt<UInt8> soundBuff;
			UInt32 soundBuffOfst;
			Sync::Mutex mut;
			Bool changed;
		private:
			void ResetStatus();
		public:
			AudioSampleRipper(NN<AudioSource> sourceAudio, UInt32 sampleCount);
			virtual ~AudioSampleRipper();

			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual UIntOS ReadBlock(Data::ByteArray blk); //ret actual block size

			Bool IsChanged();
			Bool GetSamples(UnsafeArray<UInt8> samples);
		};
	}
}
#endif
