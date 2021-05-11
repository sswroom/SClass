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
			Sync::Mutex *mut;
			Bool changed;
		private:
			void ResetStatus();
		public:
			AudioSampleRipper(IAudioSource *sourceAudio, UInt32 sampleCount);
			virtual ~AudioSampleRipper();

			virtual UInt32 SeekToTime(UInt32 time); //ms, ret actual time
			virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size

			Bool IsChanged();
			Bool GetSamples(UInt8 *samples);
		};
	}
}
#endif
