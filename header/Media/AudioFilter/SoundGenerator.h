#ifndef _SM_MEDIA_AUDIOFILTER_SOUNDGENERATOR
#define _SM_MEDIA_AUDIOFILTER_SOUNDGENERATOR
#include "Data/Integer32Map.h"
#include "Media/IAudioFilter.h"
#include "Media/AudioFilter/SoundGen/ISoundGen.h"

namespace Media
{
	namespace AudioFilter
	{
		class SoundGenerator : public IAudioFilter
		{
		protected:
			AudioFormat format;
			Data::Integer32Map<Media::AudioFilter::SoundGen::ISoundGen*> *sndGenMap;
	
		public:
			SoundGenerator(IAudioSource *sourceAudio);
			virtual ~SoundGenerator();

			virtual void GetFormat(AudioFormat *format);
			virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize); //ret actual block size

			Bool GenSound(Media::AudioFilter::SoundGen::ISoundGen::SoundType sndType, Double sampleVol);
		};
	}
}
#endif
