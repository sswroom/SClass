#ifndef _SM_MEDIA_AFILTER_SOUNDGEN_BELLSOUNDGEN
#define _SM_MEDIA_AFILTER_SOUNDGEN_BELLSOUNDGEN
#include "Media/AFilter/SoundGen/SoundTypeGen.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AFilter
	{
		namespace SoundGen
		{
			class BellSoundGen : public SoundTypeGen
			{
			protected:
				UInt32 currSample;
				Double sampleVol;
				Sync::Mutex soundMut;
		
			public:
				BellSoundGen(UInt32 freq);
				virtual ~BellSoundGen();

				virtual void GenSignals(UnsafeArray<Double> buff, UIntOS sampleCnt);
				virtual SoundType GetSoundType();

				virtual Bool GenSound(Double sampleVol);
			};
		}
	}
}
#endif
