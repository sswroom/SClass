#ifndef _SM_MEDIA_AFILTER_SOUNDGENERATOR
#define _SM_MEDIA_AFILTER_SOUNDGENERATOR
#include "Data/FastMapNN.hpp"
#include "Media/AudioFilter.h"
#include "Media/AFilter/SoundGen/SoundTypeGen.h"

namespace Media
{
	namespace AFilter
	{
		class SoundGenerator : public AudioFilter
		{
		protected:
			AudioFormat format;
			Data::FastMapNN<Int32, Media::AFilter::SoundGen::SoundTypeGen> sndGenMap;
	
		public:
			SoundGenerator(NN<AudioSource> sourceAudio);
			virtual ~SoundGenerator();

			virtual void GetFormat(NN<AudioFormat> format);
			virtual UIntOS ReadBlock(Data::ByteArray blk); //ret actual block size

			Bool GenSound(Media::AFilter::SoundGen::SoundTypeGen::SoundType sndType, Double sampleVol);
		};
	}
}
#endif
