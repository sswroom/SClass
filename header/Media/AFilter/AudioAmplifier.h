#ifndef _SM_MEDIA_AFILTER_AUDIOAMPLIFIER
#define _SM_MEDIA_AFILTER_AUDIOAMPLIFIER
#include "Media/AudioFilter.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AFilter
	{
		class AudioAmplifier : public AudioFilter
		{
		private:
			UInt32 bitCount;
			Double level;
		public:
			AudioAmplifier(NN<AudioSource> sourceAudio);
			virtual ~AudioAmplifier();

			virtual void GetFormat(NN<AudioFormat> format);
			virtual Data::Duration SeekToTime(Data::Duration time);
			virtual UIntOS ReadBlock(Data::ByteArray blk); //ret actual block size

			void SetLevel(Double level);
		};
	}
}
#endif
