#ifndef _SM_MEDIA_AUDIOFILTER_AUDIOAMPLIFIER
#define _SM_MEDIA_AUDIOFILTER_AUDIOAMPLIFIER
#include "Media/IAudioFilter.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AudioFilter
	{
		class AudioAmplifier : public IAudioFilter
		{
		private:
			UInt32 bitCount;
			Double level;
		public:
			AudioAmplifier(NN<IAudioSource> sourceAudio);
			virtual ~AudioAmplifier();

			virtual void GetFormat(NN<AudioFormat> format);
			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size

			void SetLevel(Double level);
		};
	}
}
#endif
