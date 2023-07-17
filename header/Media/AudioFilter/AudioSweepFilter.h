#ifndef _SM_MEDIA_AUDIOFILTER_AUDIOSWEEPFILTER
#define _SM_MEDIA_AUDIOFILTER_AUDIOSWEEPFILTER
#include "Media/IAudioFilter.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AudioFilter
	{
		class AudioSweepFilter : public IAudioFilter
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
			AudioSweepFilter(IAudioSource *sourceAudio);
			virtual ~AudioSweepFilter();

			virtual void GetFormat(AudioFormat *format);
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size

			void SetVolume(Double vol);
			Bool StartSweep(Double startFreq, Double endFreq, UInt32 timeSeconds);
		};
	}
}
#endif
