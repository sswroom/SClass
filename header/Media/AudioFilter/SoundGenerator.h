#ifndef _SM_MEDIA_AUDIOFILTER_SOUNDGENERATOR
#define _SM_MEDIA_AUDIOFILTER_SOUNDGENERATOR
#include "Data/FastMapNN.h"
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
			Data::FastMapNN<Int32, Media::AudioFilter::SoundGen::ISoundGen> sndGenMap;
	
		public:
			SoundGenerator(NN<IAudioSource> sourceAudio);
			virtual ~SoundGenerator();

			virtual void GetFormat(NN<AudioFormat> format);
			virtual UOSInt ReadBlock(Data::ByteArray blk); //ret actual block size

			Bool GenSound(Media::AudioFilter::SoundGen::ISoundGen::SoundType sndType, Double sampleVol);
		};
	}
}
#endif
