#ifndef _SM_MEDIA_AUDIOFILTER_AUDIOSAMPLERIPPER
#define _SM_MEDIA_AUDIOFILTER_AUDIOSAMPLERIPPER
#include "Media/IAudioFilter.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AudioFilter
	{
		class AudioSampleRipper : public IAudioFilter
		{
		private:
			UInt32 nChannel;
			UInt32 bitCount;
			UInt32 soundBuffLeng;
			UInt8 *soundBuff;
			UInt32 soundBuffOfst;
			Sync::Mutex mut;
			Bool changed;
		private:
			void ResetStatus();
		public:
			AudioSampleRipper(NN<IAudioSource> sourceAudio, UInt32 sampleCount);
			virtual ~AudioSampleRipper();

			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size

			Bool IsChanged();
			Bool GetSamples(UInt8 *samples);
		};
	}
}
#endif
