#ifndef _SM_MEDIA_AUDIOFILTER_SOUNDGENERATOR
#define _SM_MEDIA_AUDIOFILTER_SOUNDGENERATOR
#include "Data/FastMap.h"
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
			Data::FastMap<Int32, Media::AudioFilter::SoundGen::ISoundGen*> sndGenMap;
	
		public:
			SoundGenerator(NotNullPtr<IAudioSource> sourceAudio);
			virtual ~SoundGenerator();

			virtual void GetFormat(NotNullPtr<AudioFormat> format);
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size

			Bool GenSound(Media::AudioFilter::SoundGen::ISoundGen::SoundType sndType, Double sampleVol);
		};
	}
}
#endif
