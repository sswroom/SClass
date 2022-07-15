#ifndef _SM_MEDIA_AUDIOFILTER_SOUNDGEN_BELLSOUNDGEN
#define _SM_MEDIA_AUDIOFILTER_SOUNDGEN_BELLSOUNDGEN
#include "Media/AudioFilter/SoundGen/ISoundGen.h"
#include "Sync/Mutex.h"

namespace Media
{
	namespace AudioFilter
	{
		namespace SoundGen
		{
			class BellSoundGen : public ISoundGen
			{
			protected:
				UInt32 currSample;
				Double sampleVol;
				Sync::Mutex soundMut;
		
			public:
				BellSoundGen(UInt32 freq);
				virtual ~BellSoundGen();

				virtual void GenSignals(Double *buff, UOSInt sampleCnt);
				virtual SoundType GetSoundType();

				virtual Bool GenSound(Double sampleVol);
			};
		}
	}
}
#endif
