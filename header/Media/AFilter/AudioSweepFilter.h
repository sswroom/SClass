#ifndef _SM_MEDIA_AFILTER_AUDIOSWEEPFILTER
#define _SM_MEDIA_AFILTER_AUDIOSWEEPFILTER
#include "Media/AudioFilter.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AFilter
	{
		class AudioSweepFilter : public AudioFilter
		{
		protected:
			AudioFormat format;
			Sync::Mutex mut;
			Double vol;
			UInt32 currSample;
			UInt32 endSample;
			Double currT;
			Double startFreq;
			Double endFreq;

		public:
			AudioSweepFilter(NN<AudioSource> sourceAudio);
			virtual ~AudioSweepFilter();

			virtual void GetFormat(NN<AudioFormat> format);
			virtual UIntOS ReadBlock(Data::ByteArray blk); //ret actual block size

			void SetVolume(Double vol);
			Bool StartSweep(Double startFreq, Double endFreq, UInt32 timeSeconds);
		};
	}
}
#endif
